#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

//https://www.geeksforgeeks.org/binary-representation-of-a-given-number/
void bin(uint16_t n)
{
    uint16_t i;
    for (i = 1 << 15; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

int main(void)
{
    bin(7);
    printf("\n");
    bin(4);
    printf("\n");
    bin(1);
    printf("\n");
    bin(65535);
    printf("\n");
    printf("%i", 65535 % 32);
}