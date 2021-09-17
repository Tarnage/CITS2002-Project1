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
    int8_t        dirty;
    int8_t        isValid;
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

// dirty = 0, not dirty (clean) = 1
// not valid = 0, is valid = 1
AWORD read_cache_memory(int address)
{   
    int cacheAddress = address % N_CACHE_WORDS;

    if(cache[cacheAddress].tag != address || cache[cacheAddress].isValid == 0){
        ++n_cache_memory_misses;

        if(cache[cacheAddress].dirty == 0 && 
                cache[cacheAddress].isValid == 1 && 
                    cache[cacheAddress].tag != address){
            write_memory(cache[cacheAddress].tag, cache[cacheAddress].data);
        }
        cache[cacheAddress].data        = read_memory(address);
        cache[cacheAddress].tag         = address;
        cache[cacheAddress].dirty       = 1;
        cache[cacheAddress].isValid     = 1;

        return cache[cacheAddress].data;
    }
    else{
        ++n_cache_memory_hits;
        return cache[cacheAddress].data;
    }
}

void write_cache_memory(AWORD address, AWORD value)
{     
    int cacheAddress = address % N_CACHE_WORDS;

    if(cache[cacheAddress].dirty == 0 && 
            cache[cacheAddress].isValid == 1 && 
                cache[cacheAddress].tag != address){
        write_memory(cache[cacheAddress].tag, cache[cacheAddress].data);
    }
    cache[cacheAddress].data         = value;
    cache[cacheAddress].tag          = address;
    cache[cacheAddress].dirty        = 0;
    cache[cacheAddress].isValid      = 1;
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
        IWORD instruction   = read_cache_memory(PC);
        ++PC;

        if(instruction == I_HALT){
            break;
        }

        switch(instruction)
        {
            case I_NOP:
            // NO OPERATION, CONTINUES TO NEXT INSTRUCTION
                break;

            // standard arithmetic operations 
            case I_ADD:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP - 1) + read_cache_memory(SP) );
                break;

            case I_SUB:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP) - read_cache_memory(SP - 1));
                break;
            
            case I_MULT:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP - 1) * read_cache_memory(SP) );
                break;

            case I_DIV:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP) / read_cache_memory(SP - 1));
                break;

            case I_CALL:
            // I_CALL will store the current state of the process before calling the next function
            
            // store return address
                --SP;
                write_cache_memory(SP, PC + 1);

            // store the frame pointer
                --SP;
                write_cache_memory(SP, FP);

            // set frame pointer to the top of the stack
                FP = SP;

            // start execution of the next function
                PC = read_cache_memory(PC);
                break;

            case I_RETURN:
            // currently holds the off set
                instruction = read_cache_memory(PC);

            // address of next opcode to execute
                PC = read_cache_memory(FP + 1);
            
            // store functions return value to TOS
                write_cache_memory( FP + instruction, read_cache_memory(SP) );
            
            // set stack pointer to TOS
                SP = FP + instruction;

            // set frame pointer to prevoius process
                FP = read_cache_memory(FP);
                break;

            case I_JMP:
            // unconditonal jump
                PC = read_cache_memory(PC);
                break;

            case I_JEQ:
            // jump iff TOS is 0, (assumption but not less than 0?) 
                if( read_cache_memory(SP) == 0 ) PC = read_cache_memory(PC);
                else ++PC;
                ++SP;
                break;

            case I_PRINTI:
            // prints interger at the TOS to STDOUT
            // insruction holds TOS
                instruction = read_cache_memory(SP);
                ++SP;
                printf("%i", instruction);
                break;

            case I_PRINTS:
            // prints string to STDOUT
            // instruction holds the address of the next instruction when print is finished
                instruction = PC + 1;
                PC = read_cache_memory(PC);

                while(true){

                    AWORD val = read_cache_memory(PC);
                    ++PC;
                    //Each 16-bits integer contain two char
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
            // push constant to TOS
                --SP;
                write_cache_memory( SP, read_cache_memory(PC) );
                ++PC;
                break;

            case I_PUSHA:
            // push word at following address to TOS
            // instruction holds the address of value to push
                instruction = read_cache_memory(PC);
                --SP;
                write_cache_memory( SP, read_cache_memory(instruction) );
                ++PC;
                break;

            case I_PUSHR:
            // push word, that is offset from frame pointer to TOS
            // instruction holds the offset
                instruction = read_cache_memory(PC);
                --SP;
                write_cache_memory( SP, read_cache_memory(FP + instruction) );
                ++PC;
                break;

            case I_POPA:
            // pop word at the following address off the stack
                write_cache_memory( read_cache_memory(PC), read_cache_memory(SP) );
                ++SP;
                ++PC;
                break;
            
            case I_POPR:
            //pop word, that is offset from frame pointer to TOS
            // instruction holds the offset
                instruction = read_cache_memory(PC);
                write_cache_memory( FP + instruction, read_cache_memory(SP) );
                ++SP;
                ++PC;
                break;
        }
    }

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
    return read_cache_memory(SP);
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