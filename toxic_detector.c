#include "analyzer.h"

//Load toxic vocabulary in the file
int load_toxic_words(const char* filename, ToxicWord** toxic_words) {
    FILE* file;
    char buffer[MAX_WORD_LENGTH];
    char word[MAX_WORD_LENGTH];
    int level;
    ToxicWord* words = NULL;
    int count = 0;
    int capacity = 100;
    
    if (!file_exists(filename)) {
        // If there is no toxic vocabulary file,and we need to use default toxic vocabulary
        printf("Warning: Toxic vocabulary file not found '%s'，Use default toxic vocabulary list\n", filename);
        
        // Default toxic vocabulary (We classified vocablary by severity)
        struct {
            char* word;
            ToxicLevel level;
        } default_toxic_words[] = {
            // These words are severely toxic vocabulary
            {"fuck", SEVERE_TOXIC}, {"shit", SEVERE_TOXIC}, {"damn", SEVERE_TOXIC},
            {"bitch", SEVERE_TOXIC}, {"ass", SEVERE_TOXIC}, {"hell", SEVERE_TOXIC},
            {"crap", SEVERE_TOXIC}, {"stupid", MODERATE_TOXIC}, {"idiot", MODERATE_TOXIC},
            {"moron", MODERATE_TOXIC}, {"loser", MODERATE_TOXIC}, {"pathetic", MODERATE_TOXIC},
            {"useless", MODERATE_TOXIC}, {"worthless", MODERATE_TOXIC}, {"hate", MILD_TOXIC},
            {"annoying", MILD_TOXIC}, {"boring", MILD_TOXIC}, {"dumb", MILD_TOXIC},
            {"silly", MILD_TOXIC}, {"crazy", MILD_TOXIC}, {NULL, NON_TOXIC}
        };
        
        // Next,we need to calculate the default number of toxic words
        int default_count = 0;
        while (default_toxic_words[default_count].word != NULL) {
            default_count++;
        }
        
        // Allocate memory for pointers related to toxic vocabulary
        words = (ToxicWord*)malloc(default_count * sizeof(ToxicWord));
        if (!words) {
            return -1;
        }
        
        // Then, copy the toxic vocabulary
        for (int i = 0; i < default_count; i++) {
            strcpy(words[i].word, default_toxic_words[i].word);
            words[i].level = default_toxic_words[i].level;
        }
        
        *toxic_words = words;
        return default_count;
    }
    
    file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    
    words = (ToxicWord*)malloc(capacity * sizeof(ToxicWord));
    if (!words) {
        fclose(file);
        return -1;
    }
    
    while (fgets(buffer, MAX_WORD_LENGTH, file)) {
        buffer[strcspn(buffer, "\n\r")] = 0;  // Remove line breaks
        
        if (strlen(buffer) > 0) {
            // Parsing format: word, level
            if (sscanf(buffer, "%[^,],%d", word, &level) == 2) {
                if (count >= capacity) {
                    capacity *= 2;
                    ToxicWord* new_words = (ToxicWord*)realloc(words, capacity * sizeof(ToxicWord));
                    if (!new_words) {
                        free(words);
                        fclose(file);
                        return -1;
                    }
                    words = new_words;
                }
                
                strcpy(words[count].word, word);
                normalize_word(words[count].word);
                
                // We need to ensure that the level is within the effective range
                if (level >= 1 && level <= 3) {
                    words[count].level = (ToxicLevel)level;
                } else {
                    words[count].level = MODERATE_TOXIC;  // Default moderate toxicity
                }
                count++;
            }
        }
    }
    
    fclose(file);
    *toxic_words = words;
    return count;
}

// We check if the word is toxic
bool is_toxic_word(const char* word, ToxicWord* toxic_words, int toxic_count) {
    if (!toxic_words || toxic_count == 0 || !word) {
        return false;
    }
    
    for (int i = 0; i < toxic_count; i++) {
        if (strcasecmp(word, toxic_words[i].word) == 0) {
            return true;
        }
    }
    return false;
}

// Obtain the severity level of toxic vocabulary
ToxicLevel get_toxic_level(const char* word, ToxicWord* toxic_words, int toxic_count) {
    if (!toxic_words || toxic_count == 0 || !word) {
        return NON_TOXIC;
    }
    
    for (int i = 0; i < toxic_count; i++) {
        if (strcasecmp(word, toxic_words[i].word) == 0) {
            return toxic_words[i].level;
        }
    }
    return NON_TOXIC;
}

