## COMP1028 Coursework
**COMP1028 Programming and Algorithm**

**Session: Autumn 2025**

### **B&G Group**

>**Zhao, Hengyi	20792131**

>**Wang, Jinbo	20795326**

>**Li, Lingyu	20791542**

### How to Compile

You will need the GCC compiler (MinGW-w64 on Windows). Navigate to the project's root directory in your terminal (like PowerShell) and run the following command:

>gcc.exe -g -Wall -I. -c *.c

>gcc.exe *.o -o analyzer.exe

This command compiles all .c files in the directory and links them together to create a single executable named analyzer.exe.

### Required Dictionary Files
For the program to function correctly, the following files must be present in the same directory as the analyzer.exe executable:

>stopwords.txt: A plain text file containing one stopword per line.

>toxicwords.txt: A plain text file containing one toxic word per line.

### Example Usage
Run analyzer.exe from the terminal: The program will display a menu of options. Enter the number corresponding to your desired action (e.g., '1' to analyze a new file) and press Enter. Follow the on-screen prompts to provide filenames or choose further options.

### Dependencies
The project uses standard C libraries and does not have any external dependencies. A
standard C compiler is all that is required.

### GitHub Link & Video Link
