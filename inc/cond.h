#ifndef COND_H
#define COND_H
#include "reg.h"

long long handle_eq(long long val1_addr, long long val2_addr);
long long handle_neq(long long val1_addr, long long val2_addr);
long long handle_g(long long val1_addr, long long val2_addr);
long long handle_l(long long val1_addr, long long val2_addr);
long long handle_ge(long long val1_addr, long long val2_addr);
long long handle_le(long long val1_addr, long long val2_addr);

#endif