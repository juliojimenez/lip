#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.0.3"

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

int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 1) {
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
      total = total + number_of_nodes(t->children[i]);
    }
    return total;
  }
  return 0;
}

long eval(mpc_ast_t* t) {
  // If tagged as number return it directly
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }
  // The operator is always second child
  char* op = t->children[1]->contents;
  // We store the third child in x
  long x = eval(t->children[2]);
  // Iterate the remaining children and combining
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

// Use operator string to see which operation to perform
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
}

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
      // Print AST
      mpc_ast_print(r.output);
      // Load AST from output
      mpc_ast_t* a = r.output;
      printf("Tag: %s\n", a->tag);
      printf("Contents: %s\n", a->contents);
      printf("Number of children: %i\n", a->children_num);
      // Get first child
      mpc_ast_t* c0 = a->children[1];
      printf("First Child Tag: %s\n", c0->tag);
      printf("First Child Contents: %s\n", c0->contents);
      printf("First Child Number of Children: %i\n", c0->children_num);
      // Delete AST
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
