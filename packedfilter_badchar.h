//
// Created by michael on 9/12/20.
//

//#include "include/main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>


#define LAST_BITS_ON 0x101010101010101UL
#define SIGNIFICANT_BITS_ON 0x8080808080808080UL
#define hassetbyte(v) ((~(v) - 0x0101010101010101UL) & (v) & 0x8080808080808080UL)
#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7
#define bitcount(q) (q + q/255) & 255
#define count(v) result += bitcount(v)
#define xnor(t,q,c) ~(t ^ q * (c)) & ~(t ^ LAST_BITS_ON * (c))
#define distance(x) ffsll(x >> 1) / 8
#define NO_OF_CHARS 256

void badCharheuristic(unsigned char *str, int size, int badchar[NO_OF_CHARS], int last_query) {
    int i;

    for (i = 0; i < NO_OF_CHARS; i++){
        badchar[i] = last_query;
    }

    for (i = 0; i < size; i++){
        badchar[(int) str[i]] = i;
    }
}

union Window {
    uint64_t* i;
    unsigned char* c;
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
};

union Test {
    char* c;
    //char l[8];
    uint64_t i;
};

int search_test (unsigned char* query_array,
            int query_len,
            unsigned char* st,
            int text_len) {

    int badchar[NO_OF_CHARS];

    int mismatch_move_count = 0;
    int result = 0;
    int text_offset = 0;
    int true_distance = 0;
    int end_position = 0;

    unsigned char* char_ptr = &query_array[0];
    unsigned char* last_char = &query_array[query_len-1];
    unsigned char* first_char = &query_array[0];

    union Window t_w;
    t_w.c = &st[0];

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;

/*    union Test a;
    union Test b;
    a.c = "abcdefghijklmnopqrstuvwxyz";
    int diff = 'g' - 'm';
    b.c = "ghijklmn";

    int i = a.i - b.i;

    if(hassetbyte(~(i - (diff * LAST_BITS_ON)))) {
        bool correct = true;
    }
    */

    while(!(&*t_w.c > &st[text_len-1])) {
          value = xnor(*t_w.i, query_matches, *char_ptr);
          //value = ~(*t_w.i - (*char_ptr * LAST_BITS_ON));

        if((bool)(query_matches = reduce(value) & query_matches)) {

            if (&char_ptr[0] == last_char) {
                count(query_matches);
                char_ptr = first_char;
                //end_position = 7 - (__builtin_clzll(query_matches)/8);

                //text_offset += query_len + end_position;
                //t_w.c = &st[text_offset];
                if(query_len > 1) {
                    t_w.c += query_len;
                } else{
                    t_w.c += 8;
                }
            } else {
                char_ptr = last_char;
                t_w.c = t_w.c + query_len - 1;
            }
        } else {
            char_ptr = first_char;
            inverse_matches.i = (~query_matches.i & LAST_BITS_ON);
            // whenever a failed state happens, find the furthest distance you can travel via badchar
            // This is allowed because we have verified that there are no better options
            bc.o[0] = query_len - badchar[st[text_offset.o[0]]] * inverse_matches.c[0];
            bc.o[1] = query_len - badchar[st[text_offset.o[1]]] * inverse_matches.c[1];
            bc.o[2] = (query_len - badchar[st[text_offset.o[2]]]) * inverse_matches.c[2];
            bc.o[3] = query_len - badchar[st[text_offset.o[3]]] * inverse_matches.c[3];
            bc.o[4] = query_len - badchar[st[text_offset.o[4]]] * inverse_matches.c[4];
            bc.o[5] = query_len - badchar[st[text_offset.o[5]]] * inverse_matches.c[5];
            bc.o[6] = query_len - badchar[st[text_offset.o[6]]] * inverse_matches.c[6];
            bc.o[7] = query_len - badchar[st[text_offset.o[7]]] * inverse_matches.c[7];



            text_offset += badchar[match_comparison];
            t_w.c = &st[text_offset];
            query_matches = LAST_BITS_ON;
        }
    }
    //Return False if the text is searched and nothing is found.
    return result;

}


