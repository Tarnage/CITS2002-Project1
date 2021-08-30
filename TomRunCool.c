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

// THE CACHE
AWORD                       cache[N_CACHE_WORDS];

// SINGLE BYTE FOR CHECKING IF CACHE IS DIRTY OR CLEAN
// -1 = NOT USED, 0 = CLEAN, 1 = DIRTY
int8_t                     is_clean[N_CACHE_WORDS];

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
int n_instructions          = 0;
int n_func_calls            = 0;
int m_func_call_depth       = 0;
int m_stack_depth           = 0;

void report_statistics(void)
{
    printf("@number-of-main-memory-reads\t%i\n",    n_main_memory_reads);
    printf("@number-of-main-memory-writes\t%i\n",   n_main_memory_writes);
    printf("@number-of-cache-memory-hits\t%i\n",    n_cache_memory_hits);
    printf("@number-of-cache-memory-misses\t%i\n",  n_cache_memory_misses);
    printf("@\n");
    // FOLLOWIG LINES NOT NEEDED FOR FINAL PROJECT
    printf("@number-of-instructions \t%i\n",         n_instructions);        
    printf("@number-of-function-calls \t%i\n",       n_func_calls);      
    printf("@maximum-function-call-depth\t%i\n",     m_func_call_depth);   
    printf("@maximum-stack-depth \t\t%i\n",          m_stack_depth);
    printf("@\n");           
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

// TODO Implement write back cache
// STILL NEED TO WORK OUT THE CACHE HIT/MISS METRICS
// implement dirty clean tags

AWORD read_memory(int address)
{   
// The main memory address is cacheAddress + N_MAIN_MEMORY_WORDS + 1
//    AWORD cacheAddress = (N_MAIN_MEMORY_WORDS - address) - 1;

// currently counting all reads including cache reads
    ++n_main_memory_reads;
/*
    if( cacheAddress <= N_CACHE_WORDS ){
        return cache[cacheAddress];
    }
    else {
        return main_memory[address];
    }
*/
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{   
// The main memory address is cacheAddress + N_MAIN_MEMORY_WORDS + 1
//    AWORD cacheAddress = (N_MAIN_MEMORY_WORDS - address) - 1;
//    AWORD mainAddress = cacheAddress + N_MAIN_MEMORY_WORDS + 1;
// TODO how will main memory get the data from cache
// check for dirty bits and clean bits
    ++n_main_memory_writes;
/*
    if( cacheAddress <= N_CACHE_WORDS ){
        // if cache is dirty write contents to main memory

        //++n_cache_memory_hits;
        // data will be stored in cache
        cache[cacheAddress] = value;
        // the address of the data will be stored in main memory
        //main_memory[address] = cacheAddress;
        main_memory[address] = value;
    }
    else{
        //not sure if we need cache misses in write memory
        //++n_cache_memory_misses;
        
        main_memory[address] = value;
    }
*/
    main_memory[address] = value;
}

//  -------------------------------------------------------------------

// HELPER FUNCTIONS GO HERE
// TESTING PURPOSES a global variable used to help print files
AWORD size;

void printArray(int PC, int length)
{   
    printf("Current Memory PC:\n");
    for (int i = PC - 1; i < length; ++i){
        printf("%i ", main_memory[i]);
    }
    printf("\n");
}

void printStack(int SP)
{   
    if(SP == N_MAIN_MEMORY_WORDS){
        printf("Stack is empty.");
    }
    else{
        printf("Current Stack:\n");
        for (int i = SP; i < N_MAIN_MEMORY_WORDS; ++i){
            printf("%i ", main_memory[i]);
        }
    }
    printf("\n");
}

void printFrame(int FP)
{   
    if(FP == 0){
        printf("Frame is empty.");
    }
    else{
        printf("Current Frame:\n");
        for (int i = FP - 3; i != FP + 4; ++i){
            printf("%u ", main_memory[i]);
        }
    }
    printf("\n");
}

void printCache()
{   
    printf("Current Cache :\n");
    for (int i = 0; i < N_CACHE_WORDS; ++i){
        printf("%i ", cache[i]);
    }
    printf("\n");
}

void printis_clean()
{   
    printf("Current is_clean :\n");
    for (int i = 0; i < N_CACHE_WORDS; ++i){
        printf("%i ", is_clean[i]);
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
    
    while(true) {

//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED
        IWORD instruction   = read_memory(PC);
//        printf("PC Value: %i\n", PC);
        ++PC;
        ++n_instructions;

//  PRINT THE INSTRUCTIONS FOUND IN main_memory[]
//        printArray(PC, size);

// PRINTS REGISTERS
//        printf("SP Value: %i\n", SP);
        printStack(SP);
//        printf("FP Value: %i\n", FP);
//        printFrame(FP);
//        printCache();
//        printis_clean();
        printf("current stack depth %i\n", m_stack_depth);

        if(instruction == I_HALT){
            printf("Entered HALT\n");
            break;
        }

        switch(instruction)
        {
            case I_NOP:
            // NO OPERATION, CONTINUES TO NEXT INSTRUCTION
                break;

            case I_ADD:
                ++SP;
                write_memory( SP, read_memory(SP - 1) + read_memory(SP) );
                break;

            case I_SUB:
                ++SP;
                write_memory( SP, read_memory(SP) - read_memory(SP - 1) );
                break;
            
            case I_MULT:
                ++SP;
                write_memory( SP, read_memory(SP - 1) * read_memory(SP) );
                break;

            case I_DIV:
                ++SP;
                write_memory( SP, read_memory(SP) / read_memory(SP - 1) );
                break;

            case I_CALL:
            //TODO CHECK
                ++n_func_calls;
                ++m_func_call_depth;

            // push return address to TOS
                ++m_stack_depth;
                --SP;
                write_memory(SP, PC + 1);

            // push FP address to TOS
                ++m_stack_depth;
                --SP;
                write_memory(SP, FP);

            // set FP 
                FP = SP;

            // start execution of next function
                PC = read_memory(PC);
                break;

            case I_RETURN:
            //TODO CHECK
            // currently holds the off set
                instruction = read_memory(PC);

            // PC goes back to the following instruction that called current function
                PC = read_memory(FP + 1);
            
            // calculated return value placed in the FP offset
                write_memory( FP + instruction, read_memory(SP) );

            // SP reset to actual TOS
                SP = FP + instruction;

            //FP reset
                FP = read_memory(FP);
                break;

            case I_JMP:
            // TODO CHECK CORRECTNESS
            //    printf("Entered JMP\n");
                PC = read_memory(PC);
                break;

            case I_JEQ:
            // TODO CHECK CORRECTNESS
            //    printf("Entered JEQ\n");
                --m_stack_depth;
                if( read_memory(SP) == 0 ) PC = read_memory(PC);
                else ++PC;
                ++SP;
                break;

            case I_PRINTI:
            // TODO IMPLEMENT
            //    printf("Entered PRINTI\n");
                break;

            case I_PRINTS:
            // TODO IMPLEMENT
            //    printf("Entered PRINTI\n");
                break;
            
            case I_PUSHC:
                ++m_stack_depth;
                --SP;
                write_memory( SP, read_memory(PC) );
                ++PC;
                break;

            case I_PUSHA:
            // TODO CHECK
                ++m_stack_depth;
                --SP;
                write_memory( SP, read_memory( read_memory(PC) ) );
                ++PC;
                break;

            case I_PUSHR:
            // TODO CHECK
                ++m_stack_depth;
                --SP;
                write_memory( SP, read_memory( FP + read_memory(PC) ) );
                ++PC;
                break;

            case I_POPA:
            // TODO CHECK
                write_memory( read_memory(PC), read_memory(SP) );
                ++SP;
                ++PC;
                break;
            
            case I_POPR:
            // TODO CHECK
                write_memory( FP + read_memory(PC), read_memory(SP) );
                ++SP;
                ++PC;
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
    memset(cache, 0, sizeof cache);               //  clear cache
    memset(is_clean, -1, sizeof is_clean);        //  clear is_clean 

// read in buffer
    AWORD buffer[N_MAIN_MEMORY_WORDS];
    

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

// reset counter for wrtining to memory
    n_cache_memory_misses = 0;
    n_main_memory_writes = 0;
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
    read_coolexe_file("Coolexe/adivb.coolexe");

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();

    printf("my calculated result:\t\t%i\n", result);

    return result;          // or  exit(result);
}
