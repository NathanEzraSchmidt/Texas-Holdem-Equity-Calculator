#ifndef _TH_EQUITY_CALC_H
#define _TH_EQUITY_CALC_H

/*
get winners ignoring suits, returns 0 for quads and full house, otherwise 1
*/

int rank_winners(int n, int* r_len, int* best_p, int* ranks, int* p_ranks, int* found_straight, int* bool_straight);

/*
flushes
*/

/*
given distribution of board ranks (param *ranks), and hole cards for each player, get which suits can make flushes with potentially 3, 4 or 5 of each suit on board
*/

void flush_info(int* ranks, int* ranks_avail, int* ranks_short, int ranks_short_len, int* which_suit, int* flush_mems, int* no_flush_mems);


/*

in func name, "diff" refers to number of hole cards for each player whose suits differ from whichever suit is used to make flush
returns number of combos of all best flushes in each case
*/

int count_flushes_diff_0(int* flush_mems, int len);

int count_flushes_diff_1(int* flush_mems, int* no_flush_mems, int n, int k);

int count_flushes_diff_2(int* flush_mems, int* no_flush_mems, int* five_three_big, int* five_three_small);


/*
straight flushes
in func names, "none", "one", and "two" refer to how many board cards are not used in straight flush.
*/

void one_off_1(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* suits, int* flush_totals);

void none_off_1(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* suits, int* flush_totals);

void one_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals);

void two_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals, int* five_three_big, int* five_three_small);

void none_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals);

void none_off_five(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* best_suit_card, int* suits, int* flush_totals);

#endif
