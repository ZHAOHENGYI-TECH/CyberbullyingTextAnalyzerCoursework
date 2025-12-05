// Here,we use three sort methods to deal it 
#include "analyzer.h"

// The first one,comparison function for sorting by frequency
int compare_by_frequency(const void* a, const void* b) {
    WordInfo* word_a = (WordInfo*)a;
    WordInfo* word_b = (WordInfo*)b;
    
    // Sort in descending order of frequency
    return word_b->frequency - word_a->frequency;
}

// The second one,comparison function for sorting alphabetically
int compare_alphabetically(const void* a, const void* b) {
    WordInfo* word_a = (WordInfo*)a;
    WordInfo* word_b = (WordInfo*)b;
    
    return strcmp(word_a->word, word_b->word);
}

// And the third one,comparison function for sorting by toxicity level
int compare_by_toxicity(const void* a, const void* b) {
    WordInfo* word_a = (WordInfo*)a;
    WordInfo* word_b = (WordInfo*)b;
    
    // First, sort by toxicity level (Severe > Moderate > Mild > Normal)
    if (word_a->toxic_level != word_b->toxic_level) {
        return word_b->toxic_level - word_a->toxic_level;
    }
    
    // If toxicity levels are the same, sort by toxic word frequency
    return word_b->toxic_count - word_a->toxic_count;
}

// Then,swap the positions of two words
void swap_words(WordInfo* a, WordInfo* b) {
    WordInfo temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function for quick sort
int partition(WordInfo* words, int left, int right, int (*compare)(const void*, const void*)) {
    WordInfo pivot = words[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (compare(&words[j], &pivot) <= 0) {
            i++;
            swap_words(&words[i], &words[j]);
        }
    }
    
    swap_words(&words[i + 1], &words[right]);
    return i + 1;
}

// Quick sort implementation
void quick_sort_words(WordInfo* words, int left, int right, int (*compare)(const void*, const void*)) {
    if (left < right) {
        int pivot_index = partition(words, left, right, compare);
        quick_sort_words(words, left, pivot_index - 1, compare);
        quick_sort_words(words, pivot_index + 1, right, compare);
    }
}

// Bubble sort implementation
void bubble_sort_words(WordInfo* words, int count, int (*compare)(const void*, const void*)) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (compare(&words[j], &words[j + 1]) > 0) {
                swap_words(&words[j], &words[j + 1]);
            }
        }
    }
}

// Print the top N words
void print_top_words(AnalysisResult* result, int n, const char* title, int sort_type) {
    if (!result || !result->words || result->word_count == 0) {
        printf("No vocabulary data to display.\n");
        return;
    }
    
    // Create a copy of the word array to avoid modifying the original data
    WordInfo* sorted_words = (WordInfo*)malloc(result->word_count * sizeof(WordInfo));
    if (!sorted_words) {
        memory_error();
        return;
    }
    
    memcpy(sorted_words, result->words, result->word_count * sizeof(WordInfo));
    
    // Use a switch,select sorting algorithm and comparison function based on sort type
    switch (sort_type) {
        case 1:  // Sort by frequency
            printf("\n=== %s (Sorted by Frequency) ===\n", title);
            quick_sort_words(sorted_words, 0, result->word_count - 1, compare_by_frequency);
            break;
            
        case 2:  // Sort alphabetically
            printf("\n=== %s (Sorted Alphabetically) ===\n", title);
            bubble_sort_words(sorted_words, result->word_count, compare_alphabetically);
            break;
            
        case 3:  // Sort by toxicity level
            printf("\n=== %s (Sorted by Toxicity) ===\n", title);
            quick_sort_words(sorted_words, 0, result->word_count - 1, compare_by_toxicity);
            break;
            
        default:
            printf("\n=== %s ===\n", title);
            break;
    }
    
    // Last,determine the number to display
    int display_count = (n > result->word_count) ? result->word_count : n;
    
    // Display results,divide them by the toxic level
    printf("%-20s %-10s %-10s %-15s\n", "Word", "Frequency", "Toxicity", "Toxic Count");
    printf("%-20s %-10s %-10s %-15s\n", "----", "---------", "--------", "-----------");
    
    for (int i = 0; i < display_count; i++) {
        const char* toxic_level_str;
        switch (sorted_words[i].toxic_level) {
            case MILD_TOXIC:
                toxic_level_str = "Mild";
                break;
            case MODERATE_TOXIC:
                toxic_level_str = "Moderate";
                break;
            case SEVERE_TOXIC:
                toxic_level_str = "Severe";
                break;
            default:
                toxic_level_str = "Normal";
                break;
        }
        
        printf("%-20s %-10d %-10s %-15d\n", 
               sorted_words[i].word, 
               sorted_words[i].frequency,
               toxic_level_str,
               sorted_words[i].toxic_count);
    }
    
    free(sorted_words);
}
