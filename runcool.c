//  CITS2002 Project 1 2021
//  Name(s):             student-name1   (, student-name2)
//  Student number(s):   student-number1 (, student-number2)

//  compile with:  cc -std=c11 -Wall -Werror -o runcool runcool.c

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THE STACK-BASED MACHINE HAS 2^16 (= 65,536) WORDS OF MAIN MEMORY
#define N_MAIN_MEMORY_WORDS (1<<16)

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
AWORD                       main_memory[N_MAIN_MEMORY_WORDS];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32


//  see:  https://teaching.csse.uwa.edu.au/units/CITS2002/projects/coolinstructions.php
enum INSTRUCTION {
    I_HALT       = 0,
    I_NOP,
    I_ADD,
    I_SUB,
    I_MULT,
    I_DIV,
    I_CALL,
    I_RETURN,
    I_JMP,
    I_JEQ,
    I_PRINTI,
    I_PRINTS,
    I_PUSHC,
    I_PUSHA,
    I_PUSHR,
    I_POPA,
    I_POPR
};

//  USE VALUES OF enum INSTRUCTION TO INDEX THE INSTRUCTION_name[] ARRAY
const char *INSTRUCTION_name[] = {
    "halt",
    "nop",
    "add",
    "sub",
    "mult",
    "div",
    "call",
    "return",
    "jmp",
    "jeq",
    "printi",
    "prints",
    "pushc",
    "pusha",
    "pushr",
    "popa",
    "popr"
};

//  ----  IT IS SAFE TO MODIFY ANYTHING BELOW THIS LINE  --------------


//  THE STATISTICS TO BE ACCUMULATED AND REPORTED
int n_main_memory_reads     = 0;
int n_main_memory_writes    = 0;
int n_cache_memory_hits     = 0;
int n_cache_memory_misses   = 0;

void report_statistics(void)
{
    printf("@number-of-main-memory-reads\t%i\n",    n_main_memory_reads);
    printf("@number-of-main-memory-writes\t%i\n",   n_main_memory_writes);
    printf("@number-of-cache-memory-hits\t%i\n",    n_cache_memory_hits);
    printf("@number-of-cache-memory-misses\t%i\n",  n_cache_memory_misses);
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

AWORD read_memory(int address)
{
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{
    main_memory[address] = value;
}

//  -------------------------------------------------------------------

// HELPER FUNCTIONS GO HERE

void printArray(AWORD arr[], int length)
{   
    printf("Current Memory PC:\n");
    for (int i = 0; i < length; ++i){
        printf("%i ", arr[i]);
    }
    printf("\n");
}

void printStack(int SP)
{
    printf("Current Stack:\n");
    for (int i = SP; i < N_MAIN_MEMORY_WORDS; ++i){
        printf("%i ", main_memory[i]);
    }
    printf("\n");
}
//  -------------------------------------------------------------------

//  EXECUTE THE INSTRUCTIONS IN main_memory[]
int execute_stackmachine(void)
{
//  THE 3 ON-CPU CONTROL REGISTERS:
    int PC      = 0;                    // 1st instruction is at address=0
    int SP      = N_MAIN_MEMORY_WORDS;  // initialised to top-of-stack
    int FP      = 0;                    // frame pointer

//  REMOVE THE FOLLOWING LINE ONCE YOU ACTUALLY NEED TO USE FP
    FP = FP + 0;

    
    while(true) {

//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED
        IWORD instruction   = read_memory(PC);
        ++PC;

        if(instruction == I_HALT){
            printf("Entered HALT\n");
            break;
        }

        switch(instruction)
        {
            case I_NOP:
                printf("Entered NOP\n");
                break;

            case I_ADD:
                printf("Entered ADD\n");
                break;

            case I_SUB:
                printf("Entered SUB\n");
                break;
            
            case I_MULT:
                printf("Entered MULT\n");
                break;

            case I_DIV:
                printf("Entered DIV\n");
                break;

            case I_CALL:
                printf("Entered CALL\n");
                break;

            case I_RETURN:
                printf("Entered RETURN\n");
                break;

            case I_JMP:
                printf("Entered JMP\n");
                break;

            case I_JEQ:
                printf("Entered JEQ\n");
                break;

            case I_PRINTI:
                printf("Entered PRINTI\n");
                break;

            case I_PRINTS:
                printf("Entered PRINTI\n");
                break;
            
            case I_PUSHC:
                printf("Entered PUSHC\n");
                break;

            case I_PUSHA:
                printf("Entered PUSHA\n");
                break;

            case I_PUSHR:
                printf("Entered PUSHR\n");
                break;

            case I_POPA:
                printf("Entered POPA\n");
                break;
            
            case I_POPR:
                printf("Entered POPR\n");
                break;
        }
    }

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
    return read_memory(SP);
}

//  -------------------------------------------------------------------
//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
    memset(main_memory, 0, sizeof main_memory);   //  clear all memory
    AWORD buffer[N_MAIN_MEMORY_WORDS];
    AWORD size;

//  READ CONTENTS OF coolexe FILE
    FILE    *fp_in = fopen(filename, "rb");

    // Checks file was opened successfully
    if(fp_in == NULL) {
        printf( "cannot open dictionary '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    // Finds the size or count of instructions in the exe file
    fseek(fp_in, 0, SEEK_END);

    // ftell returns single bytes
    // size is actually half since each word is 2 bytes 
    size = ftell(fp_in) / 2;

    // rewind file pointer back to the beginning of file
    rewind(fp_in);

    // read in exe file
    fread(buffer, sizeof(buffer), 1, fp_in);
    // don't forget to close the file
    fclose(fp_in);

    // write the contents to memory
    for(AWORD i = 0; i < size; ++i) {
        write_memory(i, buffer[i]);
    }
}

//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  CHECK THE NUMBER OF ARGUMENTS

//    if(argc != 2) {
//        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
//        exit(EXIT_FAILURE);
//    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
//    read_coolexe_file(argv[1]);
// ADDED FOR TESTING MAKE SURE WE UNDO THE COMMENTS BEFORE SUBMIT
    read_coolexe_file("parameters.coolexe");
//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();

//  PRINT THE INSTRUCTIONS FOUND IN main_memory[]
    printArray(main_memory, 25);

    return result;          // or  exit(result);
}
