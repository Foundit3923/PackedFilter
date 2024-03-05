//
// Created by michael on 9/12/20.
//

//The core of this algorithm is the fact that binary inherently operates on instruction level parallelism.
//This parallelism is primarily visible when using specific sequences of numbers.
//Therefore this algorithm simulates mulitiple input multiple data through the use of single input multiple data. 

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
//#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7 //Original Method
#define reduce(v) (~(v + LAST_BITS_ON) / 128) & LAST_BITS_ON

//Counts the number of set bits in reduced integer
#define bitcount(q) (q + q/255) & 255

//Keeps track of found matches
#define count(v) result += bitcount(v)

//Compares a character (c) to every character in (t). Results in a fully set byte when there is a match. Fully set byte is at match location.
#define xnor(t,q,c) ~(t ^ q * (c)) //& ~(t ^ LAST_BITS_ON * (c))

//Unused
#define distance(x) ffsll(x >> 1) / 8

union Window {
    uint64_t* i;
    unsigned char* c[8];
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
};

union Pattern {
    unsigned char* c[8];
    //char l[8];
    uint64_t* i;
};

void printBits(uint64_t num){
  unsigned int size = sizeof(unsigned int);
  unsigned int maxPow = 1<<(size*8-1);
  int i = 0;
  for(;i<size*8;++i){
    // print last bit and shift left.
    printf("%u ",num&maxPow ? 1 : 0);
    num = num<<1;
  }
}

Pattern setSearchPattern(Pattern searchPattern, Window text_window, int[] window_locations){
    for(int i=0; i < 8; i++){
      searchPattern.c[n] = text_window.c[n] += window_locations[n];
    }
    return searchPattern;
}

int search_test (unsigned char* query_array,
            int query_len,
            char* text,
            int text_len) {

    //Setup
    int result = 0;
    int text_offset = 0;
    int window_offset = text_len/8;

    unsigned char* char_ptr = &query_array[0];
    unsigned char* last_char = &query_array[query_len-1];
    unsigned char* first_char = &query_array[0];

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;

    //searchPattern is an array of chars that correspond to what each window in TW is looking for
    union Pattern searchPattern;

    //text_window.c should represent a single character from 8 equal pieces of the text all in sequence.
    //|hel|l t|his| is| my| na|me |jim.
    //text_window.c = hlh   mj
    //text_window.c = &text[start];
    //text_window.c = &text[start+=offset]
    //repeat 
    //need array for storing locations in text
    union Window text_window;
    int window_offset_list[8];
    int window_locations[8];
    for(int i=0; i < 8; i++){
      window_locations[i] = (int)(window_offset * i);
      text_window.c[i] = &text[window_locations[i]]; //&text[window_locations[i]];
      searchPattern.c[i] = &query_array[0];
    }

    //experiments with incrementing
    //text_window.c[i]++ increments the pointer to the next position
    //*text_window.c[i]++ increments the value that is pointed to

    //what do I have available to use?
    //uint64_t query_matches - holds a value of one at positions where the searchPattern matches the text_window
    //

    //offset should be set so that when an 8 character sequence is grabbed the character we want is in the correct position.
    //so for text_window[3] where we want to align this position with the 4th character the offset would be 0. If we wanted to align with the 10th character the offset would be 6.
    //This allows us to grab sections of text without worrying about alignment

    //The idea here is to use masks to 1)locate the position in the text_window 2)'clear' the target byte 3)copy the desired byte 4)load the desired byte into the text_window
    //5)signal the current state of each byte to the manager 6)increment the offset

    unsigned int* p = &searchPattern.c[0];
    searchPattern.c[0]++;
    *searchPattern.c + LAST_BITS_ON;
    *searchPattern.c;
    searchPattern.i += LAST_BITS_ON;

    //query_window is a window into the query of the first 8 chars
    union Query query_window;
    query_window.c = &query_array[0];

    printf("text_window.c: %s\n", text_window.c);
    printf("text_window.i: ");
    printBits(text_window.i);
    printf("\n*text_window.i: ");
    printBits(*text_window.i);

    //While the address of the first char of text_window.c is not the address of the last char of the text. 
    while(!(&*text_window.c[7] > &text[text_len-1])) {
      searchPattern = setSearchPattern(searchPattern, text_window, window_locations);
      //Check if *char_ptr matches any chars in *text_window
      //Need to check if *text_window.i and *searchPattern.i dereference properly
      value = xnor(*text_window.i, query_matches, *searchPattern.i);
      
      //reduce any found matches to a single bit occupying the lsb position of a byte
      //compare query_matches with reduce to only keep desired matches
      //(bool) catches any set bits which indicate a match
      if((bool)(reduce(value) & query_matches)) {
          
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
          //No match found in window: move window and reset
          char_ptr = first_char;
          text_offset += 8;
          //text_window.c = &text[text_offset];
          //this hopefully moves each array forward by one.
          //text_window.c is a sequence of mem addresses and we want to increment each of those forward by 1
          text_window.c + LAST_BITS_ON;
          query_matches = LAST_BITS_ON;
      }
    }   //Return False if the text is searched and nothing is found.
    return result;

}


