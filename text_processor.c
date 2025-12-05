//We use the code in this file to process the text.Its a boring step
#include "analyzer.h"

// First,load stop words
int load_stop_words(const char* filename, char*** stop_words) {
    FILE* file;
    char buffer[MAX_WORD_LENGTH];
    char** words = NULL;
    int count = 0;
    int capacity = 100;
    
    if (!file_exists(filename)) {
        // If the stop words file is not found, use the default stop words
        printf("Warning: Stop words file '%s' not found, using default stop word list\n", filename);
        
        // These are default stop words
        char* default_stop_words[] = {
            "the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", 
            "by", "from", "up", "about", "into", "through", "during", "before", "after", 
            "above", "below", "between", "among", "is", "are", "was", "were", "be", "been", 
            "being", "have", "has", "had", "do", "does", "did", "will", "would", "could", 
            "should", "may", "might", "must", "can", "this", "that", "these", "those", "i", 
            "you", "he", "she", "it", "we", "they", "me", "him", "her", "us", "them", "my", 
            "your", "his", "her", "its", "our", "their", NULL
        };
        
        // Calculate the number of default stop words
        int default_count = 0;
        while (default_stop_words[default_count] != NULL) {
            default_count++;
        }
        
        // Allocate memory
        words = (char**)malloc(default_count * sizeof(char*));
        if (!words) {
            return -1;
        }
        
        // Copy stop words
        for (int i = 0; i < default_count; i++) {
            words[i] = (char*)malloc((strlen(default_stop_words[i]) + 1) * sizeof(char));
            if (!words[i]) {
                for (int j = 0; j < i; j++) {
                    free(words[j]);
                }
                free(words);
                return -1;
            }
            strcpy(words[i], default_stop_words[i]);
        }
        
        *stop_words = words;
        return default_count;
    }
    
    file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    
    words = (char**)malloc(capacity * sizeof(char*));
    if (!words) {
        fclose(file);
        return -1;
    }
    
    while (fgets(buffer, MAX_WORD_LENGTH, file)) {
        buffer[strcspn(buffer, "\n\r")] = 0;  // Remove newline characters
        
        if (strlen(buffer) > 0) {
            if (count >= capacity) {
                capacity *= 2;
                char** new_words = (char**)realloc(words, capacity * sizeof(char*));
                if (!new_words) {
                    free_lines(words, count);
                    fclose(file);
                    return -1;
                }
                words = new_words;
            }
            
            words[count] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
            if (!words[count]) {
                free_lines(words, count);
                fclose(file);
                return -1;
            }
            strcpy(words[count], buffer);
            count++;
        }
    }
    
    fclose(file);
    *stop_words = words;
    return count;
}

// Check if a word is a stop word
bool is_stop_word(const char* word) {
    if (!stop_words || stop_word_count == 0) {
        return false;
    }
    
    for (int i = 0; i < stop_word_count; i++) {
        if (strcasecmp(word, stop_words[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Normalize a word (convert to lowercase)
void normalize_word(char* word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
}

// Remove punctuation
void remove_punctuation(char* text) {
    int i, j = 0;
    for (i = 0; text[i]; i++) {
        if (!ispunct(text[i]) || text[i] == '\'') {
            text[j++] = text[i];
        }
    }
    text[j] = '\0';
}

// Tokenize text 
void tokenize_text(char* text, char*** tokens, int* token_count) {
    char** token_array = NULL;
    int count = 0;
    int capacity = 50;
    char* token;
    char* temp_text;
    
    token_array = (char**)malloc(capacity * sizeof(char*));
    if (!token_array) {
        *tokens = NULL;
        *token_count = 0;
        return;
    }
    
    // Create a copy of the text to avoid modifying the original
    temp_text = (char*)malloc((strlen(text) + 1) * sizeof(char));
    if (!temp_text) {
        free(token_array);
        *tokens = NULL;
        *token_count = 0;
        return;
    }
    strcpy(temp_text, text);
    
    // Remove punctuation
    remove_punctuation(temp_text);
    
    // Tokenize using spaces
    token = strtok(temp_text, " \t\n\r");
    while (token != NULL) {
        if (strlen(token) > 0) {
            // Check if resizing is needed
            if (count >= capacity) {
                capacity *= 2;
                char** new_tokens = (char**)realloc(token_array, capacity * sizeof(char*));
                if (!new_tokens) {
                    for (int i = 0; i < count; i++) {
                        free(token_array[i]);
                    }
                    free(token_array);
                    free(temp_text);
                    *tokens = NULL;
                    *token_count = 0;
                    return;
                }
                token_array = new_tokens;
            }
            
            // Normalize word
            normalize_word(token);
            
            // Allocate memory and copy token
            token_array[count] = (char*)malloc((strlen(token) + 1) * sizeof(char));
            if (!token_array[count]) {
                for (int i = 0; i < count; i++) {
                    free(token_array[i]);
                }
                free(token_array);
                free(temp_text);
                *tokens = NULL;
                *token_count = 0;
                return;
            }
            strcpy(token_array[count], token);
            count++;
        }
        token = strtok(NULL, " \t\n\r");
    }
    
    free(temp_text);
    *tokens = token_array;
    *token_count = count;
}
