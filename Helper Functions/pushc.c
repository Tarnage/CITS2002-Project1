#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// TODO
// This function will push constant to the stack
// no need to convert decimal to hexa since its just pushing a constant to the stack

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
#define SIZE                20
AWORD                       main_memory[SIZE];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32


void pushc(int SP, int PC)
{   

    return 0;
}

int main(int argc, char *argv[])
{   
    int PC = 0;
    int SP = SIZE;

    // INIT SOME RANDOM VALUES
    main_memory[0] = 7;
    main_memory[1] = 3;
    main_memory[2] = 2;

    pushc(SP, PC);

    return 0;
}