#include "array_choose.h"
#include "th_equity_calc.h"

int rank_winners(int n, int* r_len, int* best_p, int* ranks, int* p_ranks, int* found_straight, int* bool_straight)
{
    // hand types: 0 quads; 1 full house; 2 straight; 3 trips; 4 two pair; 5 one pair; 6 high card;

    int quads[16];
    int trips[32];
    int pairs[48];
    int singles[128];

    int quad_len[16];
    int trip_len[16];
    int pair_len[16];
    int single_len[16];
    int straight_counts[16];
    int straight[16];

    int best_hand = 4;
    int best_p_len = 0;

    make_zeros(quad_len, n);
    make_zeros(trip_len, n);
    make_zeros(pair_len, n);
    make_zeros(single_len, n);
    make_zeros(straight_counts, n);
    make_zeros(bool_straight, n);

    *found_straight = 0;

    // quads = 0; trips = 1; 2 pair = 2; one pair = 3;

    for (int i = 0; i < 13; ++i){
        int found_new = 0;
        for (int j = 0; j < n; ++j){
            int c = p_ranks[j*13+i] + ranks[i];
            if (!c) {
                straight_counts[j] = 0;
                continue;
            }
            if (!bool_straight[j]){
                ++straight_counts[j];
                if (straight_counts[j] == 5){
					straight[j] = i;
					bool_straight[j] = 1;
                    *found_straight = 1;
                }
				else if (i == 12 && straight_counts[j] == 4 && (ranks[0] || p_ranks[j*13])){
					straight[j] = 13;
					bool_straight[j] = 1;
                    *found_straight = 1;
				}
            }
            if (c == 1){
               singles[7*j + single_len[j]] = i;
               ++single_len[j];
            }
            else if (c == 2){
                pairs[3*j + pair_len[j]] = i; ++pair_len[j];
                if (pair_len[j] == 2){
                    if (best_hand > 2){
                        best_hand = 2;
                        found_new = 1;
                        best_p[0] = j;
                        best_p_len = 1;
                    }
                    else if (best_hand == 2){
                        best_p[best_p_len] = j;
                        ++best_p_len;
                    }
                }
                else {
                    if (best_hand > 3){
                        best_hand = 3;
                        found_new = 1;
                        best_p[0] = j;
                        best_p_len = 1;
                    }
                    else if (best_hand == 3 && found_new){
                        best_p[best_p_len] = j;
                        ++best_p_len;
                    }
                }
            }
            else if (c == 3){
                trips[2*j + trip_len[j]] = i;
                ++trip_len[j];
                if (best_hand > 1){
                    best_hand = 1;
                    found_new = 1;
                    best_p[0] = j;
                    best_p_len = 1;
                }
                else if (best_hand == 1 && found_new){
                    best_p[best_p_len] = j;
                    ++best_p_len;
                }
            }
            else {
                quads[j] = i; ++quad_len[j];
                if (best_hand > 0){
                    best_hand = 0;
                    found_new = 1;
                    best_p[0] = j;
                    best_p_len = 1;
                }
                else if (best_hand == 0 && found_new){
                    best_p[best_p_len] = j;
                    ++best_p_len;
                }
            }
        }
    }
    // quads = 0; straight = 1; trips = 2; two pair = 3; one pair = 4;

    if (best_hand == 1){
        int len = 0, card1, card2;
        int fh[2];

        for (int i = 0; i < n; ++i){
            if (trip_len[i] == 1 && pair_len[i]){
                card1 = trips[2*i]; card2 = pairs[3*i];}
            else if (trip_len[i] == 2){
                card1 = trips[2*i];
                if (pair_len[i]){
                    card2 = MIN(trips[2*i+1], pairs[3*i]); }
                else
                    card2 = trips[2*i+1];
            }
            else
                continue;
            if (!len || card1 < fh[0] || (card1 == fh[0] && card2 < fh[1])){
                fh[0] = card1; fh[1] = card2;
                best_p[0] = i;
                len = 1;
            }
            else if (card1 == fh[0] && card2 == fh[1]){
                best_p[len] = i;
                ++len;
            }
        }
        if (len) {
            *r_len = len;
            return 0;
        }
    }
    if (best_hand == 0){
        if (best_p_len == 1) {
            *r_len = 1;
            return 0;
        }
        else {
            int player, len = 0;
            int quad_card = quads[best_p[0]];
            for (int i = 0; i < 13; ++i){
                if (i == quad_card)
                    continue;
                for (int j = 0; j < best_p_len; ++j){
                    player = best_p[j];
                    if (ranks[i] || p_ranks[player*13 + i]){
                        best_p[len] = player;
                        ++len;
                    }
                }
                if (len) {
                    *r_len = len;
                    return 0;
                }
            }
        }
    }
    if (*found_straight){
        int len = 0;
        int best = 13;
        for (int i = 0; i < n; ++i)
        {
            if (!bool_straight[i])
                continue;
            if (straight[i] < best){
                best_p[0] = i; len = 1;
                best = straight[i];
            }
            else if (straight[i] == best){
                best_p[len] = i;
                ++len;
            }
        }
        *r_len = len;
        return 1;
    }
    if (best_p_len == 1) {
        *r_len = 1;
        return 1;
        }

    if (best_hand == 2){ // two pair
        int player;
        int card;
        int len = 1;
        int best;
        for (int i = 0; i < 2; ++i){
            best = 13;
            for (int j = 0; j < best_p_len; ++j){
                player = best_p[j];
                card = pairs[3*player + i];
                if (card < best){
                    best = card;
                    best_p[0] = player;
                    len = 1;
                }
                else if (card == best){
                    best_p[len] = player;
                    ++len;
                }
            }
            if (len == 1){
                *r_len = 1;
                return 1;
            }
            best_p_len = len;
        }
        best = 13;
        for (int i = 0; i < best_p_len; ++i){
            player = best_p[i];
            if (pair_len[player] == 3){
                card = MIN(pairs[3*player + 2], singles[7*player]);
            }
            else {
                card = singles[7*player];
            }
            if (card < best){
                best = card;
                best_p[0] = player;
                len = 1;
            }
            else if (card ==  best){
                best_p[len] = player;
                ++len;
            }
        }
        *r_len = len;
        return 1;
    }
    int m;
    if (best_hand == 1)
        m = 2; // trips
    else if (best_hand == 3)
        m = 3; // one pair
    else {
        best_p_len = n;
        m = 5;
        for (int i = 0; i < n; ++i){
            best_p[i] = i;
        }
    }
    int player, card, best, len;
    for (int i = 0; i < m; ++i){
        best = 13; len = 0;
        for (int j = 0; j < best_p_len; ++j){
            player = best_p[j];
            card = singles[7*player + i];
            if (card < best){
                best = card;
                best_p[0] = player; len = 1;
            }
            else if (card == best){
                best_p[len] = player;
                ++len;
            }
        }
        best_p_len = len;
        if (best_p_len == 1){
            *r_len = 1;
            return 1;
        }
    }
    *r_len = len;
    return 1;
}

