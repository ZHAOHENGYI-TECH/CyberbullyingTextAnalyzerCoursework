//We put functions,constants,structures here,as 'analyzer.h'
#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// The constants we use
#define MAX_WORD_LENGTH 100
#define MAX_LINE_LENGTH 1024
#define MAX_WORDS 10000
#define MAX_TOXIC_WORDS 1000
#define MAX_STOP_WORDS 500
#define MAX_FILES 10

// The toxicity level,we use a typedef enum
typedef enum {
    NON_TOXIC = 0,
    MILD_TOXIC = 1,
    MODERATE_TOXIC = 2,
    SEVERE_TOXIC = 3
} ToxicLevel;

// Word information structure,also use typedef struct
typedef struct {
    char word[MAX_WORD_LENGTH];
    int frequency;
    ToxicLevel toxic_level;
    int toxic_count;
} WordInfo;

// Analysis result structure
typedef struct {
    WordInfo* words;
    int word_count;
    int unique_words;
    int toxic_words_total;
    int toxic_mild;
    int toxic_moderate;
    int toxic_severe;
    int total_words;
    float toxicity_score;
    float lexical_diversity;
    float avg_sentence_length;
    char filename[256];
} AnalysisResult;

// Toxic word structure
typedef struct {
    char word[MAX_WORD_LENGTH];
    ToxicLevel level;
} ToxicWord;

// These are file handling functions
int load_file(const char* filename, char*** lines, int* line_count);
void free_lines(char** lines, int line_count);
bool file_exists(const char* filename);

// These are text processing functions
void tokenize_text(char* text, char*** tokens, int* token_count);
void normalize_word(char* word);
bool is_stop_word(const char* word);
int load_stop_words(const char* filename, char*** stop_words);
void remove_punctuation(char* text);

// These are toxic word detection functions
int load_toxic_words(const char* filename, ToxicWord** toxic_words);
ToxicLevel get_toxic_level(const char* word, ToxicWord* toxic_words, int toxic_count);
bool is_toxic_word(const char* word, ToxicWord* toxic_words, int toxic_count);
float calculate_toxicity_score(AnalysisResult* result);

// These are sorting algorithm functions
void quick_sort_words(WordInfo* words, int left, int right, int (*compare)(const void*, const void*));
void bubble_sort_words(WordInfo* words, int count, int (*compare)(const void*, const void*));
int compare_by_frequency(const void* a, const void* b);
int compare_alphabetically(const void* a, const void* b);
int compare_by_toxicity(const void* a, const void* b);

// These are analysis functions
AnalysisResult* analyze_text(char** lines, int line_count, const char* filename);
void add_word(AnalysisResult* result, const char* word, ToxicLevel level);
void calculate_statistics(AnalysisResult* result);
void print_top_words(AnalysisResult* result, int n, const char* title, int sort_type);

// These are user interface functions
void display_menu();
int get_user_choice();
void file_analysis();
void statistics_display();
void toxic_analysis();
void sorting_display();
void save_results();
void clear_screen();
void pause_screen();

// These are data persistence functions
int save_analysis_report(AnalysisResult* result, const char* filename);
int load_dictionaries();
void free_analysis_result(AnalysisResult* result);

// These are error handling functions
void file_error(const char* filename);
void memory_error();
void invalid_input();

// Global variables (defined in main.c)
extern ToxicWord* toxic_words;
extern int toxic_word_count;
extern char** stop_words;
extern int stop_word_count;
extern AnalysisResult* current_result;

#endif
