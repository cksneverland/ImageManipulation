#include "filters.h"
#include "depends.h"
#include "bitmapfiles.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>



diffusionParams floydSteinberParams[] = {
    {0, 1, 7},
    {1,-1, 3},
    {1, 0, 5},
    {1, 1, 1}
};
const BYTE numFloydSteinbergParams = 4;
const BYTE floydSteinbergDivisor =16;
    

diffusionParams simple2DParams[] = {
    {0, 1, 1},
    {1, 0, 1}
};
const BYTE numSimple2DParams = 2;
const BYTE simple2DDiviosr = 2;
    

diffusionParams jarvisJudiceNinkeParams[] = {
    {0, 1, 7},
    {0, 2, 5},
    {1,-2, 3},
    {1,-1, 5},
    {1, 0, 7},
    {1, 1, 5},
    {1, 2, 3},
    {1,-2, 1},
    {1,-1, 3},
    {1, 0, 5},
    {1, 1, 3},
    {1, 2, 1}
};
const BYTE numJarvisJudiceNinkeParams = 12;
const BYTE jarvisJudiceNinkeDivisor = 48;

diffusionParams atkinsonParams[] = {
    {0, 1, 1},
    {0, 2, 1},
    {1,-1, 1},
    {1, 0, 1},
    {1, 2, 1},
    {2, 0, 1},
};
const BYTE numAtkinsonParams = 12;
const BYTE atkinsonDivisor = 8;


diffusionPattern floydSteinbergPattern ={floydSteinberParams, numFloydSteinbergParams, floydSteinbergDivisor};
diffusionPattern simple2DPattern ={simple2DParams, numSimple2DParams, simple2DDiviosr};
diffusionPattern jarvisJudiceNinkePattern ={jarvisJudiceNinkeParams, numJarvisJudiceNinkeParams, jarvisJudiceNinkeDivisor};
diffusionPattern atkinsonPattern ={atkinsonParams, numAtkinsonParams, atkinsonDivisor};
// Remember to add new patterns to the enum in the .h


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

