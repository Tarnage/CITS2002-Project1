#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// TODO
// This function will push constant to the stack
//push integer constant
//the integer constant in the word immediately following the 'pushc' instruction is pushed onto the stack

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

// THIS FUNCTION SHOULD PUSH CONSTANTS FROM THE PROGRAM COUNTER TO THE STACK
void pushc(int SP, int PC)
{   
    AWORD temp;
    temp = read_memory(PC);
    write_memory(SP, temp);
    return;
}

void printMainMemory(AWORD arr[], int length)
{   
    printf("Main memory:\n");
    for (int i = 0; i < length; ++i){
        printf("%u ", arr[i]);
    }
    printf("\n");
}

void printStack(AWORD arr[], int SP)
{   
    printf("Stack:\n");
    for (int i = SP; i < SIZE; ++i){
        printf("%u ", arr[i]);
    }
    printf("\n");
}
int main(int argc, char *argv[])
{   
    int PC = -1;
    int SP = SIZE -1;

    // INIT SOME RANDOM VALUES TO MAIN MEMORY
    ++PC;
    write_memory(PC, 7);

    ++PC;
    write_memory(PC, 3);

    ++PC;
    write_memory(PC, 2);

    --SP;
    pushc(SP, PC);
    --PC;

    --SP;
    pushc(SP, PC);
    --PC;

    --SP;
    pushc(SP, PC);
    --PC;

    printMainMemory(main_memory, 3);
    printStack(main_memory, SP);
    printf("PC: %i\n", PC);
    printf("SP: %i\n", SP);

    return 0;
}