// In this file,we can output a complete file as the result of the analyzer.
#include "analyzer.h"
#include <time.h>

// First,load dictionary files and load words.
int load_dictionaries() {
    // Load toxic words
    toxic_word_count = load_toxic_words("toxicwords.txt", &toxic_words);
    if (toxic_word_count < 0) {
        fprintf(stderr, "Error: Failed to load toxic words file\n");
        return -1;
    }
    
    // Load stop words
    stop_word_count = load_stop_words("stopwords.txt", &stop_words);
    if (stop_word_count < 0) {
        fprintf(stderr, "Error: Failed to load stop words file\n");
        if (toxic_words) {
            free(toxic_words);
        }
        return -1;
    }
    
    return 0;
}

// Then,we can start writing the report text
// There are 6 parts in our report,i think its a clear and absolutely report
// A perfect report is important and necessary for our project
int save_analysis_report(AnalysisResult* result, const char* filename) {
    FILE* file;
    time_t now;
    struct tm *local;
    
    if (!result || !filename) {
        return -1;
    }
    
    file = fopen(filename, "w");
    if (!file) {
        return -1;
    }
    
    // First,we get current time
    time(&now);
    local = localtime(&now);
    
    // (1)Write report header
    fprintf(file, "============================================\n");
    fprintf(file, "    Cyberbullying Text Analysis Report\n");
    fprintf(file, "============================================\n");
    fprintf(file, "Generated at: %s", asctime(local));
    fprintf(file, "Analyzed File: %s\n", result->filename);
    fprintf(file, "============================================\n\n");
    
    // (2)Write basic statistics
    fprintf(file, "Basic Statistics:\n");
    fprintf(file, "--------------------------------------------\n");
    fprintf(file, "Total Words: %d\n", result->total_words);
    fprintf(file, "Unique Words: %d\n", result->unique_words);
    fprintf(file, "Lexical Diversity Index: %.4f\n", result->lexical_diversity);
    fprintf(file, "Average Sentence Length: %.2f characters\n", result->avg_sentence_length);
    fprintf(file, "\n");
    
    // (3.1)Write toxic word statistics
    fprintf(file, "Toxic Word Statistics:\n");
    fprintf(file, "--------------------------------------------\n");
    fprintf(file, "Mildly Toxic Words: %d\n", result->toxic_mild);
    fprintf(file, "Moderately Toxic Words: %d\n", result->toxic_moderate);
    fprintf(file, "Severely Toxic Words: %d\n", result->toxic_severe);
    fprintf(file, "Total Toxic Words: %d\n", result->toxic_words_total);
    fprintf(file, "Overall Toxicity Score: %.2f%%\n", result->toxicity_score);
    
    // (3.2)Toxicity level assessment
    fprintf(file, "Toxicity Level Assessment: ");
    if (result->toxicity_score < 1.0f) {
        fprintf(file, "Safe (below 1%%)\n");
    } else if (result->toxicity_score < 3.0f) {
        fprintf(file, "Minor (1-3%%)\n");
    } else if (result->toxicity_score < 6.0f) {
        fprintf(file, "Moderate (3-6%%)\n");
    } else {
        fprintf(file, "High Risk (over 6%%)\n");
    }
    fprintf(file, "\n");
    
    //*The core of our report
    // (4.1)Write top frequent words (sorted by frequency)
    fprintf(file, "Top 20 Most Frequent Words:\n");
    fprintf(file, "--------------------------------------------\n");
    
    // (4.2)Create a sorted copy
    WordInfo* sorted_words = (WordInfo*)malloc(result->word_count * sizeof(WordInfo));
    if (sorted_words) {
        memcpy(sorted_words, result->words, result->word_count * sizeof(WordInfo));
        quick_sort_words(sorted_words, 0, result->word_count - 1, compare_by_frequency);
        
        fprintf(file, "%-20s %-10s %-15s\n", "Word", "Frequency", "Toxicity Level");
        fprintf(file, "%-20s %-10s %-15s\n", "----", "---------", "--------------");
        
        int display_count = (20 > result->word_count) ? result->word_count : 20;
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
            
            fprintf(file, "%-20s %-10d %-15s\n", 
                    sorted_words[i].word, 
                    sorted_words[i].frequency,
                    toxic_level_str);
        }
        
        free(sorted_words);
    }
    fprintf(file, "\n");
    
    // (5)Write toxic word details
    if (result->toxic_words_total > 0) {
        fprintf(file, "Toxic Word Details:\n");
        fprintf(file, "--------------------------------------------\n");
        
        // Sort by toxicity level
        sorted_words = (WordInfo*)malloc(result->word_count * sizeof(WordInfo));
        if (sorted_words) {
            memcpy(sorted_words, result->words, result->word_count * sizeof(WordInfo));
            quick_sort_words(sorted_words, 0, result->word_count - 1, compare_by_toxicity);
            
            fprintf(file, "%-20s %-10s %-15s %-15s\n", "Word", "Frequency", "Toxicity Level", "Toxic Count");
            fprintf(file, "%-20s %-10s %-15s %-15s\n", "----", "---------", "--------------", "-----------");
            
            // Display only toxic words
            for (int i = 0; i < result->word_count && sorted_words[i].toxic_level > NON_TOXIC; i++) {
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
                
                fprintf(file, "%-20s %-10d %-15s %-15d\n", 
                        sorted_words[i].word, 
                        sorted_words[i].frequency,
                        toxic_level_str,
                        sorted_words[i].toxic_count);
            }
            
            free(sorted_words);
        }
    }
    
    // (6)Write report footer
    fprintf(file, "\n============================================\n");
    fprintf(file, "Report generation complete\n");
    fprintf(file, "============================================\n");
    
    fclose(file);
    return 0;
}
