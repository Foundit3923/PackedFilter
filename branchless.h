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
#define LAST_BITS_NOT_ON 0xFEFEFEFEFEFEFEFEUL
#define SIGNIFICANT_BITS_ON 0x8080808080808080UL
#define SIGNIFICANT_BITS_NOT_ON 0x7F7F7F7F7F7F7F7FUL

//Compares a character (c) to every character in (t). Results in a fully set byte when there is a match. Fully set byte is at match location.
#define xnor(t,q,c) ~(t ^ q * (c)) & ~(t ^ LAST_BITS_ON * (c))

//Reduces fully set bytes to a byte with a 1 in the LSB position, non fully set bytes are reudced to 0
#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7 //Original Method
//#define reduce(t,q,c) (~(xnor(t,q,c) + LAST_BITS_ON) / 128) & LAST_BITS_ON

//Counts the number of set bits in reduced integer
#define bitcount(q) (q + q/255) & 255

//Keeps track of found matches
#define count(v) result += bitcount(v)

//Compares and reduces the comparison of a character (c) to every character in (t). Results in a fully set byte reduced to a byte with the LSB position on. Non-fully set bytes are 0
#define reduce_xnor(t,q,c) ((LAST_BITS_NOT_ON - ~(t ^ q * (c))) >> 7) & LAST_BITS_ON

//Unused
#define distance(x) ffsll(x >> 1) / 8

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
            unsigned char* text,
            int text_len) {

    //Setup
    int result = 0;
    int text_offset = 0;
    int choice[2] = {8,0};

    unsigned char* ptr_arry[2] = {&query_array[0], &query_array[0]};
    unsigned char* char_ptr = &query_array[0];
    unsigned char* last_char = &query_array[query_len-1];
    unsigned char* first_char = &query_array[0];
    
    union Window text_window;
    text_window.c = &text[0];

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;
    uint64_t tmp1;
    uint64_t tmp2;
    uint64_t tmp3;
    uint64_t tmp4;
    uint64_t tmp5;
    uint64_t tmp6;
    
    //While the address of the first char of text_window.c is not the address of the last char of the text. 
    while(!(&*text_window.c > &text[text_len-1])) {
        //Check if *char_ptr matches any chars in *text_window  
        //value = xnor(*text_window.i, query_matches, *char_ptr);
        //value = ~(*text_window.i ^ query_matches * (*char_ptr));            //9 instructions
        //value = *text_window.i ^ query_matches * (*char_ptr);               //8 instructions
        //value = ~(*text_window.i ^ query_matches * (*char_ptr)) & ~(*text_window.i ^ LAST_BITS_ON * (*char_ptr));
        //tmp1 = (~((~(*text_window.i ^ query_matches * (*char_ptr))) + LAST_BITS_ON) / 128) & LAST_BITS_ON;
        //reduce any found matches to a single bit occupying the lsb position of a byte
        //compare query_matches with reduce to only keep desired matches
        //(bool) catches any set bits which indicate a match
        
        //tmp1 = (~(value + LAST_BITS_ON) / 128) & LAST_BITS_ON;              //7 instructions
        //tmp1 = ((0xFEFEFEFEFEFEFEFEUL - ~value) / 128) & 0x101010101010101UL;//6 instructions
        //tmp1 = (~(value + LAST_BITS_ON) >> 7) & LAST_BITS_ON;               //7 instructions
        //tmp1 = (~((value<<1)& SIGNIFICANT_BITS_ON) / 128) & LAST_BITS_ON;   //10 instructions
        
        //tmp1 = (((~value - LAST_BITS_ON) ^ ~value) & SIGNIFICANT_BITS_ON) >> 7;

        //value = ((0xFEFEFEFEFEFEFEFEUL - (*text_window.i ^ query_matches * (*char_ptr))) >> 7) & 0x101010101010101UL//;

        ////value = reduce_xnor(*text_window.i, query_matches, *char_ptr)//;

        //value = reduce(*text_window.i, query_matches, *char_ptr);
        //value = reduce_xnor(*text_window.i, query_matches, *char_ptr);  
        //tmp1 = xnor(*text_window.i, query_matches, *char_ptr);      
        //value = reduce(tmp1);
        bool big_check = (bool)(query_matches = reduce_xnor(*text_window.i, query_matches, *ptr_arry[0]) & query_matches);
        int match_check = (int)big_check; //1 if true
        //If big_check == 1 then this section
        //If the query has finished iterating: query match is found, record and reset
        bool check = (bool)(&char_ptr[0] == last_char); 
        int fin_check = (int)check; // 1 if true
        count(query_matches);
        //1->0 0->1
        //
        char_ptr = ptr_arry[match_check * (1*fin_check)]; //(&query_array[0] | (1*fin_check)) | (char_ptr | -(fin_check-1));

        //If the query is not finished iterating: query match, move forward
        //this section needs both fin_check and match_check combined
        //To succeed: match_check== 1, fin_check==0
        //match_check *-(fin_check-1)
        //1 * -(1-1) = 0
        //1 * -(0-1) = 1
        //0 * -(1-1) = 0
        //0 * -(0-1) = 0
        ptr_arry[0] += match_check * -(fin_check-1);
        text_window.c += match_check * -(fin_check-1);

        //If big_check == 0 then this section
        //If the query was not found, reset
        ptr_arry[0] = &query_array[match_check];
        text_offset += choice[match_check];
        text_window.c = &text[text_offset];
        //0-1 = -1 -(-1) = 1. 1-1 = 0 -0 = 0. So 0->1, 1->0
        //Where there is no match LAST_BITS_ON will & with 0 and when there is a match query_matches will & with zero
        query_matches = (LAST_BITS_ON * -(match_check-1)) & (query_matches * (1*match_check));     
    }
    //Return False if the text is searched and nothing is found.
    return result;

}