int count_flushes_diff_0(int* flush_mems, int len)
{
    int big = 1;
    for (int i = 0; i < len; ++i){ // small
        big *= flush_mems[i];
    }
    return big;
}

int count_flushes_diff_1(int* flush_mems, int* no_flush_mems, int n, int k)
{
    int big;
    int t = 0;
    for (int i = 0; i < n; ++i){ // small
        big = 1;
        for (int j = i+1; j < n; ++j)
            big *= flush_mems[j];
        for (int j = 0; j < i; ++j)
            big *= flush_mems[j];
        t += big*no_flush_mems[i];
    }
    return t;
}

int count_flushes_diff_2(int* flush_mems, int* no_flush_mems, int* five_three_big, int* five_three_small)
{
    int big;
    int small;
    int t = 0;
    int bi = 0;
    int si = 0;

    for (int i = 0; i < 10; ++i){ // small
        big = 1; small = 1;

        for (int j = 0; j < 3; ++j)
            big *= flush_mems[five_three_big[bi+j]];
        for (int j = 0; j < 2; ++j)
            small *= no_flush_mems[five_three_small[si+j]];

        t += big*small;
        bi += 3;
        si += 2;
    }
    return t;
}

/*
flush info
*/

void flush_info(int* ranks, int* ranks_avail, int* ranks_short, int ranks_short_len, int* which_suit, int* flush_mems, int* no_flush_mems) // modify mems to choose values for one suit
{
    int i, x, y, b, card; // card is rank
    for (i = 0; i < ranks_short_len; ++i){
        card = ranks_short[i];
        b = which_suit[card];
        x = ranks_avail[card];
        y = ranks[card];

        flush_mems[i] = choose(x - 1, y - 1)*b;
        no_flush_mems[i] = choose(x - b, y);
    }
}