void invertedColorFilter(pixel** regularImage, signedDWORD height, signedDWORD width){

    BYTE pixelaverage = 0;
    for(int i = 0; i < height; i++)
    {
        for(int coloumn = 0; coloumn < width; coloumn++)
        {

            regularImage[i][coloumn].blue  = 255 - regularImage[i][coloumn].blue;
            regularImage[i][coloumn].green = 255 - regularImage[i][coloumn].green;
            regularImage[i][coloumn].red   = 255 - regularImage[i][coloumn].red;
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


void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width)
{
    signedByte GXR[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    signedByte GYR[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

    pixel **edgeImage = malloc(sizeof(pixel *) * abs(height));
    for(int i =0; i < abs(height); i++)
    {
        pixel *tempColumns = malloc(sizeof(pixel) * width);
        edgeImage[i] = tempColumns;
        if (tempColumns == NULL)
        {
            printf("Error allocating bytes \n");
            free(tempColumns);
            for(int j = 0; j < i;j++)
            {
                free(edgeImage[j]);
            }
            return ;
        }
        
    }

    BYTE steps = 1; //1 becasue from the center of a 3x3 a "step" is the upper left corner one left and up 

    for(int mainRow = 0; mainRow < abs(height); mainRow++)
    {
        int startRow = mainRow - steps;
        for(int mainColumn = 0; mainColumn < width; mainColumn++)
        {
            int startColumn = mainColumn - steps;

            int GXRred = 0;
            int GXRgreen = 0;
            int GXRblue = 0;
            int GYRred = 0;
            int GYRgreen = 0;
            int GYRblue = 0;


            int GRow = 0;
            int GColumn = 0;
            for(int sectionRow = startRow; sectionRow < (startRow + 3); sectionRow++)
            {
                for(int sectionColumn = startColumn; sectionColumn < (startColumn + 3); sectionColumn++)
                {
                    if(sectionRow  >= 0 && sectionRow < abs(height))
                    {
                        if(sectionColumn >= 0 && sectionColumn < width)
                        {
                            GXRred   += regularImage[sectionRow][sectionColumn].red    * GXR[GRow][GColumn];
                            GXRgreen += regularImage[sectionRow][sectionColumn].green  * GXR[GRow][GColumn];
                            GXRblue  += regularImage[sectionRow][sectionColumn].blue   * GXR[GRow][GColumn];

                            GYRred   += regularImage[sectionRow][sectionColumn].red    * GYR[GRow][GColumn];
                            GYRgreen += regularImage[sectionRow][sectionColumn].green  * GYR[GRow][GColumn];
                            GYRblue  += regularImage[sectionRow][sectionColumn].blue   * GYR[GRow][GColumn];
                        }
                        
                    }
                    

                    GColumn ++;

                }
                GRow ++;
                GColumn = 0;
            }
            
            // Appproxamation first rn
            int Gred   = round(sqrt((GXRred * GXRred) + (GYRred * GYRred)));
            int Ggreen = round(sqrt((GXRgreen * GXRgreen) + (GYRgreen * GYRgreen)));
            int Gblue  = round(sqrt((GXRblue * GXRblue) + (GYRblue * GYRblue)));

            if(Gred > 255)
                edgeImage[mainRow][mainColumn].red = 255;
            else 
                edgeImage[mainRow][mainColumn].red = (BYTE)Gred;

            if(Ggreen > 255)
                edgeImage[mainRow][mainColumn].green = 255;
            else 
                edgeImage[mainRow][mainColumn].green = (BYTE)Ggreen;

            if(Gblue > 255)
                edgeImage[mainRow][mainColumn].blue = 255;
            else 
                edgeImage[mainRow][mainColumn].blue = (BYTE)Gblue;



        }
    }

    
    for(int row = 0; row < abs(height); row++)
    {
        for(int column = 0; column < width; column++)
        {
            regularImage[row][column].red = edgeImage[row][column].red;
            regularImage[row][column].green = edgeImage[row][column].green;
            regularImage[row][column].blue = edgeImage[row][column].blue;
            regularImage[row][column].alpha = 0;
        }
    }
    
    free_Image_Pixels(edgeImage, abs(height));

}

void errorDiffusionDithering(pixel** regularimage ,signedDWORD height, signedDWORD width, int diffusionType, scanType scanSetting)
{
    diffusionPattern allDiffusionPaterns[] = {floydSteinbergPattern, simple2DPattern, jarvisJudiceNinkePattern, atkinsonPattern};
    diffusionPattern *chosenPattern = &allDiffusionPaterns[diffusionType];
    diffusionParams *chosenParams = chosenPattern->params;
    int divisor = chosenPattern->divisor;


    
    height = abs(height);
    errorDiffusion **buffer = NULL;
    errorDiffusion *bufferRows = NULL;

    bufferRows = malloc(sizeof(errorDiffusion) * width * height);
    buffer = malloc(sizeof(errorDiffusion*) * height);
    
    if(!bufferRows || !buffer)
    {
        free(buffer);
        free(bufferRows);
        printf("Buffer Rows or Buffer had an error \n");
        return; // really should do sum idk
    }
    memset(bufferRows, 0, sizeof(errorDiffusion) * width * height);
    for(int i =0; i < height; i++)
    {
        buffer[i] = &bufferRows[i * width];
    }


    BYTE channelLevel = 3;
    float step = 255 / (float)(channelLevel- 1);

    BYTE quantizeLookup[256];
    int quantizeErrorLookup[256];
    for(int i = 0; i < 256; i++)
    {
        int index = (i * (channelLevel - 1) + 127) / 255; // rounding be adding .5 technically just learend ts
        quantizeLookup[i] = roundf(index * step);
    }
    for(int i = 0; i < 256; i++)
    {
        quantizeErrorLookup[i] = i - quantizeLookup[i];
    }

    scanType direction = scanSetting;
    for(int row = 0; row < height; row++)
    {
        pixel *currentImageRow = regularimage[row];
        errorDiffusion *currentBufferRow = buffer[row];
        
        if(scanSetting == scanBothWays)
        {
            direction = (row % 2 == 1) ? -1:1;
        }
            

        int start = (direction == 1) ? 0 : width - 1;
        int end   = (direction == 1) ? width : -1;

        for(int column = start; column != end; column += direction)
        {
            int oldPixelr = currentImageRow[column].red   + ((currentBufferRow[column].r + (divisor >> 1))/ divisor); // >> 1 = /2 incase i forget
            int oldPixelg = currentImageRow[column].green + ((currentBufferRow[column].g + (divisor >> 1))/ divisor);
            int oldPixelb = currentImageRow[column].blue  + ((currentBufferRow[column].b + (divisor >> 1))/ divisor);

            // Not over 255 not under 0
            oldPixelr = pixelClamp(oldPixelr);
            oldPixelg = pixelClamp(oldPixelg);
            oldPixelb = pixelClamp(oldPixelb);

            int errorR = quantizeErrorLookup[oldPixelr];
            int errorG = quantizeErrorLookup[oldPixelg];
            int errorB = quantizeErrorLookup[oldPixelb];

            currentImageRow[column].red   = quantizeLookup[oldPixelr];
            currentImageRow[column].green = quantizeLookup[oldPixelg];
            currentImageRow[column].blue  = quantizeLookup[oldPixelb];



            for(int i = 0; i < chosenPattern->numOfParams; i++)
            {
                int relRow = row  + chosenParams[i].relativeRow;
                int relColumn = column + chosenParams[i].relativeColumn * direction;

                if(relRow >= 0 && relRow < height)
                {
                    if(relColumn >= 0 && relColumn < width)
                    {
                        buffer[relRow][relColumn].r += errorR * chosenParams[i].weight;
                        buffer[relRow][relColumn].g += errorG * chosenParams[i].weight;
                        buffer[relRow][relColumn].b += errorB * chosenParams[i].weight;

                    }

                }

            }


        }
    }

    free(buffer);
    free(bufferRows);
}