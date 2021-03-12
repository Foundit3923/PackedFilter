//
// Created by michael on 1/21/21.
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

int search_test (unsigned char* query_array,
                 int query_len,
                 unsigned char* st,
                 int text_len) {

    int mismatch_move_count = 0;
    int result = 0;
    int first_index = 0;
    int last_index = 0;
    int offset_4;
    int offset_5;
    int offset_6;
    int offset_7;
    int stack[text_len];
    int stack_count = 0;
    int text_offset = 0;

    unsigned char* char_ptr;

    union Window t_w;
    t_w.c = &st[0];

    union Index idx;

    union Index last_idx;
    *last_idx.i = LAST_BITS_ON * (query_len - 1);

    union MultiChar m_c;

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;
    uint64_t match_count;

    char_ptr = &query_array[0];

    //mask for first char
    m_c.c[0] = query_array[0];
    m_c.c[1] = query_array[0];
    //mask for last char
    m_c.c[2] = query_array[query_len-1];
    m_c.c[3] = query_array[query_len-1];

    while(!(&*t_w.c > &st[text_len-1])) {
        //increment index
        *idx.i = (*idx.i & query_matches) + query_matches;

        //build multimask

        //mask to iterate through query
        m_c.c[4] = query_array[idx.c[4]];
        m_c.c[5] = query_array[idx.c[5]];
        m_c.c[6] = query_array[idx.c[6]];
        m_c.c[7] = query_array[idx.c[7]];

        //build text
        t_w.c[0] = st[first_index+1];
        t_w.c[1] = st[first_index];
        t_w.c[2] = st[last_index+1];
        t_w.c[3] = st[last_index];
        t_w.c[4] = st[offset_4];
        t_w.c[5] = st[offset_5];
        t_w.c[6] = st[offset_6];
        t_w.c[7] = st[offset_7];

        //check multi-mask against text
        value = (~((*t_w.i) ^ *m_c.i));

        //if match is found
        if((bool)(query_matches = reduce(value))) {
            //check for matches in search section
            if(query_matches & HALF_BITS_ON_1 && query_matches & HALF_BITS_ON_2) {
                stack[stack_count] = text_offset;
                stack[++stack_count] = text_offset + 1;
                //increase for next round
                stack_count++;
            } else if(query_matches & HALF_BITS_ON_1) {
                //store location
                stack[stack_count] = text_offset;
                stack_count++;
            } else if(query_matches & HALF_BITS_ON_2){
                //store location
                stack[stack_count] = text_offset + 1;
                stack_count++;
            } else {

            }
            //move forward by 2
            query_matches += HALF_BITS_ON_1;
            query_matches += HALF_BITS_ON_2;
            text_offset += 2;
            if((bool)(match_count = reduce(~(*idx.i ^ *last_idx.i)))) {
                result += (match_count + match_count / 255) & 255;
            }

        }
        //if no matches
        else {

        }

        value = (~((*t_w.i) ^ query_matches * (*char_ptr)) & ~((*t_w.i) ^ LAST_BITS_ON * (*char_ptr)));

        if((bool)(query_matches = reduce(value))) {
            mismatch_move_count++;
            text_offset++;
            if(&char_ptr[0] == &query_array[query_len-1]){
                result += (query_matches + query_matches/255) & 255;
                text_offset -= mismatch_move_count;
                mismatch_move_count = 0;
            }
            char_ptr++;
            t_w.c = &st[text_offset];
        } else {
            char_ptr = &query_array[0];
            text_offset += (8 - mismatch_move_count);
            t_w.c = &st[text_offset];
            mismatch_move_count = 0;
            query_matches = LAST_BITS_ON;
        }
    }
    //We return False if the text is searched and nothing is found.
    return result;

}


