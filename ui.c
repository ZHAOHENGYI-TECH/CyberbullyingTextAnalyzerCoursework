//In order to make our analyzer more concise and aesthetically pleasing, we pay special attention to the UI aspect.
#include "analyzer.h"

// We display the main menu as this,we have 6 steps through the analyzer
void display_menu() {
    printf("\n============================================\n");
    printf("              Main Menu\n");
    printf("============================================\n");
    printf("1. Load a text file for analysis\n");
    printf("2. Display general vocabulary statistics\n");
    printf("3. Display toxic word analysis\n");
    printf("4. Sort and display words\n");
    printf("5. Save analysis results to a file\n");
    printf("6. Exit the program\n");
    printf("============================================\n");
    printf("Please select an option (1-6): ");
}

// Get user's choice,reguler start at the first step
int get_user_choice() {
    int choice;
    char buffer[100];
    
    if (fgets(buffer, sizeof(buffer), stdin)) {
        if (sscanf(buffer, "%d", &choice) == 1) {
            return choice;
        }
    }
    return -1;  // Invalid input
}

// Handle file analysis,we get the sample file by their path
void file_analysis() {
    char filename[256];
    char** lines = NULL;
    int line_count = 0;
    
    printf("\nPlease enter the path to the text file to analyze: ");
    if (fgets(filename, sizeof(filename), stdin)) {
        filename[strcspn(filename, "\n\r")] = 0;  // Remove newline character
        
        if (strlen(filename) == 0) {
            printf("Error: Filename cannot be empty\n");
            return;
        }
        
        printf("Loading file '%s'...\n", filename);
        
        if (load_file(filename, &lines, &line_count) == 0) {
            printf("File loaded successfully! Total %d lines of text\n", line_count);
            
            // Free previous analysis results,avoid errror
            if (current_result) {
                free_analysis_result(current_result);
                current_result = NULL;
            }
            
            // Analyze text
            current_result = analyze_text(lines, line_count, filename);
            
            if (current_result) {
                printf("\nText analysis complete!\n");
                printf("Total words: %d\n", current_result->total_words);
                printf("Unique words: %d\n", current_result->unique_words);
                printf("Total toxic words: %d\n", current_result->toxic_words_total);
                printf("Toxicity score: %.2f%%\n", current_result->toxicity_score);
            } else {
                printf("Error: Text analysis failed\n");
            }
            
            free_lines(lines, line_count);
        } else {
            printf("Failed to load file '%s'\n", filename);
        }
    }
}

// Here,we display statistics,divide two parts
void statistics_display() {
    if (!current_result) {
        printf("Please load and analyze a text file first (Option 1)\n");
        return;
    }
    
    printf("\n============================================\n");//part1,total Statistics
    printf("           General Vocabulary Statistics Report\n");
    printf("============================================\n");
    printf("File: %s\n", current_result->filename);
    printf("--------------------------------------------\n");
    printf("Total words: %d\n", current_result->total_words);
    printf("Unique words: %d\n", current_result->unique_words);
    printf("Lexical diversity index: %.4f\n", current_result->lexical_diversity);
    printf("Average sentence length: %.2f characters\n", current_result->avg_sentence_length);
    printf("--------------------------------------------\n");
    printf("Toxic Word Statistics:\n");//part2,toxic word Statistics
    printf("  Mildly toxic: %d\n", current_result->toxic_mild);
    printf("  Moderately toxic: %d\n", current_result->toxic_moderate);
    printf("  Severely toxic: %d\n", current_result->toxic_severe);
    printf("  Total: %d\n", current_result->toxic_words_total);
    printf("--------------------------------------------\n");
    printf("Overall Toxicity Score: %.2f%%\n", current_result->toxicity_score);
    
    // Provide an assessment based on the toxicity score
    if (current_result->toxicity_score < 1.0f) {
        printf("Text Toxicity Level: Safe (below 1%%)\n");
    } else if (current_result->toxicity_score < 3.0f) {
        printf("Text Toxicity Level: Minor (1-3%%)\n");
    } else if (current_result->toxicity_score < 6.0f) {
        printf("Text Toxicity Level: Moderate (3-6%%)\n");
    } else {
        printf("Text Toxicity Level: High Risk (over 6%%)\n");
    }
}

// toxic word analysis
void toxic_analysis() {
    if (!current_result) {
        printf("Please load and analyze a text file first (Option 1)\n");
        return;
    }
    
    printf("\n============================================\n");
    printf("           Detailed Toxic Word Analysis\n");
    printf("============================================\n");
    
    if (current_result->toxic_words_total == 0) {
        printf("No toxic words detected!\n");
        return;
    }
    
    // Display toxic words sorted by toxicity level,three levels,so far so good
    print_top_words(current_result, 20, "Toxic Word List", 3);
    
    printf("\nToxicity Distribution:\n");
    printf("Mildly toxic words: %d (%.2f%%)\n", 
           current_result->toxic_mild, 
           (float)current_result->toxic_mild / current_result->toxic_words_total * 100);
    printf("Moderately toxic words: %d (%.2f%%)\n", 
           current_result->toxic_moderate, 
           (float)current_result->toxic_moderate / current_result->toxic_words_total * 100);
    printf("Severely toxic words: %d (%.2f%%)\n", 
           current_result->toxic_severe, 
           (float)current_result->toxic_severe / current_result->toxic_words_total * 100);
}

// Then,display sorting options and results,also three sorting methods
void sorting_display() {
    if (!current_result) {
        printf("Please load and analyze a text file first (Option 1)\n");
        return;
    }
    
    int sort_choice, n;
    
    printf("\nSorting Options:\n");
    printf("1. Sort by frequency\n");
    printf("2. Sort alphabetically\n");
    printf("3. Sort by toxicity level\n");
    printf("Please select a sorting method (1-3): ");
    
    sort_choice = get_user_choice();
    if (sort_choice < 1 || sort_choice > 3) {
        printf("Invalid choice!\n");
        return;
    }
    
    printf("How many top words to display? ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid input!\n");
        // Clear input buffer
        while (getchar() != '\n');
        return;
    }
    // Clear input buffer
    while (getchar() != '\n');
    
    // Display sorted results
    print_top_words(current_result, n, "Sorted Words", sort_choice);
}

// Display saving results,we set a default name and avoid the empty
void save_results() {
    if (!current_result) {
        printf("Please load and analyze a text file first (Option 1)\n");
        return;
    }
    
    char filename[256];
    printf("Enter the output filename (default: analysis_report.txt): ");
    
    if (fgets(filename, sizeof(filename), stdin)) {
        filename[strcspn(filename, "\n\r")] = 0;  // Remove newline character
        
        if (strlen(filename) == 0) {
            strcpy(filename, "analysis_report.txt");
        }
        
        if (save_analysis_report(current_result, filename) == 0) {
            printf("Analysis results have been successfully saved to '%s'\n", filename);
        } else {
            printf("Error saving the file!\n");
        }
    }
}

// Fianally,pause screen
void pause_screen() {
    printf("\nPress Enter to continue...");
    getchar();
}
