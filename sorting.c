#include <stdio.h>
#include <string.h>

#define MAX_LEN 100
#define MAX_WORDS 5000

//Define a structure to store words.
typedef struct {
    char word[MAX_LEN]; 
    int frenquency;
    int isToxic;               
} WordStat;

WordStat allWords[MAX_WORDS];
int uniqueWordCount = 0;

//Exchange two word elements.
void swap(WordStat *a, WordStat *b) {
    WordStat temp = *a;
    *a=*b;
    *b=temp;
}

//Sorting these words by frenquency.
void sorting(WordStat arr[], int n){
    if(n<=1){
        return;
    }
    int i,j;
    for(i=0;i<n-1;i++){
        int swapcount=0;
        for(j=0;j<n-i-1;j++){
            if(arr[j].frenquency<arr[j+1].frenquency){
                swap(&arr[j],&arr[j+1]);
                swapcount=1;
            }
        }
        if(swapcount==0){
            break;
        }
    }
}

//Show these top N words after sorting them.
void display(WordStat sortedarr[], int totalcount, int N){
    printf("---");
    int limit=(N>totalcount)?totalcount:N;
    if(limit==0){
        printf("No words can be shown.\n");
        return;
    }
    for(int i=0;i<limit;i++){
        printf("%d.%s(frenquency of occurrence:%d)\n",
                i+1,
                sortedarr[i].word,
                sortedarr[i].frenquency);
    }
    printf("----------------------------------\n");
}

//Show these top N toxic words.
void display2(WordStat allwords[], int toxicwordcount, int N){
    WordStat toxicwords[MAX_WORDS];
    int toxiccount=0;
    for(int i=0;i<toxicwordcount;i++){
        if(allwords[i].isToxic==1){
            toxicwords[toxiccount]=allwords[i];
            toxiccount++;
        }
    }

    sorting(toxicwords, toxiccount);

    printf("\n");
    printf("---");

    int limit=(N>toxiccount)?toxiccount:N;
    if(limit==0){
        printf("Can not find the toxic words in the final of the text.\n");
        return;
    }

    for(int i=0;i<limit;i++){
        printf("%d.%s(frenquency of occurrence:%d)\n",
               i+1,
               toxicwords[i].word,
               toxicwords[i].frenquency);
    }
    printf("---------------------------------\n");
}


void display3(int totalwords, int uniquewords, int sentences){
    printf("\n---summary statistics---\n");
    printf("total words:%d\n",totalwords);
    printf("unique words:%d\n",uniquewords);
    printf("total sentences:%d\n",sentences);
    
    if(sentences>0){
        printf("Average sentence length:%.2f words",(double)totalwords/sentences);
    }
    if(totalwords>0){
        printf("Vocabulary Diversity Index:%.2f",(double)uniquewords/totalwords);
    }
    printf("---------------------------------\n");
}