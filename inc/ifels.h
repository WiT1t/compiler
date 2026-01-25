#ifndef IFELS_H
#define IFELS_H
#include "reg.h"
#include "cond.h"

int handle_if(long long cond_result_addr);
int handle_else(int jump_line);
void handle_endif(int jump_line);

#endif