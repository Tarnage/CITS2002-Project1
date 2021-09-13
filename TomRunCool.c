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
//TODO dont need valid, tag see https://secure.csse.uwa.edu.au/run/help2002?p=np&a=432
struct
{   
    //if dirty we need to write to memory, before we write over the line
    AWORD       dirtyBit;
    AWORD       tag;
    AWORD         data;

} cache[N_CACHE_WORDS];


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
    printf("@number-of-instructions     \t%i\n",    n_instructions);        
    printf("@number-of-function-calls   \t%i\n",    n_func_calls);      
    printf("@maximum-function-call-depth\t%i\n",    m_func_call_depth);   
    printf("@maximum-stack-depth        \t%i\n",    m_stack_depth);
    printf("@\n");           
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY
void printCache()
{   
    printf("Current Cache:\n");
    for (int i = 0; i < N_CACHE_WORDS; ++i){
        printf("%i ", cache[i].data);
    }
    printf("\n");
}

AWORD read_memory(int address)
{
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{
    main_memory[address] = value;
}

AWORD read_cache_memory(int address, AWORD offset)
{   
    int cacheAddress = (address % N_CACHE_WORDS) + offset;
    if(cache[cacheAddress].dirtyBit != address){
        //TODO impelemnt dirtybit
        ++n_cache_memory_misses;
        ++n_main_memory_reads;
        ++n_main_memory_writes;
        write_memory(cache[cacheAddress].dirtyBit , cache[cacheAddress].data);
        cache[cacheAddress].data = read_memory(address);
        cache[cacheAddress].dirtyBit = address;

        return cache[cacheAddress].data;
    }
    else{
        ++n_cache_memory_hits;
        return cache[cacheAddress].data;
    }
}

void write_cache_memory(AWORD address, AWORD value, AWORD offset)
{   
//    printCache();
    
    int cacheAddress = (address % N_CACHE_WORDS) + offset;
    //TODO add a dirty bit check or something 
    if(cache[cacheAddress].dirtyBit != address){
        ++n_main_memory_writes;
        write_memory(cacheAddress, cache[cacheAddress].data);
    }
    cache[cacheAddress].data        = value;
    cache[cacheAddress].dirtyBit    = address;
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
        ++n_main_memory_reads;
        IWORD instruction   = read_memory(PC);
        ++PC;
        ++n_instructions;

//  PRINT THE INSTRUCTIONS FOUND IN main_memory[]
//        printArray(PC, size);

// PRINTS REGISTERS
//        printf("Current Instruction being executed: %i\n", instruction);
//        printf("PC Value: %i\n", PC);
//        printf("SP Value: %i\n", SP);
//        printStack(SP);
//        printf("FP Value: %i\n", FP);
//        printFrame(FP);
        printCache();
//        printf("current stack depth %i\n", m_stack_depth);

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
                write_cache_memory( SP, read_cache_memory(SP - 1, 0) + read_cache_memory(SP, 0), 0 );
                break;

            case I_SUB:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, 0) - read_cache_memory(SP - 1, 0), 0 );
                break;
            
            case I_MULT:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP - 1, 0) * read_cache_memory(SP, 0), 0 );
                break;

            case I_DIV:
                ++SP;
                write_cache_memory( SP, read_cache_memory(SP, 0) / read_cache_memory(SP - 1, 0), 0 );
                break;

            case I_CALL:
            //TODO CHECK
                ++n_func_calls;
                ++m_func_call_depth;

            // push return address to TOS
                ++m_stack_depth;
                --SP;
                write_cache_memory(SP, PC + 1, 0);

            // push FP address to TOS
                ++m_stack_depth;
                --SP;
                write_cache_memory(SP, FP, 0);

            // set FP 
                FP = SP;

            // start execution of next function
                ++n_main_memory_reads;
                PC = read_memory(PC);
                break;

            case I_RETURN:
            //TODO CHECK
            // currently holds the off set
                ++n_main_memory_reads;
                instruction = read_memory(PC);

            // PC goes back to the following instruction that called current function
                PC = read_cache_memory(FP + 1, 0);
            
            // calculated return value placed in the FP offset
                write_cache_memory( FP, read_cache_memory(SP, 0), instruction );

            // SP reset to actual TOS
                SP = FP + instruction;

            //FP reset
                FP = read_cache_memory(FP, 0);
                break;

            case I_JMP:
            // TODO CHECK CORRECTNESS
            //    printf("Entered JMP\n");
                ++n_main_memory_reads;
                PC = read_memory(PC);
                break;

            case I_JEQ:
            // TODO CHECK CORRECTNESS
            //    printf("Entered JEQ\n");
                ++n_main_memory_reads;
                --m_stack_depth;
                if( read_cache_memory(SP, 0) == 0 ) PC = read_memory(PC);
                else ++PC;
                ++SP;
                break;

            case I_PRINTI:
            // TODO IMPLEMENT
            //    printf("Entered PRINTI\n");
            // insruction holds TOS
                ++n_main_memory_reads;
                instruction = read_memory(SP);
                ++SP;
                printf("%i", instruction);
                break;

            case I_PRINTS:
            // TODO IMPLEMENT
            //    printf("Entered PRINTI\n");
            // instruction holds the address of the next instruction when print is finished
                instruction = PC + 1;
                ++n_main_memory_reads;
                PC = read_memory(PC);

                while(true){
                    //read value from PC 
                    ++n_main_memory_reads;
                    AWORD val = read_memory(PC);
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
                ++n_main_memory_reads;
                ++m_stack_depth;
                --SP;
                write_cache_memory( SP, read_memory(PC), 0 );
                ++PC;
                break;

            case I_PUSHA:
            // TODO CHECK
                ++m_stack_depth;
                n_main_memory_reads += 2;
                // instruction holds the address of value to push
                instruction = read_memory(PC);
                --SP;
                write_cache_memory( SP, read_memory(instruction), 0 );
                ++PC;
                break;

            case I_PUSHR:
            // TODO CHECK
                ++m_stack_depth;
                ++n_main_memory_reads ;
            // instruction holds the offset
                instruction = read_memory(PC);
                --SP;
                write_cache_memory( SP, read_cache_memory(FP, instruction), 0 );
                ++PC;
                break;

            case I_POPA:
            // TODO CHECK
                ++n_main_memory_reads;
                write_cache_memory( read_memory(PC), read_cache_memory(SP, 0), 0 );
                ++SP;
                ++PC;
                break;
            
            case I_POPR:
            // TODO CHECK
            // instruction holds the offset
                ++n_main_memory_reads;
                instruction = read_memory(PC);
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

//    if(argc != 2) {
//        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
//        exit(EXIT_FAILURE);
//    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
//    read_coolexe_file(argv[1]);

// ADDED FOR TESTING MAKE SURE WE UNDO THE COMMENTS BEFORE SUBMIT
    read_coolexe_file("Coolexe/parameters.coolexe");

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();

    printf("my calculated result:       \t%i\n", result);

    return result;          // or  exit(result);
}
