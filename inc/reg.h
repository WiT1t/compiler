#ifndef REG_H
#define REG_H

extern char registers_in_use[8];

int get_free_register();
void release_register(int reg);

#endif