void flush_info_none(int* ranks, int* ranks_avail, int* ranks_short, int ranks_short_len, int* which_suit, int* flush_mems) // modify mems to choose values for one suit
{
    int i, x, y, b, card; // card is rank
    for (i = 0; i < ranks_short_len; ++i){
        card = ranks_short[i];
        b = which_suit[card];
        x = ranks_avail[card];
        y = ranks[card];

        flush_mems[i] = choose(x - 1, y - 1)*b;
    }
}


/*
straight flushes
*/

void one_off_1(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* suits, int* flush_totals)
{
    int i = -1;
    int* suit;
    int c;
    int player[16];
    int counts[16];
    int which_player;
    int avail;
    int off_count;
    int suited_count;
    int off_rank;
    int len;

    new_suit:

        ++i;
        if (i == 4)
            return;
        len = 0;
        suit = suits + i*13;

        for (int j = 0; j < n; ++j){
            if (p_suit[i*16 + j] && bool_straight[j]){
                player[len] = j; ++len;
            }
        }
        if (!len)
            goto new_suit;
        c = -1;

    new_off:
        for (int j = 0; j < n; ++j)
            counts[j] = 0;
        ++c;
        if (c == ranks_short_len)
            goto new_suit;

        off_rank = ranks_short[c];

        avail = suit[off_rank];

        for (int a = 0; a < ranks_short_len; ++a){
            if (a == c)
                continue;
            int card = ranks_short[a];
            if (!suit[card])
                goto new_off;
        }

        for (int r = 0; r < 13; ++r){
            for (int j = 0; j < len; ++j){
                which_player = player[j];
                if (p_bool[which_player*52 + 13*i + r] || (ranks[r] && off_rank != r)){
                    ++counts[which_player];
                    if (counts[which_player] == 5)
                        goto _count;
                }
                else {counts[which_player] = 0;}
            }
        }
        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (counts[which_player] == 4 && (p_bool[which_player*52 + 13*i] || (ranks[0] && off_rank))){
                goto _count;}
        }
        goto new_off;
    _count:
        avail = suit[off_rank];
        off_count = choose(ranks_avail[off_rank] - avail, ranks[off_rank]);
        suited_count = 1;
        for (int cc = 0; cc < ranks_short_len; ++cc){
            if (cc == c)
                continue;
            suited_count *= choose(ranks_avail[ranks_short[cc]] - 1, ranks[ranks_short[cc]] - 1);
        }
        flush_totals[which_player] += suited_count*off_count;
        goto new_off;
}


void none_off_1(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* suits, int* flush_totals)
{
    int i = -1;
    int* suit;
    int len;
    int player[16];
    int which_player;
    int counts[16];
    int total;

    new_suit:
        ++i;
        if (i == 4)
            return;
        suit = suits + i*13;

        for (int j = 0; j < ranks_short_len; ++j){
            if (!suit[ranks_short[j]])
                goto new_suit;
        }
        len = 0;

        for (int j = 0; j < n; ++j){
            if (bool_straight[j] && p_suit[i*16 + j]){
                player[len] = j; ++len;
            }
        }
        if (!len)
            goto new_suit;

        for (int j = 0; j < n; ++j)counts[j] = 0;

        for (int r = 0; r < 13; ++r){
            for (int j = 0; j < len; ++j){
                which_player = player[j];
                if (ranks[r] || p_bool[which_player*52 + i*13 + r]){
                    ++counts[which_player];
                    if (counts[which_player] == 5)
                        goto _count;
                }
                else
                    counts[which_player] = 0;
            }
        }
        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (counts[which_player] == 4 && (ranks[0] || p_bool[which_player*52 + 13*i]))
                goto _count;
        }
        goto new_suit;
    _count:
        total = 1;
        for (int c = 0; c < ranks_short_len; ++c){
            int card = ranks_short[c];
            total *= choose(ranks_avail[card] - 1, ranks[card] - 1);
        }
        flush_totals[which_player] += total;
        goto new_suit;
}



void one_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals)

