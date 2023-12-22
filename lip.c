#include "mpc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION "0.0.4"

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

// Create enumeration of possible error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Create enumeration of possible lval types
enum { LVAL_NUM, LVAL_ERR };

// Declare new lval struct
typedef struct {
    int type;
    long num;
    int err;
} lval;

// Create a new number type lval
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

// Create a new error type lval
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

// Print an lval
void lval_print(lval v) {
    switch (v.type) {
        // In the case the type is a number, print it
        // then break out of the switch
        case LVAL_NUM: printf("%li", v.num); break;

        // In the case the type is an error...
        case LVAL_ERR:
            // Check what type of error it is and print it
            if (v.err == LERR_DIV_ZERO) {
                printf("Error: Division by Zero!");
            }
            if (v.err == LERR_BAD_OP) {
                printf("Error: Invalid Operator!");
            }
            if (v.err == LERR_BAD_NUM) {
                printf("Error: Invalid Number!");
            }
        break;
    }
}

// Print an lval, followed by a newline.
void lval_println(lval v) { lval_print(v); putchar('\n'); }

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

// Use operator string to see which operation to perform
lval eval_op(lval x, char* op, lval y) {
    // If either value is an error return it
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    // Otherwise do maths on the number values
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) {
        // If second operand is zero return error
        return y.num == 0
            ? lval_err(LERR_DIV_ZERO)
            : lval_num(x.num / y.num); 
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        // Check if there is some error in conversion
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
  
    char* op = t->children[2]->contents;
    // Helps with finding operators in nested forms
    if (strstr(t->children[2]->tag, "operator") == 0) {
        op = t->children[1]->contents;
    }
    // printf("op: %s\n", op);
  
    lval x = eval(t->children[3]);
    //printf("x: "); lval_println(x);
    
    int i = 4;
    // Helps with finding expressions in nested forms
    if (strstr(t->children[i]->tag, "expr") == 0) {
        printf("in\n");
        i = 3;
    }
    printf("tag: %s\n", t->children[i]->tag);
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        printf("loop x: "); lval_println(x);
        printf("loop op: %s\n", op);
        printf("loop tag: %s\n", t->children[i]->tag);
        i++;
        printf("loop tag+: %s\n", t->children[i]->tag);
    }
    return x;
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
      lip       : /^/ '(' <operator> <expr>+ ')' /$/ ;    \
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
      mpc_ast_t* c0 = a->children[2];
      printf("First Child Tag: %s\n", c0->tag);
      printf("First Child Contents: %s\n", c0->contents);
      printf("First Child Number of Children: %i\n", c0->children_num);
      // Eval
      lval result = eval(r.output);
      lval_println(result);
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
