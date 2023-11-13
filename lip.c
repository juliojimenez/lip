#include <stdio.h>

#define VERSION "0.0.1"

// Declare a buffer for user input of size 4096
static char input [4096];

int main(int argc, char** argv) {
    // Print version and exit information
    printf("Lip v%s\n", VERSION);
    puts("Press CTRL+c to Exit\n");

    // REPL
    while (1) {
        // Output lip prompt
        fputs("lip > ", stdout);
        // Read a line of user input, maximum size 4096
        fgets(input, 4096, stdin);
        // Echo input back to user
        printf("Output: %s", input); 
    }
    return 0;
}