{
    int i = -1;
    int c;
    int player[16];
    int counts[16];
    int which_player;
    int avail;
    int best_reg;
    int off_rank;
    int off_count;
    int suited_count;
    int* suit;

    new_suit:
        ++i;
            if (i == 4)
                return;
        int len = 0;
        suit = suits + i*13;
        best_reg = best_suit[i];

        for (int j = 0; j < n; ++j){
            if (p_suit[i*16 + j] && bool_straight[j]){
                player[len] = j; ++len;
            }
        }
        if (!len || (len == 1 && player[0] == best_reg))
            goto new_suit;
        c = -1;

    new_off:
        for (int j = 0; j< n; ++j)
            counts[j] = 0;
        ++c;
        if (c == ranks_short_len)
            goto new_suit;

        off_rank = ranks_short[c];
        avail = suit[off_rank];

    for (int a = 0; a < ranks_short_len; ++a){
        if (a == c)
            continue;
        int card = ranks_short[a];
        if (!suit[card])
            goto new_off;
    }

    for (int r = 0; r < 13; ++r){
        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (p_bool[which_player*52 + 13*i + r] || (ranks[r] && off_rank != r)){
                ++counts[which_player];
                if (counts[which_player] == 5){
                    if (which_player == best_reg)
                        goto new_off;
                    else
                        goto _count;
                }
            }
            else {counts[which_player] = 0;}
        }
    }
    for (int j = 0; j < len; ++j){
        which_player = player[j];
        if (counts[which_player] == 4  && ((off_rank && ranks[0]) || p_bool[which_player*52 + 13*i])){
            if (which_player == best_reg)
                goto new_off;
            else
                goto _count;
        }
    }
    goto new_off;
    _count:
    avail = suit[off_rank];
    off_count = choose(ranks_avail[off_rank] - avail, ranks[off_rank]);
    suited_count = 1;
    for (int cc = 0; cc < ranks_short_len; ++cc){
        if (cc == c)
            continue;
        suited_count *= choose(ranks_avail[ranks_short[cc]] - 1, ranks[ranks_short[cc]] - 1);
    }

    flush_totals[best_reg] -= suited_count*off_count;
    flush_totals[which_player] += suited_count*off_count;
    goto new_off;
}

void two_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals, int* five_three_big, int* five_three_small)
{
   int i = -1;
   int c;
   int player[16];
   int counts[16];
   int which_player;
   int avail_1, avail_2, off_rank_1, off_rank_2;
   int best_reg, len;
   int off_count, suited_count;
   int* suit, *which_small, *which_big;

   new_suit:
       ++i; if (i == 4) return;

       suit = suits + i*13;
       best_reg = best_suit[i];
       len = 0;

       for (int j = 0; j < n; ++j){
            if (p_suit[i*16 + j] && bool_straight[j]){
                player[len] = j; ++len;
            }
       }
       if (!len || (len == 1 && player[0] == best_reg)) goto new_suit;
       c = -1;

    new_off:
        for (int j=0;j<n;++j)counts[j]=0;
        ++c;
        if (c == 10) goto new_suit;

        which_small = five_three_small + c*2;

        off_rank_1 = ranks_short[which_small[0]], off_rank_2 = ranks_short[which_small[1]];

        for (int a = 0; a < ranks_short_len; ++a){
            int card = ranks_short[a];
            if (!suit[card] && off_rank_1 != card && off_rank_2 != card) goto new_off;
        }

        for (int r = 0; r < 13; ++r){
            for (int j = 0; j < len; ++j){
                which_player = player[j];
                if (p_bool[which_player*52 + 13*i + r] || (ranks[r] && off_rank_1 != r && off_rank_2 != r)){
                    ++counts[which_player];
                    if (counts[which_player] == 5){
                        if (which_player == best_reg) goto new_off;
                        else goto _count;
                    }
                }
                else counts[which_player] = 0;
            }
        }
        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (counts[which_player] == 4  && ((off_rank_1 && off_rank_2 && ranks[0]) || p_bool[which_player*52 + 13*i])){
                if (which_player == best_reg) goto new_off;
                else goto _count;
            }
        }
        goto new_off;
    _count:
        avail_1 = suit[off_rank_1], avail_2 = suit[off_rank_2];
        which_big = five_three_big + c*3;
        suited_count = 1;
        for (int cc = 0; cc < 3; ++cc){
            int card = ranks_short[which_big[cc]];
            suited_count *= choose(ranks_avail[card] - 1, ranks[card] - 1);
        }

        off_count = choose(ranks_avail[ranks_short[which_small[0]]] - avail_1, ranks[ranks_short[which_small[0]]]);
        off_count *= choose(ranks_avail[ranks_short[which_small[1]]] - avail_2, ranks[ranks_short[which_small[1]]]);

        flush_totals[best_reg] -= suited_count*off_count;
        flush_totals[which_player] += suited_count*off_count;
        goto new_off;
}

