#include <stdio.h>
#include <string.h>

void parseBinary(int ascii_value)
{
    char binary_value;
    char binary_string[8];

    for (int i = 7; i >= 0; i--)
    {
        // we will use ternary to perform conversion (ASCII-> Binary) because it is easier than using raw bytes
        // if the resulting value from the operation is 1 (true), it returns a '1' string
        // if the resulting value from the operation is 0 (false), it returns a '0' string 
        binary_value = ( (ascii_value >> i) & 1 ) ? '1' : '0';
        binary_string[7-i] = binary_value;
    }

    // we will set a null terminator to avoid garbage values being printed
    binary_string[8] = '\0';

    printf("Binary value: %s\n", binary_string);
     
}

char* parseText()
{
    char* input_text="abcd./1";

    for (int i = 0; i < strlen(input_text); i++)
    {
        parseBinary((int) input_text[i]);
    }
    // parse individual characters here, simply typecasting will ensure that the value will be converted ASCII->decimal
    return "========";
}
