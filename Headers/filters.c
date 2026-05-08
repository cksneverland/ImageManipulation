#include "filters.h"
#include "depends.h"
#include "bitmapfiles.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void grayscaleFilter(pixel** regularImage, signedDWORD height, signedDWORD width){

    BYTE pixelaverage = 0;
    for(int i = 0; i < height; i++)
    {
        for(int coloumn = 0; coloumn < width; coloumn++)
        {
            pixelaverage = (regularImage[i][coloumn].blue + regularImage[i][coloumn].red + regularImage[i][coloumn].green) / 3;

            regularImage[i][coloumn].blue  = pixelaverage;
            regularImage[i][coloumn].green = pixelaverage;
            regularImage[i][coloumn].red   = pixelaverage;
        }
    }


}

void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale)
{
    if(blurScale < 2)
    {
        printf("Blur scale less than 2");
        return;
    }

    pixel **bluredImage = malloc(sizeof(pixel *) * abs(height));
    for(int i =0; i < abs(height); i++)
    {
        pixel *tempColumns = malloc(sizeof(pixel) * width);
        bluredImage[i] = tempColumns;
        if (tempColumns == NULL)
        {
            printf("Error allocating bytes \n");
            free(tempColumns);
            for(int j = 0; j < i;j++)
            {
                free(bluredImage[j]);
            }
            return ;
        }
        
    }

    int steps = (int)blurScale/2;
    for(int mainRow = 0; mainRow < abs(height); mainRow++)
    {
       
        int startRow = mainRow - steps;

        for(int mainColumn = 0; mainColumn < width; mainColumn++)
        {
            int startColumn = mainColumn - steps;
            
           
            int sumRed = 0;
            int sumGreen = 0;
            int sumBlue = 0;

            int foundPixels = 0;
            
            // Getting neighbours
            for(int stepsRow = startRow; stepsRow <= (startRow + blurScale); stepsRow++)
            {
                for(int stepsColumn = startColumn; stepsColumn <= (startColumn + blurScale); stepsColumn++)
                {
                    if(stepsRow >= 0 && stepsRow < abs(height))
                    {
                        if(stepsColumn >= 0 && stepsColumn < width)
                        {
                            sumRed += regularImage[stepsRow][stepsColumn].red;
                            sumGreen += regularImage[stepsRow][stepsColumn].green;
                            sumBlue += regularImage[stepsRow][stepsColumn].blue;
                            foundPixels ++;
                        }
                    }
                    
                }
            }
            bluredImage[mainRow][mainColumn].red = round(sumRed / foundPixels);
            bluredImage[mainRow][mainColumn].green = round(sumGreen / foundPixels);
            bluredImage[mainRow][mainColumn].blue = round(sumBlue / foundPixels);        
            bluredImage[mainRow][mainColumn].alpha = 0;

            foundPixels = 0;
        }

    }

    

    for(int row = 0; row < abs(height); row++)
    {
        for(int column = 0; column < width; column++)
        {
            regularImage[row][column].red = bluredImage[row][column].red;
            regularImage[row][column].green = bluredImage[row][column].green;
            regularImage[row][column].blue = bluredImage[row][column].blue;
            regularImage[row][column].alpha = bluredImage[row][column].alpha;
        }
    }
    
    free_Image_Pixels(bluredImage, abs(height));
    

}


