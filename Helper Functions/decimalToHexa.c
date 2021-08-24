// This function will convert decimal to hexa and return the hexa value 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

void decimalToHexa(uint16_t decimal)
{
    int16_t quotient, remainder;
    int i = 0;
    int j = 0;
    char hexadecimal[100];

    quotient = decimal;

    while(quotient != 0){
        remainder = quotient % 16;
        if (remainder < 10) {
            hexadecimal[j++] = 48 + remainder;
        }
        else{
            hexadecimal[j++] = 55 + remainder;
        }
        quotient = quotient / 16;
    }
    
    //test 
    printf("The Hexa of %u is: %s\n", decimal, hexadecimal);

}


int main(int argc, char *argv[])
{

    decimalToHexa(192);
    
    char hexa[] = "C";
    int16_t num = hexa - '0';

    printf("Hexa %s is int16_t: %i\n", hexa, num);

    return 0;
}