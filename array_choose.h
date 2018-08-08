#ifndef _ARRAY_CHOOSE_H
#define _ARRAY_CHOOSE_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int choose(int n, int k);

void make_zeros(int* a, int len);

void make_val(int* a, int a_len, int val);

#endif
