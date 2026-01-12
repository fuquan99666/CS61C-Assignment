// this file is to hide some word information into the least significant bit of the blue channel of an image
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

// 规定格式为./hide <input_image> <message>
int main(int argc, char **argv){
    if (argc != 3){
        printf("Usage: %s <input_image> <message>\n", argv[0]);
        return -1;
    }
    // 由于我们已经有了imageloader.c和imageloader.h，所以我们可以直接使用它们
    // 但是问题是这个只针对ppm格式的图片 ，所以需要先使用convert工具将png转换为ppm格式
    // convert -compress none input.png input.ppm
    Image *input_image = readData(argv[1]);

    if (input_image == NULL){
        printf("Error: could not read image %s\n", argv[1]);
        return -1;
    }
    Image *new_image = malloc(sizeof(Image));
    new_image->rows = input_image->rows;
    new_image->cols = input_image->cols;
    new_image->image = malloc(sizeof(Color*) * new_image->rows);
    for(int i=0;i<new_image->rows;i++){
        new_image->image[i] = malloc(sizeof(Color) * new_image->cols);
        for(int j=0;j<new_image->cols;j++){
            new_image->image[i][j] = input_image->image[i][j];
        }
    }
    // 这里我已经将message存储在了一张掩码照片中
    Image *message_image = readData(argv[2]);
    if (message_image == NULL){
        printf("Error: could not read image %s\n", argv[2]);
        freeImage(input_image);
        freeImage(new_image);
        return -1;
    }
    // start hiding information
    for (int i= 0; i< input_image->rows; i++){
        for(int j=0;j<input_image->cols;j++){

            Color* new_pixel = &new_image->image[i][j];
            // we only care about the blue channel's LSB
            if(i < message_image->rows && j < message_image->cols){
                Color message_pixel = message_image->image[i][j];
                if(message_pixel.B == 255){
                    new_pixel->B |= 1;
                }else{
                    new_pixel->B &= ~1;
                }
            }
            else{
                // for the area not in message, to 0
                new_pixel->B &= ~1;
            }
        }
    }
    // write new image to stdout
    // how to save it to a file? such a png?
    writeData(new_image);
    

    // free memory
    freeImage(input_image);
    freeImage(message_image);
    freeImage(new_image);
    return 0;
}