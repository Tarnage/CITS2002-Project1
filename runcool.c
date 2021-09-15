//  CITS2002 Project 1 2021
//  Name(s):             Tom Nguyen     , Yu Li
//  Student number(s):   22914578       , 21241907

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
struct
{   
    int8_t        dirtyBit;
    int8_t        valid;
    AWORD         tag;
    AWORD         data;

} cache[N_CACHE_WORDS];


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
    printf("@\n");
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

AWORD read_memory(int address)
{   
    ++n_main_memory_reads;
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{   
    ++n_main_memory_writes;
    main_memory[address] = value;
}

AWORD read_cache_memory(int address, int offset)
{   
    int cacheAddress = (address  + offset) % N_CACHE_WORDS;
    if(cache[cacheAddress].tag != address + offset || cache[cacheAddress].valid == 0){
        ++n_cache_memory_misses;

        if(cache[cacheAddress].dirtyBit == 0 && 
                cache[cacheAddress].valid == 1 && 
                    cache[cacheAddress].tag != address + offset){
            write_memory(cache[cacheAddress].tag, cache[cacheAddress].data);
        }
        cache[cacheAddress].data = read_memory(address + offset);
        cache[cacheAddress].tag = address + offset;
        cache[cacheAddress].dirtyBit = 1;
        cache[cacheAddress].valid = 1;

        return cache[cacheAddress].data;
    }
    else{
        ++n_cache_memory_hits;
        return cache[cacheAddress].data;
    }
}

void write_cache_memory(AWORD address, AWORD value, int offset)
{     
    int cacheAddress = (address  + offset) % N_CACHE_WORDS;

    if(cache[cacheAddress].dirtyBit == 0 && 
            cache[cacheAddress].valid == 1 && 
                cache[cacheAddress].tag != address + offset){
        write_memory(cache[cacheAddress].tag, cache[cacheAddress].data);
    }

    cache[cacheAddress].data         = value;
    cache[cacheAddress].tag          = address + offset;
    cache[cacheAddress].dirtyBit     = 0;
    cache[cacheAddress].valid        = 1;
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
        IWORD instruction   = read_cache_memory(PC, 0);
        ++PC;

        if(instruction == I_HALT){
            break;
        }

        switch(instruction)
        {
            case I_NOP:
            // NO OPERATION, CONTINUES TO NEXT INSTRUCTION
                break;

            case I_ADD:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, -1) + read_cache_memory(SP, 0), 0 );
                break;

            case I_SUB:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, 0) - read_cache_memory(SP, -1), 0 );
                break;
            
            case I_MULT:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, -1) * read_cache_memory(SP, 0), 0 );
                break;

            case I_DIV:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, 0) / read_cache_memory(SP, -1), 0 );
                break;

            case I_CALL:

            // push return address to TOS
                --SP;
                write_cache_memory(SP, PC + 1, 0);

            // push FP address to TOS
                --SP;
                write_cache_memory(SP, FP, 0);

            // set FP 
                FP = SP;

            // start execution of next function
                PC = read_cache_memory(PC, 0);
                break;

            case I_RETURN:
            // currently holds the off set
                instruction = read_cache_memory(PC, 0);

            // PC goes back to the following instruction that called current function
                PC = read_cache_memory(FP, 1);
            
            // calculated return value placed in the FP offset
                write_cache_memory( FP, read_cache_memory(SP, 0), instruction );

            // SP reset to actual TOS
                SP = FP + instruction;
            //FP reset
                FP = read_cache_memory(FP, 0);
                break;

            case I_JMP:
            //    printf("Entered JMP\n");
                PC = read_cache_memory(PC, 0);
                break;

            case I_JEQ:
                if( read_cache_memory(SP, 0) == 0 ) PC = read_cache_memory(PC, 0);
                else ++PC;
                ++SP;
                break;

            case I_PRINTI:
            // insruction holds TOS
                instruction = read_cache_memory(SP, 0);
                ++SP;
                printf("%i", instruction);
                break;

            case I_PRINTS:
            // instruction holds the address of the next instruction when print is finished
                instruction = PC + 1;
                PC = read_cache_memory(PC, 0);

                while(true){
                    //read value from PC 
                    AWORD val = read_cache_memory(PC, 0);
                    ++PC;
                    //Each 16-bits integer contain two char
                    //first 8-bits is a
                    //second 8-bit is b
                    char a = val % 256;
                    char b = val / 256;

                    if(a != '\0') printf("%c", a);
                    else {
                        PC = instruction;
                        break;
                    }
                    
                    if(b != '\0') printf("%c", b);
                    else {
                        PC = instruction;
                        break;
                    }
                }
                
                break;
            
            case I_PUSHC:
                --SP;
                write_cache_memory( SP, read_cache_memory(PC, 0), 0 );
                ++PC;
                break;

            case I_PUSHA:
                // instruction holds the address of value to push
                instruction = read_cache_memory(PC, 0);
                --SP;
                write_cache_memory( SP, read_cache_memory(instruction, 0), 0 );
                ++PC;
                break;

            case I_PUSHR:
            // instruction holds the offset
                instruction = read_cache_memory(PC, 0);
                --SP;
                write_cache_memory( SP, read_cache_memory(FP, instruction), 0 );
                ++PC;
                break;

            case I_POPA:
                write_cache_memory( read_cache_memory(PC, 0), read_cache_memory(SP, 0), 0 );
                ++SP;
                ++PC;
                break;
            
            case I_POPR:
            // instruction holds the offset
                instruction = read_cache_memory(PC, 0);
                write_cache_memory( FP, read_cache_memory(SP, 0), instruction );
                ++SP;
                ++PC;
                break;
        }
    }

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
    return read_cache_memory(SP, 0);
}

//  -------------------------------------------------------------------
//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
    memset(main_memory, 0, sizeof main_memory);   //  clear all memory
    memset(cache, 0, sizeof cache);               //  clear cache

// read in buffer
    AWORD buffer[N_MAIN_MEMORY_WORDS];

// size of bytes
    int size;

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

        if(i < N_CACHE_WORDS){
            cache[i].data = buffer[i];
            cache[i].dirtyBit = i;
        }

        main_memory[i] = buffer[i];
    }
}

//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  CHECK THE NUMBER OF ARGUMENTS

    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
    read_coolexe_file(argv[1]);

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();

    printf("my calculated result:       \t%i\n", result);

    return result;          // or  exit(result);
}