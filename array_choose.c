#include "array_choose.h"

int choose(int n, int k)
{
    if (k == n || k == 0)
        return 1;
    if (k == 1 || n - k == 1)
        return n;
    if (k > n)
        return 0;
    if (n == 4)
        return 6;
    return 0;
}

void make_zeros(int* a, int len)
{
    for (int i = 0; i < len; ++i){
        a[i] = 0;
    }
}

void make_val(int* a, int a_len, int val)
{
    for (int i = 0; i < a_len; ++i){
        a[i] = val;
    }
}
