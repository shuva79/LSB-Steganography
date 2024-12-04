#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FAIL_CODE 1
#define SUCCESS_CODE 0

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
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t *pixel = &initial_pixel_data[x * bytes_needed + y * besides_padding];

            pixel[0] = 24;
            pixel[1] = 24;
            pixel[2] = 24;
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
    ChangeImageBits(image_data, width, height);

    // write modified image data to a new file
    fp = fopen("output_images/modified_hill.bmp", "wb");            // opens a write file for binary
    fwrite(&bitmapFile, sizeof(BMPHEAD), 1, fp);
    fwrite(&bitmapInfo, sizeof(BMPINFO), 1, fp);
    fwrite(image_data, bitmapInfo.imgSize, 1, fp);
    fclose(fp);


    free(image_data);
    return SUCCESS_CODE;
}