#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>
#include "node.h"

void tree_write(node_t* root, FILE* stream);
void tree_write_file(node_t* root, char const* fname);

node_t* tree_read(char const* fname);
#endif
