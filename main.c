#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array_choose.h"
#include "th_equity_calc.h"

#define MAX_PLAYER_NUM 16

int get_card_num(char* card);

int get_player_cards(int* player, int which_player);

int get_all_players_cards(int* players);

void print_instructions();

int main()
{
    print_instructions();

    int players[32];
    int n;
    int ranks_avail[13];
    int p_ranks[MAX_PLAYER_NUM*13];
    int final_totals[256];
    int best_suit_1[4]; // positions correspond to suits, values correspond to which player (1 to n-1) has best flush when 4 of that suit are on board
    int best_suit_2[4]; // same as best_suit_1 except for when 3 of that suit are on board
    int best_suit_card[4];
    int suit[52];
    int suit_needed[4];
    int p_bool[52*MAX_PLAYER_NUM];

    int suit_player_1[4*MAX_PLAYER_NUM]; // 1st d is suits, second is player nos, bool list
    int suit_player_2[4*MAX_PLAYER_NUM];

    int five_three_big[30] = {0,1,2, 0,1,3, 0,1,4, 0,2,3, 0,2,4, 0,3,4, 1,2,3, 1,2,4, 1,3,4, 2,3,4}; // distributions of suits on board
    int five_three_small[20] = {3,4,   2,4,   2,3,   1,4,   1,3,   1,2,   0,4,   0,3,   0,2,   0,1};

    int bool_straight[MAX_PLAYER_NUM];
    int best_p[MAX_PLAYER_NUM];
    int best_p_len;
    int found_straight;
    int flush_totals[MAX_PLAYER_NUM+1];

    int flush_count;
    int ranks_short[5];
    int ranks_short_len;
    int hand_type;
    int combs;

    /*
    suit info
    */

    int flush_mems[8];
    int no_flush_mems[8];
    int needed;
    int flush_winner;
    int suit_diff;

    int card1, card2, rank1, rank2, suit1, suit2, temp;
    int best2[4];

    while (1) {

        n = get_all_players_cards(players);
        make_val(best2, 4, 13);
        make_val(ranks_avail, 13, 4);
        make_zeros(p_ranks, n*13);
        make_zeros(final_totals, 256);

        make_zeros(suit_player_1, MAX_PLAYER_NUM*4); //positions correspond to player and suit, values are 0 or 1 depending on whether or not suit is possible with 4 on board
        make_zeros(suit_player_2, MAX_PLAYER_NUM*4); //same as above except with 3 on board

        make_zeros(p_bool, 52*MAX_PLAYER_NUM);

        make_val(suit, 52, 1);
        make_val(suit_needed, 4, 5);

        make_val(best_suit_1, 4, n);
        make_val(best_suit_2, 4, n);
        make_val(best_suit_card, 4, 13);

        for (int i = 0; i < n; ++i){
            for (int j = 0; j < 2; ++j){
            ++p_ranks[13*i + players[i*2 + j]%13];
            }
        }

        //get which player would have best flush, for when there are both are 3 and 4 of each suit on board, and if flushes in such cases are possible
        for (int i = 0; i < n; ++i){
            card1 = players[i*2];
            card2 = players[i*2+1];
            p_bool[i*52 + card1] = 1;
            p_bool[i*52 + card2] = 1;
            rank1 = card1%13;
            rank2 = card2%13;
            if (rank1 > rank2){
                temp = card1;
                card1 = card2;
                card2 = temp;
                temp = rank1;
                rank1 = rank2;
                rank2 = temp;
            }
            --ranks_avail[rank1];
            --ranks_avail[rank2];

            suit[card1] = 0; suit[card2] = 0;
            rank1 = card1%13; rank2 = card2%13;
            suit1 = card1/13; suit2 = card2/13;
            if (suit1 == suit2){
                if (rank1 < best2[suit1]){
                    best2[suit1] = rank1;
                    best_suit_2[suit1] = i;
                }
                if (rank1 < best_suit_card[suit1]){
                    best_suit_card[suit1] = rank1;
                    best_suit_1[suit1] = i;
                }
                suit_needed[suit1] = 3;
                suit_player_2[suit1*MAX_PLAYER_NUM+i] = 1;
                suit_player_1[suit1*MAX_PLAYER_NUM+i] = 1;
                    }
            else {
                suit_needed[suit1] = MIN(suit_needed[suit1], 4);
                suit_needed[suit2] = MIN(suit_needed[suit2], 4);
                if (rank1 < best_suit_card[suit1]){
                    best_suit_card[suit1] = rank1;
                    best_suit_1[suit1] = i;
                }
                if (rank2 < best_suit_card[suit2]){
                    best_suit_card[suit2] = rank2;
                    best_suit_1[suit2] = i;
                }
                suit_player_1[suit1*MAX_PLAYER_NUM+i] = 1;
                suit_player_1[suit2*MAX_PLAYER_NUM+i] = 1;
            }
        }

        make_zeros(bool_straight, n);
        make_zeros(flush_totals, n+1);

        int ranks[13];
        int running_total = 0;
        int new_entry;
        int index = 0;
        int new_max = ranks_avail[0];

        double total_combs = 0;

        make_zeros(ranks, 13);

        forward: // start enumerating board ranks
            new_entry = MIN(5 - running_total, new_max);
            ranks[index] = new_entry;
            if (running_total + new_entry == 5){
                ranks_short_len = 0;
                for (int i = 0; i < 13; ++i){
                    if (ranks[i]){
                        ranks_short[ranks_short_len] = i;
                        ++ranks_short_len;
                    }
                }
                combs = 1; //number of combos of each distribution of board ranks
                for (int i = 0; i < ranks_short_len; ++i){
                    int card = ranks_short[i];
                    combs *= choose(ranks_avail[card], ranks[card]);
                }

                total_combs += combs;
                found_straight = 0;

                //account for best hand ignoring suits, return value hand_type is 0 if best hand is quads or full house, otherwise 1

                hand_type = rank_winners(n, &best_p_len, best_p, ranks, p_ranks, &found_straight, bool_straight);

                if (hand_type){
                    if (ranks_short_len == 3 || ranks_short_len == 4) { //account for flushes when flush tie is possible
                        make_zeros(flush_totals, n+1);
                        int* which_suit = suit;
                        for (int i = 0; i < 4; ++i){ // suits
                            flush_info(ranks, ranks_avail, ranks_short, ranks_short_len, which_suit, flush_mems, no_flush_mems);
                            needed = suit_needed[i];
                            if (needed == 3)
                                flush_winner = best_suit_2[i];
                            else
                                flush_winner = best_suit_1[i];
                            for (int j = needed; j < ranks_short_len + 1; ++j){
                                suit_diff = ranks_short_len - j;

                                switch (suit_diff){
                                case (0):
                                    flush_totals[flush_winner] += count_flushes_diff_0(flush_mems, ranks_short_len);
                                    break;
                                case (1):
                                    flush_totals[flush_winner] += count_flushes_diff_1(flush_mems, no_flush_mems, ranks_short_len, j);
                                    break;
                                default:
                                    flush_totals[flush_winner] += count_flushes_diff_2(flush_mems, no_flush_mems, five_three_big, five_three_small);
                                    break;
                                }
                                flush_winner = best_suit_1[i];
                            }
                            which_suit += 13;
                        }
                    }

                    //account for flushes when flush tie is possible

                    else if (ranks_short_len == 5){
                        make_zeros(flush_totals, n+1);
                        int* which_suit = suit;

                        for (int i = 0; i < 4; ++i){ // suits
                            flush_info(ranks, ranks_avail, ranks_short, 5, which_suit, flush_mems, no_flush_mems);
                            needed = suit_needed[i];
                            for (int j = needed; j < 5; ++j){
                                if (j == 3){
                                    flush_winner = best_suit_2[i];
                                    flush_totals[flush_winner] += count_flushes_diff_2(flush_mems, no_flush_mems, five_three_big, five_three_small);
                                }

                                else if (j == 4){
                                    flush_winner = best_suit_1[i];
                                    flush_totals[flush_winner] += count_flushes_diff_1(flush_mems, no_flush_mems, 5, 4);
                                }
                            }
                            if (best_suit_card[i] > ranks_short[4])
                                flush_winner = n;
                            int found = 1;
                            for (int j = 0; j < 5; ++j){
                                int card = ranks_short[j];
                                if (!which_suit[card]){
                                    found = 0;
                                    break;
                                }
                            }
                            flush_totals[flush_winner] += found;
                            which_suit += 13;
                        }
                    }
                    flush_count = 0;
                    for (int i = 0; i < n+1; ++i)
                        flush_count += flush_totals[i];

                    if (found_straight && flush_count) {// account for straight flushes, in cases where any flush would beat best rank hand, so only checking for when straight flush beats a higher flush
                        if (ranks_short_len == 5){
                            two_off(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_2, best_suit_2, suit, flush_totals, five_three_big, five_three_small);
                            one_off(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_1, best_suit_1, suit, flush_totals);
                            none_off_five(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_1, best_suit_1, best_suit_card, suit, flush_totals);
                        }
                        else if (ranks_short_len == 4){
                            one_off(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_2, best_suit_2, suit, flush_totals);
                            none_off(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_1, best_suit_1, suit, flush_totals);
                        }
                        else {
                            none_off(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_2, best_suit_2, suit, flush_totals);
                        }
                    }
                }
                else {
                    if (found_straight){
                        make_zeros(flush_totals, n+1);
                        if (ranks_short_len == 4) {
                            one_off_1(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_2, suit, flush_totals);
                            none_off_1(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_1, suit, flush_totals);
                        }
                        else {
                            none_off_1(n, ranks_short_len, bool_straight, ranks, ranks_avail, ranks_short, p_bool, suit_player_2, suit, flush_totals);
                        }
                    flush_count = 0;
                    for (int i = 0; i < n+1; ++i)
                        flush_count += flush_totals[i];
                    }
                    else {
                        flush_count = 0;
                    }
                }
                if (flush_count){
                   for (int i = 0; i < n; ++i){
                        final_totals[i*n] += flush_totals[i];
                   }
                   if (flush_totals[n]){
                       for (int i = 0; i < n; ++i){
                           final_totals[n*(i+1) - 1] += flush_totals[n];
                       }
                   }
                }
                for (int i = 0; i < best_p_len; ++i){
                    int player = best_p[i];
                    final_totals[n*player + best_p_len - 1] += combs - flush_count;
                }
            if (index == 12){
                goto backward;
            }
            else {
                --ranks[index];
                running_total += new_entry - 1;
                ++index;
                new_max = ranks_avail[index];
                goto forward;
            }
        }
        else if (index == 12)
            goto backward;
        else {
            running_total += new_entry;
            ++index;
            new_max = ranks_avail[index];
            goto forward;
        }
        backward:
            ranks[index] = 0;
            while (index > -1 && ranks[index] == 0)
                --index;
            if (index > -1){
                new_max = ranks[index] - 1;
                running_total -= ranks[index];
                goto forward;
            }
        printf("\ncombos:\n");
        for (int i = 0; i < n; ++i) {
            printf("player %d: ", i+1);
            for (int j = 0; j < n; ++j) {
                printf("%d ", final_totals[i*n + j]);
            }
            printf("\n\n");
        }
        printf("percentages:\n");
        for (int i = 0; i < n; ++i) {
            printf("player %d: ", i+1);
            for (int j = 0; j < n; ++j) {
                printf("%.4f ", 100*final_totals[i*n + j]/total_combs);
            }
            printf("\n\n");
        }
    }
    return 0;
}

