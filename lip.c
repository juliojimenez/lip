#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.0.1"

// if we are compiling on windows compile these functions
#ifdef _WIN32
#include <string.h>

// Declare a buffer for user input of size 4096
// static makes it local to this file
static char input [4096];

// Fake readline function
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 4096, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1];
    return cpy;
}

// Fake add_history function
void add_history(char* unused) {}

// Otherwise include the editline headers
#else
#include <editline/readline.h>
#endif

int main(int argc, char** argv) {
    // Print version and exit information
    printf("Lip v%s\n", VERSION);
    puts("Press CTRL+c to Exit\n");
    
    // REPL
    while (1) {
        // Output lip prompt and get input
        char* input = readline("lip > ");
        // Add input to history
        add_history(input);
        // Echo input back to user
        printf("Output: %s\n", input);
        // Free retrieved input
        free(input);
    }
    return 0;
}
