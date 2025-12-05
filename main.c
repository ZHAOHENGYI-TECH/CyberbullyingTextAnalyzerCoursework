//This is our main loop of the project,also the core of the whole project
#include "analyzer.h"

// These global variables we use
ToxicWord* toxic_words = NULL;
int toxic_word_count = 0;
char** stop_words = NULL;
int stop_word_count = 0;
AnalysisResult* current_result = NULL;

// Main function
int main() {
    int choice;

    // Then we load dictionary files,which is a text file filled with toxic words found online
    if (load_dictionaries() != 0) {
        fprintf(stderr, "Error: Failed to load dictionary files\n");
        return 1;
    }
    
    printf("Dictionaries loaded successfully!\n");
    printf("- Toxic words count: %d\n", toxic_word_count);
    printf("- Stop words count: %d\n\n", stop_word_count);

    printf("Development Team:B&G\n");
    printf("  - HENGYI ZHAO (Student ID: 20792131)\n");
    printf("  - JINBO WANG (Student ID: 20795326)\n");
    printf("  - LINGYU LI (Student ID: 20791542)\n");

    // Here is the main loop, we use the switch to deal it
    while (1) {
        display_menu();
        choice = get_user_choice();
        
        switch (choice) {
            case 1:
                file_analysis();
                break;
            case 2:
                statistics_display();
                break;
            case 3:
                toxic_analysis();
                break;
            case 4:
                sorting_display();
                break;
            case 5:
                save_results();
                break;
            case 6:
                printf("\nThank you for using the Cyberbullying Text Analyzer!\n");
                printf("Cleaning up resources...\n");
                
                // Here,clean up resources
                if (current_result) {
                    free_analysis_result(current_result);
                }
                if (toxic_words) {
                    free(toxic_words);
                }
                if (stop_words) {
                    free_lines(stop_words, stop_word_count);
                }
                
                return 0;
            default:
                invalid_input();
                break;
        }
        
        pause_screen();
    }
    
    return 0;
}
