//  runcool-soln (v1.0), written by Chris.McDonald@uwa.edu.au, 2021-
//  under licence - creativecommons.org/licenses/by-nc-sa/4.0/

//  compile with:  cc -std=c11 -Wall -Werror -o runcool-soln runcool-soln.c
//
//  add  -DUSE_FASTJEQ and/or -DUSE_CACHE  to get different versions

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THE STACK-BASED MACHINE HAS 2^16 (= 65,536) WORDS OF MAIN MEMORY
#define N_MAIN_MEMORY_WORDS     (1<<16)

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD                   uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD                   int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
AWORD                           main_memory[N_MAIN_MEMORY_WORDS];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS           32


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


//  TRY TO CATCH POTENTIAL PROBLEMS  (NOT-REQUIRED)
#include <sys/signal.h>
#include <sys/time.h>

#define MAX_EXECUTED_INSTRUCTIONS       500000000
#define MAX_RUNTIME_SECS                40

void catch_problem(int sig)
{
    printf("\n@\n");
    if(sig == SIGALRM) {
        printf("@possible-infinite-loop-detected-(not-required)\n");
        printf("@killed-after-%i-seconds-(not-required)\n", MAX_RUNTIME_SECS);
    }
    else if(sig == SIGUSR1) {
        printf("@possible-infinite-loop-detected-(not-required)\n");
        printf("@killed-after-%i-instructions-(not-required)\n",
                        MAX_EXECUTED_INSTRUCTIONS);
    }
    else {
        printf("@caught-signal-%i-(not-required)\n", sig);
    }
    exit(EXIT_FAILURE);
}

void anticipate_problems(void)
{
//  CATCH POTENTIAL PROBLEMS WITHIN SAMPLE SOLUTION (SIGNALS)
    for(int sig=SIGHUP ; sig<=SIGTERM ; ++sig) {
        signal(sig, catch_problem);
    }

//  CATCH POTENTIAL PROBLEMS WITHIN SAMPLE SOLUTION (TIMEOUT)
    struct itimerval    tv;
    tv.it_interval.tv_sec   = 0;
    tv.it_interval.tv_usec  = 0;
    tv.it_value.tv_sec      = MAX_RUNTIME_SECS;
    tv.it_value.tv_usec     = 0;
    setitimer(ITIMER_REAL, &tv, (struct itimerval *)NULL);
}

//  -------------------------------------------------------------------

//  THE STATISTICS TO BE ACCUMULATED AND REPORTED
int n_main_memory_reads     = 0;
int n_main_memory_writes    = 0;
int n_cache_memory_hits     = 0;
int n_cache_memory_misses   = 0;

int n_instructions          = 0;
int n_function_calls        = 0;
int max_fncall_depth        = 0;
int max_stack_depth         = 0;
int total_reads             = 0;

void clear_statistics(void)
{
    n_main_memory_reads     = 0;
    n_main_memory_writes    = 0;
    n_cache_memory_hits     = 0;
    n_cache_memory_misses   = 0;

    n_instructions          = 0;
    n_function_calls        = 0;
    max_fncall_depth        = 0;
    max_stack_depth         = 0;
    total_reads             = 0;
}