// And then calculate toxicity score
float calculate_toxicity_score(AnalysisResult* result) {
    if (!result || result->total_words == 0) {
        return 0.0f;
    }
    
    // The following are the steps to calculate the weighted toxicity score
    float weighted_toxic_words = 
        result->toxic_mild * 1.0f +           // Mild toxicity weight 1
        result->toxic_moderate * 2.0f +       // Moderate toxicity weight 1
        result->toxic_severe * 3.0f;          // Heavy toxicity weight 3
    
    // Calculate as follows: Toxicity score=(weighted toxicity words/total words) * 100
    float score = (weighted_toxic_words / result->total_words) * 100.0f;
    
    return score;
}

// Then we analyze the text
AnalysisResult* analyze_text(char** lines, int line_count, const char* filename) {
    AnalysisResult* result;
    char** tokens = NULL;
    int token_count = 0;
    int total_sentences = 0;
    int total_chars = 0;
    
    if (!lines || line_count <= 0) {
        return NULL;
    }
    
    // Allocate result structure memory
    result = (AnalysisResult*)malloc(sizeof(AnalysisResult));
    if (!result) {
        handle_memory_error();
        return NULL;
    }
    
    // Initialize result structure
    memset(result, 0, sizeof(AnalysisResult));
    result->words = (WordInfo*)malloc(MAX_WORDS * sizeof(WordInfo));
    if (!result->words) {
        free(result);
        handle_memory_error();
        return NULL;
    }
    
    strcpy(result->filename, filename ? filename : "unknown");
    
    // We use a for loop to process each line
    for (int i = 0; i < line_count; i++) {
        if (strlen(lines[i]) == 0) {
            continue;
        }
        
        total_sentences++;  // Simply consider each line as a sentence
        total_chars += strlen(lines[i]);
        
        // participle
        tokenize_text(lines[i], &tokens, &token_count);
        
        if (tokens && token_count > 0) {
            // Process each word
            for (int j = 0; j < token_count; j++) {
                if (strlen(tokens[j]) == 0 || is_stop_word(tokens[j])) {
                    continue;
                }
                
                result->total_words++;
                
                // Check if it is a toxic vocabulary
                ToxicLevel level = get_toxic_level(tokens[j], toxic_words, toxic_word_count);
                
                // Add to vocabulary list
                add_word(result, tokens[j], level);
                
                free(tokens[j]);  // Release segmentation memory
            }
            
            free(tokens);
            tokens = NULL;
        }
    }
    
    // Calculate statistical information
    calculate_statistics(result);
    
    // Calculate the average sentence length
    if (total_sentences > 0) {
        result->avg_sentence_length = (float)total_chars / total_sentences;
    }
    
    return result;
}

// Then, we add words to the analysis results
void add_word(AnalysisResult* result, const char* word, ToxicLevel level) {
    if (!result || !word || strlen(word) == 0) {
        return;
    }
    
    // Check if the word already exists
    for (int i = 0; i < result->word_count; i++) {
        if (strcmp(result->words[i].word, word) == 0) {
            result->words[i].frequency++;
            if (level > NON_TOXIC) {
                result->words[i].toxic_count++;
                result->words[i].toxic_level = level;
            }
            return;
        }
    }
    
    //If it is a new word, add it to the list
    if (result->word_count < MAX_WORDS) {
        strcpy(result->words[result->word_count].word, word);
        result->words[result->word_count].frequency = 1;
        result->words[result->word_count].toxic_level = level;
        result->words[result->word_count].toxic_count = (level > NON_TOXIC) ? 1 : 0;
        result->word_count++;
        result->unique_words++;
        
        // We have updated the statistics of toxic vocabulary and used the swift structure
        if (level > NON_TOXIC) {
            result->toxic_words_total++;
            switch (level) {
                case MILD_TOXIC:
                    result->toxic_mild++;
                    break;
                case MODERATE_TOXIC:
                    result->toxic_moderate++;
                    break;
                case SEVERE_TOXIC:
                    result->toxic_severe++;
                    break;
                default:
                    break;
            }
        }
    }
}

// Calculate statistical information
void calculate_statistics(AnalysisResult* result) {
    if (!result || result->total_words == 0) {
        return;
    }
    
    // Calculate vocabulary diversity
    result->lexical_diversity = (float)result->unique_words / result->total_words;
    
    // Calculate toxicity score
    result->toxicity_score = calculate_toxicity_score(result);
}

// Finally, it is necessary to release the memory of the analysis results
void free_analysis_result(AnalysisResult* result) {
    if (result) {
        if (result->words) {
            free(result->words);
        }
        free(result);
    }
}
