/* tinywc_filestop.c  –-  极简 + 文件停用词，零堆分配 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_WORD  50000   /* 单词池大小 */
#define MAX_STOP  5000    /* 停用词池大小 */
#define MAX_LEN   64      /* 单个词最大长度 */
#define TOP_K     20

static char word_buf[MAX_WORD][MAX_LEN];  /* 单词池 */
static int  word_cnt[MAX_WORD];
static int  unique = 0;

static char stop_buf[MAX_STOP][MAX_LEN];  /* 停用词池 */
static int  stop_n = 0;

/* 线性查停用词 */
static int find_stop(const char *w)
{
    int i;
    for (i = 0; i < stop_n; ++i)
        if (strcmp(stop_buf[i], w) == 0) return 1;
    return 0;
}

/* 线性查找或插入单词；返回下标，满返回-1 */
static int find_or_add(const char *w)
{
    int i;
    for (i = 0; i < unique; ++i)
        if (strcmp(word_buf[i], w) == 0) return i;

    if (unique < MAX_WORD) {
        strncpy(word_buf[unique], w, MAX_LEN - 1);
        word_buf[unique][MAX_LEN - 1] = '\0';
        word_cnt[unique] = 0;
        return unique++;
    }
    return -1;
}

/* 选择排序输出前K */
static void sort_and_print(void)
{
    int i, j, best;
    for (i = 0; i < unique && i < TOP_K; ++i) {
        best = i;
        for (j = i + 1; j < unique; ++j)
            if (word_cnt[j] > word_cnt[best] ||
                (word_cnt[j] == word_cnt[best] &&
                 strcmp(word_buf[j], word_buf[best]) < 0))
                best = j;
        char tmp_s[MAX_LEN];
        int  tmp_c;
        strcpy(tmp_s, word_buf[i]);
        tmp_c = word_cnt[i];
        strcpy(word_buf[i], word_buf[best]);
        word_cnt[i] = word_cnt[best];
        strcpy(word_buf[best], tmp_s);
        word_cnt[best] = tmp_c;

        printf("%-15s %d\n", word_buf[i], word_cnt[i]);
    }
}

/* 读停用词文件，存到固定数组 */
static int load_stop(const char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (!fp) return 0;
    char line[MAX_LEN];
    while (stop_n < MAX_STOP && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;          /* 去换行 */
        if (strlen(line) == 0) continue;
        strncpy(stop_buf[stop_n], line, MAX_LEN - 1);
        stop_buf[stop_n][MAX_LEN - 1] = '\0';
        stop_n++;
    }
    fclose(fp);
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        puts("usage: tinywc_filestop <textfile>");
        return 1;
    }
    /* 1. 读停用词 */
    if (!load_stop("stopwords.txt"))
        puts("Warning: stopwords.txt not found or empty, skip filtering.");

    /* 2. 打开文本 */
    FILE *fp = fopen(argv[1], "r");
    if (!fp) { perror(argv[1]); return 1; }

    int  total = 0;
    char tok[MAX_LEN];
    int  len = 0;
    int  c;

    /* 3. 逐字符拼词 */
    while ((c = fgetc(fp)) != EOF) {
        if (isalpha(c)) {
            if (len < MAX_LEN - 1)
                tok[len++] = tolower(c);
        } else {
            if (len > 0) {
                tok[len] = '\0';
                if (!find_stop(tok)) {          /* 过滤 */
                    int idx = find_or_add(tok);
                    if (idx >= 0) {
                        word_cnt[idx]++;
                        total++;
                    }
                }
                len = 0;
            }
        }
    }
    /* 文件末尾 */
    if (len > 0) {
        tok[len] = '\0';
        if (!find_stop(tok)) {
            int idx = find_or_add(tok);
            if (idx >= 0) {
                word_cnt[idx]++;
                total++;
            }
        }
    }
    fclose(fp);

    /* 4. 结果 */
    printf("Total words (filtered): %d\n", total);
    printf("Unique words: %d\n", unique);
    puts("\nTop 20:");
    sort_and_print();
    return 0;
}