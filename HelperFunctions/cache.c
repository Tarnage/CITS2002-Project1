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
#define SIZE                20
AWORD                       main_memory[SIZE];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32

typedef struct
{
    uint8_t     tag;
    uint8_t     valid;
    uint8_t     dirtyBit;
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

void printArray(CACHE arr[])
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
    memset(cache, 0, sizeof cache);
    CACHE first;
    first.valid = 1;
    first.tag = 00101;
    first.dirtyBit = 0;
    first.data = 32;

    printf("The cache structure first contains: \n");
    printf("valid bit: %i\n", first.valid);
    printf("tag bit: %i\n", first.tag);
    printf("dirtyBit bit: %i\n", first.dirtyBit);
    printf("data bit: %i\n", first.data);

    printArray(cache);
    return 0;
}