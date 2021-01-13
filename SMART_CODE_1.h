//
// Created by michael on 9/12/20.
// Updated by micheal on 12/1/2020
// PackedFilter by Michael Olson is licensed under Attribution-NonCommercial-ShareAlike 4.0 International 

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
};

int search_test (unsigned char* query_array,
            int query_len,
            unsigned char* st,
            int text_len) {

    int text_offset = query_len - 1;
    int mismatch_move_count = 0;
    int result = 0;

    unsigned char* char_ptr;
    unsigned char* string_check[2];

    union Window t_w;
    t_w.c = &st[0];

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;
    uint64_t int_check[2];
    uint64_t shift_four;

    bool check = false;

    char_ptr = &query_array[0];

    text_offset = 0;

    string_check[0] = &query_array[0];

    int_check[0] = LAST_BITS_ON;
    
    //While there's text left
    while(!(&*t_w.c > &st[text_len-1])) {
        
        //Make a single character mask and compare with text window
        value = (~((*t_w.i) ^ query_matches * (*char_ptr)) & ~((*t_w.i) ^ LAST_BITS_ON * (*char_ptr)));
        
        //Recude the single character mask to a bool array 
        shift_four = value & (value >> 4) & 0xF0F0F0F0F0F0F0FUL;
        query_matches = ((shift_four & (shift_four >> 2)) &
                         ((shift_four & (shift_four >> 2)) >> 1)) * (bool) hassetbyte(value);
        
        //check IF query_matches is 0
        check = (bool) query_matches;
        
        //If we get to the last character in the query and there's a match
        if((&char_ptr[0] == &query_array[query_len-1]) && check){
            //result += __builtin_popcount(query_matches);
            //Count
            result += ((uint8_t) (query_matches + query_matches/255));
            //Reset everything
            text_offset -= mismatch_move_count;
            query_matches = 0;
            check = false;
            mismatch_move_count = 0;
        }
        
        //increase or reset char_ptr
        string_check[1] = char_ptr + 1;
        char_ptr = string_check[check];
        
        //Increment mismatch_move_count, increment text_offset, set t_w.c to the new location
        t_w.c = &st[(text_offset += ((8 - (mismatch_move_count += check)) * !(check)) + check)];
        
        //Reset mismatch_move_count if mismatch
        mismatch_move_count = mismatch_move_count * check;
        
        //Reset query_matches if mismatch
        int_check[1] = query_matches;
        query_matches = int_check[check];
    }
    return result;

}


