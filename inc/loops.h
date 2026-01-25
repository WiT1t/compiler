#ifndef LOOPS_H
#define LOOPS_H

void handle_RU(int loop_start_line); //REPEAT -- UNTIL --
void handle_while(int cond_line, int patch_line);
int handle_up_for(long long it_addr, long long limit_addr);
int handle_endfor_up(int patch_line, long long it_addr);
int handle_down_for(long long it_addr, long long limit_addr);
int handle_endfor_down(int patch_line, long long it_addr);
long long handle_iterator(const char* name, long long start_val_addr);

#endif