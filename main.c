#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parse.h"

#define FAIL_CODE 1
#define SUCCESS_CODE 0
#define END_OF_INPUT 0

#pragma pack(push, 1) // Disable padding
// for more info on pragma look into this https://stackoverflow.com/questions/3318410/pragma-pack-effect

typedef struct      // this structure is found in BITMAPFILEHEADER structure in wingdi.h check msndocs for this 
{
    uint16_t fileType;        // File type must be 0x4d42 or "BM" in ascii. uint8_t means unsigned integer of size 16 bits or 1 WORD
    uint32_t fileSize;        // Size of the bitmap file in bytes. DWORD is equal to 32 bits 
    uint16_t fileReserved1;   // Reserved value, must be 0  
    uint16_t fileReserved2;   // Reserved value, must be 0
    uint32_t fileOffsetBits;  // Offset value in bytes. From the beginning of the BMPHEAD structure to the bitmap bits.   
} BMPHEAD;

typedef struct 
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imgSize;
    uint32_t xRes;
    uint32_t yRes;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} BMPINFO;

#pragma pack(pop) // Re-enable padding

int ChangeImageBits(uint8_t *initial_pixel_data, int width, int height)       // since we'll be dealing with characters, which are of size 8 bits, we will set pixel data value to uint8_t
{
    // since we are manipulating 24-bit bitmap files, we need 3 bytes. (8 * 3 = 24)
    int bytes_needed = 3;

    // we need this to make sure that the padding portion is not affected
    // the image repeates a portion onto the output image if padding is also manipulated. 
    // besides_padding ensures the alignment between total no. of bytes in a row of pixel data and multiple of 4
    // refer to this for more info: https://www.cs.umd.edu/~meesh/cmsc411/website/projects/outer/memory/align.htm

    int besides_padding = ((bytes_needed * width) + 3 ) & ~3; 
    printf("\nTotal bytes needed= %d\n",besides_padding);
    //char* parse_return = parseText();

    // todo: allocate memory dynamically
    // memory allocation
    char input_text[100];
    int count_input_char_position = 0;

    printf("\nEnter a value to be embedded: \n");
    scanf("%s", input_text);
    printf("\nHola %s\n", input_text);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x+=3)
        {
            // pixel holds the combined 24-bit value of an individual BMP image pixel 
            // for each iteraction, we will take three image pixels to store a single character
            
            /*
                Since a single character takes 8 bytes of space, we will require 3 pixels to store a single character if we choose to only make use of 1 LSB
                3 pixels = 9 RGB values, 8 RGB values used for hiding the information and the trailing value to indicate continuation of data hiding
                1 = Continue encoding
                0 = Finish encoding 
            */

            uint8_t *pixel1 = &initial_pixel_data[x * bytes_needed + y * besides_padding];
            uint8_t *pixel2 = &initial_pixel_data[(x+1) * bytes_needed + y * besides_padding];
            uint8_t *pixel3 = &initial_pixel_data[(x+2) * bytes_needed + y * besides_padding];

        
            // Here, pixel[0], [1] and [2] set the B G R values respectively.
            /*
                Understanding negation operation:
                    The negation operator `~` flips bits. So here, 1 (0000 0001) gets flipped.
                    So, ~1 is 1111 1110. When you perform the & operation, it ends up setting all 7 bits to 1 and the last bit to 0.
                    This would pretty much mean every bit except for the least significant one stays the same.

                What we need in order to embed the data:
                value = ( value & ∼1 ) | ( bit & 1 )


            */
            int a[3] = {pixel1[0], pixel1[1], pixel1[2]};

            // we have to allocate 9 bytes of memory, 1 for each char 
            // Since 1 char has 8 bits of info and we have to store each bit as a character
            // 1 * 8 + 1 = 9 bytes of memory allocated (1 for termination bit)
            char* abc = (char*) malloc(9 * sizeof(char));
    
            // parse the text and store the last binary string in the buffer
            parseText(input_text[count_input_char_position], &abc);
            printf("\ninput char: %c\n", input_text[count_input_char_position]);
            count_input_char_position++;
            

            // this is for debugging 
            printf("\nlast buffer val: %s\n", abc);
            int j = 0;
            printf("\nCheck here\n");

            pixel1[0] = (pixel1[0] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            pixel1[1] = (pixel1[1] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            pixel1[2] = (pixel1[2] & ~1) | (convertToRawBinary(abc[j++]) & 1);

            // pixel2[0] = 0;
            // pixel2[1] = 0;
            // pixel2[2] = 0;
            pixel2[0] = (pixel2[0] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            pixel2[1] = (pixel2[1] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            pixel2[2] = (pixel2[2] & ~1) | (convertToRawBinary(abc[j++]) & 1);
        
            pixel3[0] = (pixel3[0] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            pixel3[1] = (pixel3[1] & ~1) | (convertToRawBinary(abc[j++]) & 1);
            // for this, you have to create some form of condition. Treat this as a continuation bit

            pixel3[2] = (pixel3[2] & ~1) | (convertToRawBinary(abc[j++]) & 1);

            printf("\nCheck here\n"); 

            if (input_text[count_input_char_position] == '\0')
                return END_OF_INPUT;
            free(abc);
        }
    }
    return SUCCESS_CODE;   // return just in case 
}

int main()
{
    FILE *fp;
    char input_image_name[50] = "input_images/hotel.bmp";
    fp = fopen(input_image_name,"rb");

    if (!fp)
    {
        printf("No pic :(\n");
        return FAIL_CODE;       
    }
    
    BMPHEAD bitmapFile;
    BMPINFO bitmapInfo;

    // reading file headers
    fread(&bitmapFile, sizeof(BMPHEAD), 1, fp);
    fread(&bitmapInfo, sizeof(BMPINFO), 1, fp);
    
    int width = bitmapInfo.width;
    int height = bitmapInfo.height;

    printf("Width: %d\n", width);
    printf("Height: %d\n", height);

    // read image data
    uint8_t *image_data = (uint8_t *) malloc(bitmapInfo.imgSize);

    if (image_data == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        free(image_data);
        return FAIL_CODE;
    }

    fseek(fp, bitmapFile.fileOffsetBits, SEEK_SET);     // SEEK_SET is a macro defined as 0 aka explicitly setting an offset https://stackoverflow.com/questions/56433377/why-is-the-beginning-of-a-c-file-stream-called-seek-set
    fread(image_data, bitmapInfo.imgSize, 1, fp);       // image_data stores the pixel values of the bitmap image
    fclose(fp);

    // perform image manipulation
    printf("\nEmbed or extract? (e/x)\n");
    if (getc(stdin) == 'e')
        ChangeImageBits(image_data, width, height);
    else

    // write modified image data to a new file
    fp = fopen("output_images/modified_hill.bmp", "wb");            // opens a write file for binary
    fwrite(&bitmapFile, sizeof(BMPHEAD), 1, fp);
    fwrite(&bitmapInfo, sizeof(BMPINFO), 1, fp);
    fwrite(image_data, bitmapInfo.imgSize, 1, fp);
    fclose(fp);


    free(image_data);
    return SUCCESS_CODE;
}