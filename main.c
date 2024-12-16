#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parse.h"
#include "extract.h"

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

int ChangeImageBits(uint8_t *initial_pixel_data, int width, int height, FILE *fp)
{
	printf(
		"\n"
		"|=====================|\n"
		"|      EMBEDDING      |\n"
		"|=====================|\n"
		);


    int bytes_needed = 3;  // Each pixel is 3 bytes (RGB)
    int padding = (4 - (width * bytes_needed) % 4) % 4;  // Row padding to make the row size a multiple of 4
    int row_size = (width * bytes_needed) + padding;

    char input_text[100];
    int count_input_char_position = 0;

    printf("Enter a value to be embedded: ");
    fgets(input_text, sizeof(input_text) , stdin );
    // Remove newline character if present
    input_text[strcspn(input_text, "\n")] = '\0';

    size_t input_text_len = strlen(input_text);

    printf("Embedding data: %s\n", input_text);

    // Iterate through the image's pixel data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x += 3) {  // Process each 3-pixel block
            if (count_input_char_position < input_text_len) {
                // For each character, parse it into binary
                char binary_string[9] = {0}; // since our array will always be 9
                
                parseBinary(input_text[count_input_char_position], binary_string);

                // Embed the binary data into the least significant bits of the RGB values
                uint8_t *pixel1 = &initial_pixel_data[(x + 0) * bytes_needed + y * row_size];
                uint8_t *pixel2 = &initial_pixel_data[(x + 1) * bytes_needed + y * row_size];
                uint8_t *pixel3 = &initial_pixel_data[(x + 2) * bytes_needed + y * row_size];

                // Embed each bit of the character into the RGB channels
                int j = 0;
                pixel1[0] = (pixel1[0] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel1[1] = (pixel1[1] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel1[2] = (pixel1[2] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);

                pixel2[0] = (pixel2[0] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel2[1] = (pixel2[1] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel2[2] = (pixel2[2] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);

                pixel3[0] = (pixel3[0] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel3[1] = (pixel3[1] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);
                pixel3[2] = (pixel3[2] & ~1) | (convertToRawBinary(binary_string[j++]) & 1);

                // Set the continuation bit if there are more characters to embed
                pixel3[2] = (pixel3[2] & ~1) | (input_text[count_input_char_position + 1] != '\0');

                count_input_char_position++;
            }

            // Handle end of input: we mark the last pixel with continuation = 0
            if (input_text[count_input_char_position] == '\0') {
                return END_OF_INPUT;
            }
        }
    }

    return SUCCESS_CODE;
}


int main(int argc , char *argv[])
{

    if(argc != 3){
            printf("Usage ./program -embed/extract picture\n"
                   "Example: ./program -embed picture.bmp\n"
                   "Example2: ./program -extract picture.bmp\n");
            exit(FAIL_CODE);
    }
    
    int embed_flag = 0; // initiate at 0 
    FILE *fp = fopen(argv[2] ,"rb"); // that way we can use any image // but maybe check if in that case?
    printf("%s" , argv[2]);
    if (!fp)
    {
        printf("No pic :(\n");
        return FAIL_CODE;
    }
    
      if (strcasecmp(argv[1], "-embed") == 0 ) // Embedding data
    {
        embed_flag = 1; // only change if we embed
    }
      else if(strcasecmp(argv[1] , "-extract") != 0){
      		printf("Error parsing options");
      		exit(FAIL_CODE); // we could use some different code base on the return 
      	}
    


    BMPHEAD bitmapFile;
    BMPINFO bitmapInfo;

    // Reading file headers
    fread(&bitmapFile, sizeof(BMPHEAD), 1, fp);
    fread(&bitmapInfo, sizeof(BMPINFO), 1, fp);

    int width = bitmapInfo.width;
    int height = bitmapInfo.height;

    printf("Width: %d\n", width);
    printf("Height: %d\n", height);

    // Read image data
    uint8_t *image_data = (uint8_t *) malloc(bitmapInfo.imgSize);

    printf("\nBitmap offset: %x\n", bitmapFile.fileOffsetBits);
    if (image_data == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        free(image_data);
        return FAIL_CODE;
    }

    // Move to the offset of the pixel data
    fseek(fp, bitmapFile.fileOffsetBits, SEEK_SET);

    // Read the pixel data
    fread(image_data, bitmapInfo.imgSize, 1, fp);       // image_data stores the pixel values of the bitmap image

    fclose(fp);

    // Now perform image manipulation or extraction based on the user's choice
    if (embed_flag == 1)
    {
        // Embedding case, modify the image and save it
        const char output[] = "output.bmp"; // regardless of option , this shall be the out picture
        fp = fopen(output, "wb");            
        
        fwrite(&bitmapFile, sizeof(BMPHEAD), 1, fp);
        fwrite(&bitmapInfo, sizeof(BMPINFO), 1, fp);

        // Call embedding function to modify pixel data
        ChangeImageBits(image_data, width, height, fp);

        // Write modified image data back to file
        fwrite(image_data, bitmapInfo.imgSize, 1, fp);
        fclose(fp);
    }
    else
    {
        // Extraction case, read from the modified image
        ExtractImageBits(image_data, width, height); // Process the modified image data
    }

    // Free allocated memory
    free(image_data);
    return SUCCESS_CODE;
}

