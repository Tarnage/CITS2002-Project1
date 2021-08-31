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
    printf("address for 7: %i\n", 7 % 32);
    bin(4);
    printf("\n");
    printf("address for 4: %i\n", 4 % 32);
    bin(1);
    printf("\n");
    printf("address for 1: %i\n", 1 % 32);
    bin(986);
    printf("\n");
    printf("address for 986: %i\n", 986 % 32);
    bin(65533);
    printf("\n");
    printf("address for 65533:%i\n", 65533 % 32);
}