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

//Reduces fully set bytes to a byte with a 1 in the LSB position, non fully set bytes are reudced to 0
#define reduce(v) (~(v + LAST_BITS_ON) / 128) & LAST_BITS_ON

//Counts the number of set bits in reduced integer
#define bitcount(q) (q + q/255) & 255

//Keeps track of found matches
#define count(v) result += bitcount(v)

//Compares a character (c) to every character in (t). Results in a fully set byte when there is a match. Fully set byte is at match location.
#define xnor(t,c) ~(t ^ c) 

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

union Pattern {
    unsigned char* c[8];
    //char l[8];
    uint64_t* i;
};

Pattern setSearchPattern(Pattern searchPattern, unsigned char* query_array, int[] pattern_locations){
    for(int i=0; i < 8; i++){
      searchPattern.c[i] = query_array[pattern_locations[i]];
    }
    return searchPattern;
}

Window setMultiTextWindow(Window text_window, unsigned char* text, int[] window_locations){
    for(int i=0; i < 8; i++){
      text_window.c[i] = text[window_locations[i]];
    }
    return text_window;
}

int search_test (unsigned char* query_array,
            int query_len,
            unsigned char* text,
            int text_len) {

    //Setup
    int result = 0;
    int text_offset = 0;

    unsigned char* char_ptr = &query_array[0];
    unsigned char* last_char = &query_array[query_len-1];
    unsigned char* first_char = &query_array[0];
    
    union Window text_window;
    text_window.c = &text[0];

    union Pattern searchPattern;

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;

    int search_windows[8];
    int eval_locations[10];
    int match_locations[10];
    
    bool evalFlag = false;
    bool compareFlag = false;
    char* modeFlag = "search";
    
    //While the address of the first char of text_window.c is not the address of the last char of the text. 
    while(!(&*text_window.c > &text[text_len-1])) {
        searchPattern = setSearchPattern(searchPattern, query_array, pattern_locations);
        text_window = setMultiTextWindow(text_window, text, window_locations);
        //Check if *char_ptr matches any chars in *text_window  
        value = xnor(*text_window.i, query_matches, *searchPattern.i);
        
        //reduce any found matches to a single bit occupying the lsb position of a byte
        //compare query_matches with reduce to only keep desired matches
        //(bool) catches any set bits which indicate a match
        if((bool)(query_matches = reduce(value) & query_matches)) {
            
            //On match found, evaluate Mode for instructions
            switch(modeFlag){
              case "search":
                modeFlag = "eval";
                //increment everything first 

                //set the window that matched to evaluate the last char in the potential match on the next cycle and save the last window and pattern position in pAlignPos.

              break;

              case "eval":
                modeFlag = "compare";
              break;

              case "compare":
              break;

              default:
              break;
            }

            //If the query has finished iterating: query match is found, record and reset
            if (&char_ptr[0] == last_char) {
                count(query_matches);
                char_ptr = first_char;
            } else {
                //Character match found: move to next char in text and query
                char_ptr++;
                text_window.c++;
            }
        } else {
            modeFlag = "search";
            //Search State
            //No match found in window: move window and reset
            text_window.c++;
            query_matches = LAST_BITS_ON;
        }
    }
    //Return False if the text is searched and nothing is found.
    return result;

}


