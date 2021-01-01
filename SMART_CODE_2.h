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
#define hassetbyte(v) ((~(v) - 0x0101010101010101UL) & (v) & 0x8080808080808080UL)

union Window {
    uint64_t* i;
    unsigned char* c;
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
}Query;

union Index {
    uint64_t i;
    unsigned char c[8];
}Index;

union multiChar {
    uint64_t i;
    unsigned char c[8];
}multiChar;

void print_bits(uint64_t num) {
    int i;
    char p[71];
    char* s;
    int check = num & 1;
    int count = 0;
    if( num & 1){
        p[0] = '1';
    }
    else{
        p[0] = '0';
    }
    for( i = 1; i < 71; i++){
        // printf("%i", p);

        if( count == 8 ) { //i == 8 || i == 17 || i == 25 || i == 33 || i == 41 || i == 49 || i == 57) {
            count = 0;
            p[i] = ' ';
            if(((num >>= 1) & 1)){
                //count++;
                p[i+1] = '1';
            }
            else{
                //count++;
                p[i+1] = '0';
            }
        }
        else if(((num >>= 1) & 1)){
            count++;
            p[i] = '1';
        }
        else{
            count++;
            p[i] = '0';
        }
    }
    s = &p[0];
    printf("%s\n", s);
}

int search_test (unsigned char* query_array,
                 int query_len,
                 unsigned char* st,
                 int text_len) {

    union Window t_w;
    t_w.c = &st[0];

    union Index i1;
    i1.i = 0UL;

    union multiChar m_c;

    int text_offset = query_len - 1;
    int mismatch_move_count = 0;
    int result = 0;

    unsigned char* char_ptr;
    unsigned char* string_check[2];

    uint64_t query_matches = 0UL;
    uint64_t value;
    uint64_t int_check[2];
    uint64_t shift_four;
    uint64_t initial_query_char_matches = 0;
    uint64_t initial_single_char_mask = query_array[0] * LAST_BITS_ON;
    uint64_t alignment_table[9] = {0x0UL,0x1UL,0x100UL,0x10000UL,0x1000000UL,0x100000000UL,0x10000000000UL,0x1000000000000UL,0x100000000000000UL};

    bool check = false;
    bool redo = false;

    char_ptr = &query_array[0];

    string_check[0] = &query_array[0];

    int_check[0] = 0UL;

    text_offset = 0;

    while(!(&*t_w.c > &st[text_len-1])) {

        i1.i = (i1.i & query_matches) + query_matches;

        m_c.c[0] = query_array[i1.c[0]];
        m_c.c[1] = query_array[i1.c[1]];
        m_c.c[2] = query_array[i1.c[2]];
        m_c.c[3] = query_array[i1.c[3]];
        m_c.c[4] = query_array[i1.c[4]];
        m_c.c[5] = query_array[i1.c[5]];
        m_c.c[6] = query_array[i1.c[6]];
        m_c.c[7] = query_array[i1.c[7]];

        uint64_t check = m_c.i;

        value = (~((*t_w.i) ^ m_c.i));

        shift_four = value & (value >> 4) & 0xF0F0F0F0F0F0F0FUL;
        query_matches = ((shift_four & (shift_four >> 2)) &
                         ((shift_four & (shift_four >> 2)) >> 1)) * (bool) hassetbyte(value);

        check = (bool) query_matches;

        if (&char_ptr[0] == &query_array[query_len - 1]) {
            if (check) {
                result += (uint8_t) (query_matches + query_matches / 255);
                query_matches = 0;
                check = false;
            }
        }

        string_check[1] = char_ptr + 1;
        char_ptr = string_check[check];

        text_offset += (8 * !(check)) + check;

        t_w.c = &st[text_offset];

        int_check[1] = query_matches;
        query_matches = int_check[check];
    }

    //We return False if the text is searched and nothing is found.
    return result;

}


