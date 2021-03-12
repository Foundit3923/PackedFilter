//
// Created by michael on 1/23/21.
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
#define HALF_BITS_ON_1 0x1000100UL
#define HALF_BITS_ON_2 0x10001UL
#define hassetbyte(v) ((~(v) - 0x0101010101010101UL) & (v) & 0x8080808080808080UL)
#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7

union Window {
    uint64_t* i;
    unsigned char* c;
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
}Query;

union Index {
    uint64_t* i;
    unsigned char* c;
}Index;

union MultiChar {
    uint64_t* i;
    unsigned char* c;
}MultiChar;

union Matches {
    uint64_t* i;
    unsigned char* c;
}Matches;

union Offset {
    uint64_t* i;
    int o[8];
};

int search_test (unsigned char* query_array,
                 int query_len,
                 unsigned char* st,
                 int text_len) {

    int mismatch_move_count = 0;
    int result = 0;
    int last_query_location = query_len - 1;
    int text_section_size = floor(text_len / 8);
    union Offset text_offset;
    text_offset.o[0] = text_section_size;
    text_offset.o[1] = floor(text_section_size * 2);
    text_offset.o[2] = floor(text_section_size * 3);
    text_offset.o[3] = floor(text_section_size * 4);
    text_offset.o[4] = floor(text_section_size * 5);
    text_offset.o[5] = floor(text_section_size * 6);
    text_offset.o[6] = floor(text_section_size * 7);
    text_offset.o[7] = floor(text_section_size * 8);

    union Offset query_offset;
    query_offset.o[0] = last_query_location;
    query_offset.o[1] = last_query_location;
    query_offset.o[2] = last_query_location;
    query_offset.o[3] = last_query_location;
    query_offset.o[4] = last_query_location;
    query_offset.o[5] = last_query_location;
    query_offset.o[6] = last_query_location;
    query_offset.o[7] = last_query_location;

    int stack[text_len];
    int stack_count = 0;


    unsigned char* char_ptr;

    union Window t_w;
    t_w.c = &st[0];

    union Index idx;

    union Index last_idx;
    *last_idx.i = LAST_BITS_ON * (query_len - 1);

    union MultiChar m_c;

    union Matches query_matches;
    query_matches.i = LAST_BITS_ON;

    union Matches last_query_match;
    uint64_t value;
    uint64_t match_count;
    uint64_t finished_queries;
    uint64_t finsihed_qeuries_offset;
    uint64_t finished_queries_offset_plus_query_len;

    union Matches match_comparison;

    char_ptr = &query_array[0];

    while(text_offset.o[7] <= (text_len-query_len) && (*query_matches.i != 0UL)) {
        *last_query_match.i = *query_matches.i;

        //increment text_offset
        text_offset.o[0] += query_matches.c[0];
        text_offset.o[1] += query_matches.c[1];
        text_offset.o[2] += query_matches.c[2];
        text_offset.o[3] += query_matches.c[3];
        text_offset.o[4] += query_matches.c[4];
        text_offset.o[5] += query_matches.c[5];
        text_offset.o[6] += query_matches.c[6];
        text_offset.o[7] += query_matches.c[7];

        //increment query_offset
        query_offset.o[0] -= query_matches.c[0];
        query_offset.o[1] -= query_matches.c[1];
        query_offset.o[2] -= query_matches.c[2];
        query_offset.o[3] -= query_matches.c[3];
        query_offset.o[4] -= query_matches.c[4];
        query_offset.o[5] -= query_matches.c[5];
        query_offset.o[6] -= query_matches.c[6];
        query_offset.o[7] -= query_matches.c[7];

        //build multimask

        query_matches.c[0] = query_array[query_offset.o[0]];
        query_matches.c[1] = query_array[query_offset.o[1]];
        query_matches.c[2] = query_array[query_offset.o[2]];
        query_matches.c[3] = query_array[query_offset.o[3]];
        query_matches.c[4] = query_array[query_offset.o[4]];
        query_matches.c[5] = query_array[query_offset.o[5]];
        query_matches.c[6] = query_array[query_offset.o[6]];
        query_matches.c[7] = query_array[query_offset.o[7]];

        //build text
        t_w.c[0] = st[text_offset.o[0]];
        t_w.c[1] = st[text_offset.o[1]];
        t_w.c[2] = st[text_offset.o[2]];
        t_w.c[3] = st[text_offset.o[3]];
        t_w.c[4] = st[text_offset.o[4]];
        t_w.c[5] = st[text_offset.o[5]];
        t_w.c[6] = st[text_offset.o[6]];
        t_w.c[7] = st[text_offset.o[7]];

        //check multi-mask against text
        value = ~((*t_w.i) ^ *query_matches.i);

        //if match is found
        if((bool)(*query_matches.i = reduce(value))) {
            //if pattern is present at current shift
            finished_queries = (*query_offset.i - *query_matches.i) & ~((LAST_BITS_ON * -1) - 1);
            if (finished_queries) {
                //s += (s+m < n)? m-badchar[txt[s+m]] : 1;
                finsihed_qeuries_offset = *text_offset.i & (finished_queries * 255);
                finished_queries_offset_plus_query_len = finsihed_qeuries_offset + (finished_queries * query_len);
                while (finished_queries_offset_plus_query_len > 0) {

                }

            } else {
                //s += max(1, j - badchar[txt[s+j]]);
            }
        } else {
            //s += max(1, j - badchar[txt[s+j]]);
        }
    }
    //We return False if the text is searched and nothing is found.
    return result;

}


