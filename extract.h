#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_TEXT_LENGTH 100 

int ExtractImageBits(uint8_t *initial_pixel_data, int width, int height)
{
    // printf("The function ExtractImageBits has been called\n"); <-- debug statement 
    
    int bytes_needed = 3;
    int besides_padding = (4 - (width * bytes_needed) % 4) % 4;
    int bit_count = 0;
    
    char extracted_bits[8 * MAX_TEXT_LENGTH + 1]; // 2 way here -> use malloc -> transfer the size as a parameter
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x+=3)
        {
            uint8_t *pixel1 = &initial_pixel_data[(x) * bytes_needed  + y * besides_padding];
            uint8_t *pixel2 = &initial_pixel_data[(x+1) * bytes_needed  + y * besides_padding];
            uint8_t *pixel3 = &initial_pixel_data[(x+2) * bytes_needed  + y * besides_padding];

            extracted_bits[bit_count++] = (pixel1[0] & 1) + '0';  
            extracted_bits[bit_count++] = (pixel1[1] & 1) + '0';  
            extracted_bits[bit_count++] = (pixel1[2] & 1) + '0';  

            extracted_bits[bit_count++] = (pixel2[0] & 1) + '0';  
            extracted_bits[bit_count++] = (pixel2[1] & 1) + '0';  
            extracted_bits[bit_count++] = (pixel2[2] & 1) + '0';  

            extracted_bits[bit_count++] = (pixel3[0] & 1) + '0'; 
            extracted_bits[bit_count++] = (pixel3[1] & 1) + '0';  
            
            int continuation_bit = pixel3[2] & 1;
            
            extracted_bits[9] = '\0';
            char extracted_char = 0;
            
            for(int i = 0 ; i < 8 ; i++){
            	extracted_char = (extracted_char << 1) | (extracted_bits[i] - '0');
            }
            
            printf("%c" , extracted_char );
            bit_count = 0;
            if(continuation_bit == 0){
            	return 0;
            }
            
        }
    }
    return 0;
}
