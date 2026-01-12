/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	//YOUR CODE HERE
	// note that we treat the top row as adjacent to the bottom row
	// and the left colum as adjacent to the right colum

	Color Cell = image->image[row][col];
	Color* newColor = malloc(sizeof(Color));
	newColor->R = Cell.R;
	newColor->G = Cell.G;
	newColor->B = Cell.B;
	int rows = image->rows;
	int cols = image->cols;
	// now we think about all 24 bits how to shift next cycle
	for(int c=0;c<24;c++){
		// find 8 neighbors
		int LiveNeighbors = 0;
		for(int i = -1; i <= 1; i++){
			for(int j = -1; j <= 1; j++){
				if (i == 0 && j == 0){
					continue;
				}
				int neighbor_row = (row + i + rows) % rows;
				int neighbor_col = (col + j + cols) % cols;
				Color neighbor = image->image[neighbor_row][neighbor_col];
				// find the color of c bit
				// 8 red 8 blue 8 green
				if ( (c<8 && (neighbor.G >> c) & 1 ) || (c>=8 && c<16 && (neighbor.B >> (c-8)) & 1) || (c>=16 && (neighbor.R >> (c-16)) & 1)){
					LiveNeighbors++;
				}
			}
		}
		int current_bit;
		if (c<8){
			current_bit = (Cell.G >> c) & 1;
		}else if(c>=8 && c<16){
			current_bit = (Cell.B >> (c-8)) & 1;
		}else{
			current_bit = (Cell.R >> (c-16)) & 1;
		}
		if (current_bit == 1){
			// alive
			if ((rule >> 9 >> LiveNeighbors) & 1){
				continue;
			}else{
				if(c<8){
					newColor->G &= ~(1<<c);
				}else if(c>=8 && c<16){
					newColor->B &= ~(1<<(c-8));
				}else{
					newColor->R &= ~(1<<(c-16));
				}
			}
		}
		else{
			// dead
			if ((rule >> LiveNeighbors) & 1){
				if(c<8){
					newColor->G |= (1<<c);
				}else if(c>=8 && c<16){
					newColor->B |= (1<<(c-8));
				}else{
					newColor->R |= (1<<(c-16));
				}
			}
		}
	}
	return newColor;

}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
	//YOUR CODE HERE
	Image * newImage = malloc(sizeof(Image));
	newImage->cols = image->cols;
	newImage->rows = image->rows;
	newImage->image = malloc(sizeof(Color*) * image->rows);
	for (int i=0;i<image->rows;i++){
		newImage->image[i] = malloc(sizeof(Color)*image->cols);
		for(int j=0;j<image->cols;j++){
			Color* new_color = evaluateOneCell(image,i,j,rule);
			newImage->image[i][j] = *new_color;
			free(new_color);
		}
	}
	return newImage;

}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	//YOUR CODE HERE
	if(argc != 3){
		printf("Usage: %s <file> <rule>\n",argv[0]);
		return 0;
	}
	Image* input = readData(argv[1]);
	uint32_t rule = strtol(argv[2],NULL,16);
	Image*new_image = life(input,rule);
	writeData(new_image);
	freeImage(input);
	freeImage(new_image);
	return 0;
}
