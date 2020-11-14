#ifndef PARSER_H
#define PARSER_H

#define PARSER_MAX_TOKENS 10

int const parser_pass(char const* fname, int pedantic);

int const parser_init();
void parser_free();
#endif
