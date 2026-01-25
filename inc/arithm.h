#ifndef ARITHM_H
#define ARITHM_H
#include "reg.h"

extern int first_mult;
extern int mult_start;
extern int first_div;
extern int div_start;

long long handle_plus(long long val1_addr, long long val2_addr);
long long handle_minus(long long val1_addr, long long val2_addr);
long long handle_div(long long val1_addr, long long val2_addr);
long long handle_star(long long val1_addr, long long val2_addr);
long long handle_mod(long long val1_addr, long long val2_addr);

#endif