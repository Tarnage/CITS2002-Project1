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
    int SP = SIZE;

    // INIT SOME RANDOM VALUES TO MAIN MEMORY
    write_memory(PC, 7);
    ++PC;
    write_memory(PC, 3);
    ++PC;
    write_memory(PC, 2);

    return 0;
}