void none_off(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail,  int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* suits, int* flush_totals)

{
    int i = -1;
    int* suit;
    int len;
    int player[16];
    int which_player;
    int best_reg;
    int counts[16];
    int total;

    new_suit:
        ++i; if (i == 4)
            return;
        suit = suits + i*13;

        for (int j = 0; j < ranks_short_len; ++j){
            if (!suit[ranks_short[j]])
                goto new_suit;
        }
        len = 0;

        for (int j = 0; j < n; ++j){
            if (bool_straight[j] && p_suit[i*16 + j]){
                player[len] = j;
                ++len;
            }
        }

        if (!len)
            goto new_suit;

        best_reg = best_suit[i];

        if (len == 1 && best_reg == player[0])
            goto new_suit;

        for (int j = 0; j < n; ++j)
            counts[j] = 0;

        for (int r = 0; r < 13; ++r){
            for (int j = 0; j < len; ++j){
                which_player = player[j];
                if (ranks[r] || p_bool[which_player*52 + i*13 + r]){
                    ++counts[which_player];
                    if (counts[which_player] == 5){
                        if (which_player == best_reg) goto new_suit;
                        else goto _count;
                    }
                }
                else counts[which_player] = 0;
            }
        }
        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (counts[which_player] == 4  && (ranks[0] || p_bool[which_player*52 + 13*i])){
                if (which_player == best_reg) goto new_suit;
                else goto _count;
            }
        }
        goto new_suit;
    _count:
        total = 1;
        for (int c = 0; c < ranks_short_len; ++c){
            int card = ranks_short[c];
            total *= choose(ranks_avail[card] - 1, ranks[card] - 1);
        }
        flush_totals[best_reg] -= total;
        flush_totals[which_player] += total;

        goto new_suit;
}

void none_off_five(int n, int ranks_short_len, int* bool_straight, int* ranks, int* ranks_avail, int* ranks_short, int *p_bool, int* p_suit, int* best_suit, int* best_suit_card, int* suits, int* flush_totals)
{
    int i = -1;
    int* suit;
    int len;
    int player[16];
    int which_player;
    int best_reg;
    int counts[16];

    int board_straight;

    if (ranks_short[4] - ranks_short[0] == 4)
        board_straight = ranks_short[0];
    else if (ranks_short[0] == 0 && ranks_short[1] == 9 && ranks_short[4] == 12)
        board_straight = 9;
    else
        board_straight = -1;
    if (!board_straight)
        return;

    new_suit:
        ++i;
        if (i == 4)
            return;
        suit = suits + i*13;
        for (int j = 0; j < ranks_short_len; ++j){
            if (!suit[ranks_short[j]])
                goto new_suit;
        }
        if (best_suit_card[i] > ranks_short[4])
            best_reg = n;
        else
            best_reg = best_suit[i];

        if (board_straight > 0){
            if (best_reg == n)
                goto new_suit;
            --flush_totals[best_reg];
            ++flush_totals[n];
            for (int j = 0; j < n; ++j){
                if (p_bool[j*52 + i*13 + board_straight - 1]){
                    --flush_totals[n];
                    ++flush_totals[j];
                    goto new_suit;
                }
            }
            goto new_suit;
        }

        len = 0;

        for (int j = 0; j < n; ++j){
            if (bool_straight[j] && p_suit[i*16 + j]){
                player[len] = j; ++len;
            }
        }

        if (!len)
            goto new_suit;

        if (len == 1 && best_reg == player[0])
            goto new_suit;

        for (int j = 0; j < n; ++j)
            counts[j] = 0;

        for (int r = 0; r < 13; ++r){
            for (int j = 0; j < len; ++j){
                which_player = player[j];
                if (ranks[r] || p_bool[which_player*52 + i*13 + r]){
                    ++counts[which_player];
                    if (counts[which_player] == 5){

                        if (which_player != best_reg){
                            --flush_totals[best_reg]; ++flush_totals[which_player];
                        }
                        goto new_suit;
                    }
                }
                else counts[which_player] = 0;
            }
        }

        for (int j = 0; j < len; ++j){
            which_player = player[j];
            if (counts[which_player] == 4 && (ranks[0] || p_bool[which_player*52 + i *13])){
                if (which_player != best_reg){
                    --flush_totals[best_reg]; ++flush_totals[which_player];
                }
                goto new_suit;
            }
        }
        goto new_suit;
}
