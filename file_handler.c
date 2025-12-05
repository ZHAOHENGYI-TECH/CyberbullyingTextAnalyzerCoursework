#include "analyzer.h"

// This is start of a file load step,we check if a file exists
bool file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Then we load a text file by finding the path of the file
int load_file(const char* filename, char*** lines, int* line_count) {
    FILE* file;
    char buffer[MAX_LINE_LENGTH];
    char** temp_lines = NULL;
    int count = 0;
    int capacity = 100;  // Initial capacity
    
    if (!file_exists(filename)) {
        file_error(filename);
        return -1;
    }
    
    file = fopen(filename, "r");
    if (!file) {
        file_error(filename);
        return -1;
    }
    
    // In this step,use mallocto allocate initial memory
    temp_lines = (char**)malloc(capacity * sizeof(char*));
    if (!temp_lines) {
        memory_error();
        fclose(file);
        return -1;
    }
    
    // So far so good,read file line by line and do others 
    while (fgets(buffer, MAX_LINE_LENGTH, file)) {
        // Remove trailing newline characters
        buffer[strcspn(buffer, "\n\r")] = 0;
        
        // If the line is not empty, save it
        if (strlen(buffer) > 0) {
            // Check if resizing is needed
            if (count >= capacity) {
                capacity *= 2;
                char** new_lines = (char**)realloc(temp_lines, capacity * sizeof(char*));
                if (!new_lines) {
                    memory_error();
                    free_lines(temp_lines, count);
                    fclose(file);
                    return -1;
                }
                temp_lines = new_lines;
            }
            
            // Allocate memory and copy the line content
            temp_lines[count] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
            if (!temp_lines[count]) {
                memory_error();
                free_lines(temp_lines, count);
                fclose(file);
                return -1;
            }
            strcpy(temp_lines[count], buffer);
            count++;
        }
    }
    
    fclose(file);
    
    *lines = temp_lines;
    *line_count = count;
    
    return 0;
}

// We must free memory for lines,avoid injury the memory
void free_lines(char** lines, int line_count) {
    if (lines) {
        for (int i = 0; i < line_count; i++) {
            if (lines[i]) {
                free(lines[i]);
            }
        }
        free(lines);
    }
}

// And at last,deal with these possible errors 
// Handle file errors
void file_error(const char* filename) {
    fprintf(stderr, "Error: Cannot access file '%s'\n", filename);
    fprintf(stderr, "Please check if the file exists and you have read permissions.\n");
}

// Handle memory errors
void memory_error() {
    fprintf(stderr, "Error: Out of memory\n");
    fprintf(stderr, "The program needs more memory to continue.\n");
}

// Handle invalid input
void invalid_input() {
    printf("Invalid choice! Please enter a number between 1 and 6.\n");
}
