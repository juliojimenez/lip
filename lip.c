#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.0.2"

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
  // Create some parsers
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lip       = mpc_new("lip");

  // Define them with the following language
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number    : /-?[0-9]+/ ;                            \
      operator  : '+' | '-' | '*' | '/' ;                 \
      expr      : <number> | '(' <operator> <expr>* ')' ; \
      lip       : /^/ '(' <operator> <expr>+ ')' /$/ ;            \
    ",
    Number, Operator, Expr, Lip);

  // Print version and exit information
  printf("Lip v%s\n", VERSION);
  puts("Press CTRL+c to Exit\n");
  
  // REPL
  while (1) {
    // Output lip prompt and get input
    char* input = readline("lip > ");
    // Add input to history
    add_history(input);
    // Attempt to parse the user input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lip, &r)) {
      // On success print and delete the AST
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      // Otherwise print and delete the error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    // Free retrieved input
    free(input);
  }
  // Undefine and delete our parsers
  mpc_cleanup(4, Number, Operator, Expr, Lip);
  return 0;
}
