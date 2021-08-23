# Project 1 2021

**The goal of this project is to emulate the execution of programs from a small, C-like programming language, on a stack-based machine, and to report some metrics of the program's execution.** 
 
    Successful completion of the project will enhance your understanding of some features of the C11 programming language, 
    and develop your understanding of how simple programs can be executed on a general-purpose computer architecture.

## The cool programming language

The cool programming language (not a real programming language that you'll find on the web!) is a very small programming language with a C-like syntax. 
It is designed to be easily understand by C programmers (who will have no difficuly relating it to C's appearance and execution) 
so that attention may focus on the execution of the programs. Its syntax is presented here.

Here is a quick summary of cool's (lack of) features:

- programs are written in text files, whose filename ends in the .cool extension.
- each self-contained program is written in a single source-file, and cannot access any external libraries.
- all programs must have a single 'int main(void)' function, which receives no parameters, and returns an integer result which provides the program's exit status.
- program termination must be through main(), as there is no exit() function.
- all variables (globals, parameters, locals) and expressions are of type integer.
- by default, all variables are initialised to zero, or may be initialised to an integer constant.
- there is no bool datatype, but non-zero expressions are considered 'true' in evaluating if and while statements.
- all functions are of type void, or return an integer.
- all identifiers must be defined before their use (no forward declarations or mutually-recursive functions).
- to aid debugging, a single print statement prints either an integer expression or a character string (the only place that strings exist).}

## Compiling and running cool programs

Programs are compiled by the coolc compiler, which translates a .cool program to an 'executable' format, whose filename ends in the .coolexe extension. 
The compiler will only be available via a webpage while the project is running (details follow), but its source code (2040 lines, so far) will be made available after the project. 
This approach allows bugs to be corrected for all users, avoids the problems of providing multi-platform support, and avoids compiler versioning and distribution problems.

The coolc compiler generates code for a (ficticious) stack-based machine. 
Code generation is performed in a single-pass without the need for an intermediate assembly-language file (although coolc displays a 'fake' assembly-language file on its webpage).

## cool programs may be compiled in two (related) ways:

    The coolc webpage allows you to upload a text file containing your cool program, to be compiled on the web-server. 
    Your source code will be displayed, together with any (at most one) syntax error. 
    If your program contains no syntax errors, a (pseudo) assembly language version of your program will also be displayed, 
    its instructions and data written to a coolexe file, and that file downloaded to your computer.

    The coolc shellscript may be downloaded to your computer and executed to send a text file containing your cool program to the web-server (described above). 
    If the uploaded file contains a syntax error, a report will be returned to your computer. 
    If your program contains no syntax errors, its instructions and data will be written to a coolexe file, and the file downloaded to your computer.

    prompt>  ./coolc.sh  myprogram.cool

Once you have a coolexe file on your computer, using either of the two mechanisms described above, 
you can emulate the execution of the program using your program, your project, named runcool:

    prompt>  ./runcool  myprogram.coolexe
The computer architecture

The target computer is a stack-based machine, with no general-purpose registers. The stack is used to hold temporary results during arithmetic evaluation, 
and memory addresses to manage the flow of each program's execution (described later).

## Control registers

### Three specific on-CPU control registers control the execution of programs:

    PC - the program counter, holds the memory address of the next instruction to fetched from the computer's RAM.
    SP - the stack pointer points to the word on the top of the computer's stack.
    FP - the frame pointer points to the current function's activation frame of the computer's stack (described later). 

The on-CPU control registers are modified in response to each program's instructions. 
For example, an instruction requiring an integer to be pushed onto the stack, results in SP being modified. 
Similarly, a function call results in the PC's value being pushed onto the stack, and the PC modified to point to the first instruction of the called function.

16-bit words
The computer on which cool programs execute can be described as an (aging) a 16-bit computer:

    a pair of 2 successive bytes are termed a (16-bit) word
    each memory address is 16-bits wide, meaning that there are 216 distinct addresses.
    memory addresses may be considered as unsigned integers, ranging from 0 to 65535.
    memory is word addressable, meaning that each address refers to one of 216 possible words of memory (or 128KB in total). 
    The on-CPU registers - PC, SP, and FP - each hold one address, and are thus 16-bits wide.
    the only supported datatype is the signed integer, ranging from -32,768 to 32,767.
    conveniently, and by design, each address and each integer occupies the same sized unit of memory - one word.

The instruction set
cool programs consist of a mixture of instructions, addresses, and data (integers). The computer's instruction set is presented here. 
(At present) there are only 17 instructions. Each instruction has a unique (integer) value (0..16) and each instruction requires one or two words of the computer's memory:

    Some instructions, such as 'add', require no additional information (here, because the values to be added reside on the stack). 
    The very next word after an 'add' instruction is the next instruction to be executed. 'add' is an example of a 0-operand instruction, and requires one word in total.
    Some instructions, such as 'call' require additional information. 
    The very next word after a 'call' instruction is the memory address of the first instruction of the function to be called. 
    'call' is an example of a 1-operand instruction, and requires two words in total.

Cache memory
More to come.
Project requirements
More to come.

Assessment
The project is due 11:59pm Friday 17th September (end of week 7). and is worth 25% of your final mark for CITS2002. It will be marked out of 50.

The project may be completed individually or in teams of two (but not teams of three). 
The choice of project partners is up to you - you will not be automatically assigned a project partner.

You are strongly advised to work with another student who is around the same level of understanding and motivation as yourself. 
This will enable you to discuss your initial design together, and to assist each other to develop and debug your joint solution. 
Work together - do not attempt to split the project into two equal parts, and then plan to meet near the deadline to join your parts together.

25 of the possible 50 marks will come from the correctness of your solution (automated marking). 
The remaining 25 marks will come from assessing your programming style, including your use of meaningful comments; well chosen identifier names; 
appropriate choice of basic data-structures, data-types and functions; and appropriate choice of control-flow constructs (manual marking).

During the marking, attention will obviously be given to the correctness of your solution. 
However, a correct and efficient solution should not be considered as the perfect, nor necessarily desirable, form of solution. 
Preference will be given to well presented, well documented solutions that use the appropriate features of the language to complete tasks 
in an easy to understand and easy to follow manner. 
That is, do not expect to receive full marks for your project simply because it works correctly. 
Remember, a computer program should not only convey a message to the computer, but also to other human programmers.

### Submission requirements

    The deadline for the project is 11:59pm Friday 17th September (end of week 7).

    You must submit your project electronically using cssubmit.

    The cssubmit program will display a receipt of your submission. You should print and retain this receipt in case of any dispute. 
    Note also that cssubmit does not archive submissions and will simply overwrite any previous submission with your latest submission.
    Your submission's C11 source file must contain C11 block comment:


    //  CITS2002 Project 1 2021
    //  Name(s):             student-name1 (, student-name2)
    //  Student number(s):   student-number1 (, student-number2)

    If working as a team, only one team member should make the team's submission.

    Your submission will be examined, compiled, and run on a contemporary Linux system (such as Ubuntu). Your submission should work as expected on this platform. 
    While you may develop your project on other computers, excuses such as "it worked at home, just not when you tested it!"  will not be accepted.

    This project is subject to UWA's Policy on Assessment - particularly §5.3 Principles of submission and penalty for late submission, and Policy on Academic Conduct. 
    In accordance with these policies, you may discuss with other students the general principles required to understand this project, 
    but the work you submit must be the result of your own team's efforts. 
    All projects will be compared using software that detects significant similarities between source code files. 
    Students suspected of plagiarism will be interviewed and will be required to demonstrate their full understanding of their project submission. 

Good luck!

Amitava Datta & Chris McDonald. 