#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

//  TODO
// push absolute
// the word immediately following the 'pusha' instruction holds the address of the integer value to be pushed onto the stack

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
#define SIZE                20
AWORD                       main_memory[SIZE];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32

AWORD read_memory(int address)
{
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{   
    main_memory[address] = value;
}

void pusha(uint16_t decimal)
{
    return;
}

void printArray(AWORD arr[], int length)
{   
    printf("Main memory:\n");
    for (int i = 0; i < length; ++i){
        printf("%u ", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{   
    int PC = 0;
    //int SP = SIZE;

    // INIT SOME RANDOM VALUES TO MAIN MEMORY
    write_memory(PC, 7);
    ++PC;
    write_memory(PC, 3);
    ++PC;
    write_memory(PC, 2);
    ++PC;                                         // This should return 2, 512 is the decimal value of hexa "0200"
    write_memory(PC, 512);                        // This is the address to the int value to be pushed onto stack 

    char hex[] = "0020";

    uint16_t result = strtol(hex, NULL, 16);

    //sscanf(hex, "0x%x", result);
    
    printf("hex: %s = uint16_t: %i\n", hex, result);
    return 0;
}