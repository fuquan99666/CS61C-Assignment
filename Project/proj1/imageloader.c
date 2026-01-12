/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	//YOUR CODE HERE
	FILE *fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Error: could not open file %s\n",filename);
		return NULL;
	}
	// start reading file
	char format[3]; // "P3" + "\0"
	fscanf(fp, "%2s", format);
	if (strcmp(format, "P3")!=0){
		printf("Error: file %s is not in P3 format\n", filename);
		fclose(fp);
		return NULL;
	}
	int width , height , colormax;
	fscanf(fp, "%d %d", &width, &height);
	fscanf(fp, "%d", &colormax);

	Image *image = malloc (sizeof(Image));
	image->cols = width;
	image->rows = height;
	image->image = (Color**)malloc(sizeof(Color*) * height);
	for(int i = 0; i < height; i++){
		image->image[i] = (Color*)malloc(sizeof(Color) * width);
		for(int j=0;j<width;j++){
			// read R G B values
			int r,g,b;
			fscanf(fp, "%d %d %d", &r, &g, &b);
			image->image[i][j].R = (uint8_t) r;
			image->image[i][j].G = (uint8_t) g;
			image->image[i][j].B = (uint8_t) b;
		}
	}
	fclose(fp);
	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	//YOUR CODE HERE
	printf("P3\n");
	printf("%d %d\n", image->cols, image->rows);
	printf("255\n");
	for(int i=0;i<image->rows;i++){
		for(int j=0;j<image->cols-1;j++){
			printf("%3d %3d %3d   ", image->image[i][j].R, image->image[i][j].G, image->image[i][j].B);
		}
		printf("%3d %3d %3d", image->image[i][image->cols-1].R, image->image[i][image->cols-1].G, image->image[i][image->cols-1].B);
		printf("\n");
	}

}
//Frees an image
void freeImage(Image *image)
{
	//YOUR CODE HERE
	for(int i = 0;i<image->rows;i++){
		free(image->image[i]);
	}
	free(image->image);
	free(image);
}