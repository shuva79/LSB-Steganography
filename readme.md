# LSB-Stego

## Overview
LSB-Stego is a steganography tool written in C that leverages the Least Significant Bit (LSB) technique to embed or extract hidden data within BMP image files. The program supports embedding secret messages or extracting hidden content from BMP images.

## Features
- **Embed Mode**: Hide a message within a BMP image.
- **Extract Mode**: Retrieve hidden messages from a BMP image.
- Output image is automatically saved as `output.bmp`.

## Usage
Run the program using the following syntax:
```
./lsb-stego
```

### Commands
```bash
Usage: ./lsb-stego -embed/-extract picture.bmp

Examples:
  ./lsb-stego -embed picture.bmp
  ./lsb-stego -extract picture.bmp
```

## Output
- The resulting image with embedded data is saved as `output.bmp`.
- If extracting data, the hidden message will be printed to the terminal.


## Compilation
The program can be compiled using either a Makefile or GCC:

### Using Makefile
Run the following command:
```
make
```

### Using GCC
Alternatively, you can compile the program manually:
```
gcc -o lsb-stego main.c
```

## Requirements
- A BMP image file to embed or extract data from.
- A C compiler (e.g., GCC).


---