int get_card_num(char* card)
{
    char* ranks = "akqjt98765432";
    char* suits = "dhsc";
    for (int i = 0; i < 4; ++i) {
        if (suits[i] == card[1]) {
            for (int j = 0; j < 13; ++j) {
                if (card[0] == ranks[j]) {
                    return j + 13*i;
                }
                if (j == 12)
                    return -1;
            }
        }
        if (i == 3)
            return -1;
    }
    return -1;
}

int get_player_cards(int* player, int which_player)
{
    //returns 1 if hand was entered, otherwise 0
    char hand[128];
    int c = 0;
    int i;
    get:
        ++c;
        if (c > 1) {
            printf("don't recognize input. enter hand using all lower case with no spaces. (e.g., th9d).\n", hand);
        }
        printf("player %d = ", which_player + 1);
        i = -1;

        do {
            ++i;
            scanf("%c", &hand[i]);
        } while (hand[i] != '\n');

        if (hand[0] == '\n')
            return 0;
        player[0] = get_card_num(hand);
        if (player[0] == -1)
            goto get;
        player[1] = get_card_num(&hand[2]);
        if (player[1] == -1)
            goto get;
        if (player[0] == player[1]) {
            puts("duplicate card found");
            c = 0;
            goto get;
        }
        for (int i = 0; i < which_player*2; ++i) {
            int x = player[-i-1];
            if (x == player[0] || x == player[1]) {
                puts("duplicate card found");
                c = 0;
                goto get;
            }
        }
    return 1;
}

int get_all_players_cards(int* players)
{

    for (int i = 0; i < 16; ++i) {
        if (get_player_cards(&players[i*2], i) == 0)
            return i;
    }
    return 16;
}

void print_instructions()
{
    puts("\n***********************\n");
    printf("Input Instructions:\n");
    printf("To input a player's hand, input the two hole cards with rank followed by suit in all lower case with no spaces in between and in any order (e.g. asqh). ");
    printf("Use \"t\" for \"10\" (e.g. khtc, and not kh10c). ");
    printf("Input hands for up to 16 players. When done, input nothing and just press enter when prompted for the next player's hand.\n");
    printf("\nAbout output:\n");
    printf("First the combinations and second and percentages of a win, 2-way tie, 3-way tie, etc. is given for each player.\n");
    puts("\n***********************\n");
}
