#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
#define SIZE                64
AWORD                       main_memory[SIZE];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32

typedef struct
{   
    //Check if current line is occupied
    uint8_t     valid;
    //Check if line is dirty or clean 
    //if clean it does not need to be written to memeory
    //if dirty we need to write to memory
    uint8_t     dirtyBit;
    // actual data
    AWORD       data;
} CACHE;

CACHE                cache[N_CACHE_WORDS];

AWORD read_memory(int address)
{
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{   
    main_memory[address] = value;
}

void function_to_implement(uint16_t decimal)
{
    return;
}

void printArray(uint8_t arr[])
{   
    printf("Cache:\n");
    for (int i = 0; i < 32; ++i){
        printf("%u ", arr[i]);
    }
    printf("\n");
}

void print_binary(unsigned int number)
{
    if (number >> 1) {
        print_binary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

int main(int argc, char *argv[])
{   

    int SP = SIZE;
    int PC = 0;

    memset(cache, 0, sizeof cache);
    memset(cache, 0, sizeof cache);

    AWORD instructions[] = {12, 0, 6, 5, 0, 12, 4, 12, 2, 5, 7, 1, 7, 1};

    for(int i = 0; i < 14; ++i){
        main_memory[i] = instructions[i];
    }

    for(int i = 0; i < 32; ++i){
        CACHE first;
        first.valid = 0;
        cache[i] = first;
    }

    

    /*
    printf("The cache structure first contains: \n");
    printf("valid bit: %i\n", first.valid);
    printf("tag bit: %i\n", first.tag);
    printf("dirtyBit bit: %i\n", first.dirtyBit);
    printf("data bit: %i\n", first.data);
    */
    printf("data: %i\n",cache[0].data);



    return 0;
}