void report_statistics(void)
{
#define REPORT(s,v)         printf("@%-44s %i\n", s, v)

#if defined(USE_FASTJEQ)
    REPORT("number-of-main-memory-reads-(fast-jeq)",    n_main_memory_reads);
    REPORT("number-of-main-memory-writes-(fast-jeq)",   n_main_memory_writes);
#else
    REPORT("number-of-main-memory-reads",               n_main_memory_reads);
    REPORT("number-of-main-memory-writes",              n_main_memory_writes);
#endif

    REPORT("number-of-cache-memory-hits",               n_cache_memory_hits);
    REPORT("number-of-cache-memory-misses",             n_cache_memory_misses);

    printf("@\n");
    REPORT("number-of-instructions-(not-required)",         n_instructions);
    REPORT("number-of-function-calls-(not-required)",       n_function_calls);
    REPORT("maximum-function-call-depth-(not-required)",    max_fncall_depth);
    REPORT("maximum-stack-depth-(not-required)",            max_stack_depth);
    printf("@%-44s %.1f%%\n", "percentage-of-cache-hits-(not-required)",
            100.0*n_cache_memory_hits/total_reads);
#undef  REPORT
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

#if defined(USE_CACHE)
struct {
    AWORD       caching;
    bool        dirty;
    AWORD       contents;
} cache_memory[N_CACHE_WORDS];

//  INITIALIZE cache_memory SO THAT ALL FUTURE MEMORY ACCESSES PASS THROUGH IT
void init_cache(void)
{
    AWORD   caching = (N_MAIN_MEMORY_WORDS - 1);    // highest stack address

//  INITIALIZE cache_memory TO 'PRETEND' THAT IT IS CACHING VALUES FROM STACK
    for(int n=0 ; n<N_CACHE_WORDS ; ++n) {
        int cw                      = caching % N_CACHE_WORDS;

        cache_memory[cw].caching    = caching;
        cache_memory[cw].dirty      = true;
        cache_memory[cw].contents   = 0;
        --caching;
    }
}

//  THESE FUNCTIONS IMPLEMENT WRITE-BACK CACHING
AWORD read_memory(int address)
{
    int cw  = address % N_CACHE_WORDS;

    ++total_reads;

//  ARE WE ALREADY CACHING THE ADDRESS WE WANT?
    if(cache_memory[cw].caching == address) {
        ++n_cache_memory_hits;
    }
    else {
        ++n_cache_memory_misses;

//  IF A DIFFERENT ADDRESS AND DIRTY, WRITE-BACK
        if(cache_memory[cw].dirty) {
            main_memory[ cache_memory[cw].caching ] = cache_memory[cw].contents;
            ++n_main_memory_writes;
//          cache_memory[cw].dirty  = false;
        }

//  UPDATE THE CACHE WITH THE REQUESTED DATA FROM MAIN-MEMORY
        cache_memory[cw].caching    = address;
        cache_memory[cw].contents   = main_memory[address];
        ++n_main_memory_reads;
        cache_memory[cw].dirty      = false;
    }
    return cache_memory[cw].contents;
}

void write_memory(AWORD address, AWORD value)
{
    int cw  = address % N_CACHE_WORDS;

//  IF WRITING TO THE ALREADY CACHED ADDRESS
    if(cache_memory[cw].caching == address) {
        if(cache_memory[cw].contents != value) {    // need to update cache?
            cache_memory[cw].dirty      = true;
            cache_memory[cw].contents   = value;
        }
    }
    else {

//  DIFFERENT ADDRESS, IS CURRENT LOCATION DIRTY? => WRITE-BACK
        if(cache_memory[cw].dirty) {
            main_memory[ cache_memory[cw].caching ] = cache_memory[cw].contents;
            ++n_main_memory_writes;
            cache_memory[cw].dirty  = false;
        }
//  UPDATE THE CACHE WITH NEW DATA WRITTEN BY THE PROGRAM
        cache_memory[cw].dirty      = true;
        cache_memory[cw].caching    = address;
        cache_memory[cw].contents   = value;
    }
}

#else

//  THESE FUNCTIONS PERFORM NO CACHING
AWORD read_memory(int address)
{
    ++total_reads;
    ++n_main_memory_reads;
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{
    ++n_main_memory_writes;
    main_memory[address]    = value;
}
#endif

//  -------------------------------------------------------------------

//  EXECUTE THE INSTRUCTIONS IN main_memory[]
int execute_stackmachine(void)
{
//  THE 3 ON-CPU CONTROL REGISTERS:
    int PC      = 0;                    // 1st instruction is at address=0
    int FP      = 0;                    // frame pointer
    int SP      = N_MAIN_MEMORY_WORDS;  // initialised to top-of-stack
                                        // first operation will always PUSH

    int fncall_depth = 0;
    int stack_depth  = 0;

#define FETCH(x)        x = read_memory(PC);        \
                        ++PC
#define PUSH(value)     --SP;                       \
                        write_memory(SP, value);    \
                        ++stack_depth;              \
                        if(max_stack_depth < stack_depth) max_stack_depth = stack_depth
#define POP(result)     result = read_memory(SP);   \
                        ++SP;                       \
                        --stack_depth

#if defined(USE_FASTJEQ)

#if defined(USE_CACHE)
    printf("@@sample solution with fast-jeq and write-back cache\n");
#else
    printf("@@sample solution with fast-jeq and no cache\n");
#endif

#else

#if defined(USE_CACHE)
    printf("@@sample solution with write-back cache\n");
#else
    printf("@@sample solution with no cache\n");
#endif

#endif

    while(true) {
//  TEMPORARY VALUES COPYING MEMORY AND REGISTERS
        IWORD       instruction;
        AWORD       address;
        IWORD       value1, value2, offset;

//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED
        FETCH(instruction);
        ++n_instructions;

//      printf("\t\t\t\t\tPC=%-4i%-8s\tSP=%i\n",
//              PC-1, INSTRUCTION_name[instruction], SP);

//  TRY TO CATCH POTENTIAL INFINITE LOOPS
        if(n_instructions >= MAX_EXECUTED_INSTRUCTIONS) {
            catch_problem(SIGUSR1);
        }

//  TEST FOR halt OUTSIDE OF THE switch AS WE CAN'T break TWICE
        if(instruction == I_HALT) {
            break;
        }

        switch (instruction) {
        case I_NOP :                                // nothing to do, just skip
                        break;
        case I_ADD :
                        POP(value1);
                        POP(value2);
                        PUSH(value1 + value2);
                        break;
        case I_SUB :
                        POP(value1);
                        POP(value2);
                        PUSH(value2 - value1);
                        break;
        case I_MULT :
                        POP(value1);
                        POP(value2);
                        PUSH(value1 * value2);
                        break;
        case I_DIV :
                        POP(value1);
                        POP(value2);
                        PUSH(value2 / value1);
                        break;
        case I_CALL :
                        FETCH(address);             // find callee's 1st inst'n
                        PUSH(PC);                   // save return address
                        PUSH(FP);                   // save caller's FP
                        FP  = SP;                   // set callee's FP
                        PC  = address;              // jump to callee

                        ++n_function_calls;
                        ++fncall_depth;
                        if(max_fncall_depth < fncall_depth) {
                            max_fncall_depth = fncall_depth;
                        }
                        break;
        case I_RETURN :
                        FETCH(offset);              // offset of returned value
                        address = read_memory(FP+1);
                        POP(value1);                // get value to return
                        write_memory(FP+offset, value1);

                        stack_depth -= (FP+offset - SP);
                        --fncall_depth;

                        SP  = FP+offset;            // SP points to returned value
                        FP  = read_memory(FP);      // restore caller's FP
                        PC  = address;              // jump back to caller
                        break;
        case I_JMP :
                        FETCH(address);             // unconditional jump
                        PC  = address;
                        break;
        case I_JEQ :
#if defined(USE_FASTJEQ)
                        POP(value1);
                        if(value1 == 0) {           // jump iff TOS is zero
                            FETCH(address);
                            PC  = address;
                        }
                        else {
                            ++PC;
                        }
#else
                        FETCH(address);
                        POP(value1);
                        if(value1 == 0) {           // jump iff TOS is zero
                            PC  = address;
                        }
#endif
                        break;
        case I_PRINTI :
                        POP(value1);
                        printf("%i", value1);
                        break;
        case I_PRINTS :
                        FETCH(address);             // beginning of string
                        while(true) {
                            char ch;

                            value1  = read_memory(address);
                            ch  = value1 % 256;
                            if(ch == '\0')
                                break;
                            putchar(ch);

                            ch  = value1 / 256;
                            if(ch == '\0')
                                break;
                            putchar(ch);

                            ++address;
                        }
                        break;
        case I_PUSHC :
                        FETCH(value1);
                        PUSH(value1);
                        break;
        case I_PUSHA :
                        FETCH(address);
                        value1  = read_memory(address);
                        PUSH(value1);
                        break;
        case I_PUSHR :
                        FETCH(offset);
                        address = FP+offset;
                        value1  = read_memory(address);
                        PUSH(value1);
                        break;
        case I_POPA :
                        FETCH(address);
                        POP(value1);
                        write_memory(address, value1);
                        break;
        case I_POPR :
                        FETCH(offset);
                        address = FP+offset;
                        POP(value1);
                        write_memory(address, value1);
                        break;
        }
    }

//  THE RESULT OF EXECUTING THE PROGRAM IS FOUND ON THE TOP-OF-STACK
    IWORD result;
    POP(result);
    return result;
}

//  -------------------------------------------------------------------

#include <fcntl.h>

char    *argv0;                     // program's name

//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
//  ATTEMPT TO OPEN coolexe FILE
    int fd = open(filename, O_RDONLY, 0);
    if(fd < 0) {
        fprintf(stderr, "%s: cannot open '%s'\n", argv0, filename);
        exit(EXIT_FAILURE);
    }

    memset(main_memory, 0, sizeof main_memory);   //  clear all memory

//  READ CONTENTS OF coolexe FILE  (AT MOST THE SIZE OF main_memory[] )
    if(read(fd, main_memory, sizeof main_memory) <= 0) {
        fprintf(stderr, "%s: cannot read '%s'\n", argv0, filename);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
    char *argv0 = (argv0 = strrchr(argv[0],'/')) ? argv0+1 : argv[0];
    int  result;

//  CHECK THE NUMBER OF ARGUMENTS
    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.coolexe\n", argv0);
        exit(EXIT_FAILURE);
    }

    anticipate_problems();
    read_coolexe_file(argv[1]);
#if defined(USE_CACHE)
    init_cache();
#endif
    clear_statistics();
    result = execute_stackmachine();
    report_statistics();
    printf("@\n@exit(%i)\n", result);

    return result;                  // or  exit(result);
}

//  vim: ts=8 sw=4
