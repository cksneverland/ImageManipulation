#include "filters.h"
#include "depends.h"
#include "bitmapfiles.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


filterBuffer **createFilterBuffer(signedDWORD height, signedDWORD width)
{
    filterBuffer **imageBuffer = malloc(sizeof(filterBuffer*) * abs(height));
    filterBuffer *imageBufferRows = malloc(sizeof(filterBuffer) * abs(height) * width);

    if(!imageBuffer || !imageBufferRows)
    {
        free(imageBuffer);
        free(imageBufferRows);
        return NULL;
    }

    for(int i = 0; i< abs(height); i++)
    {
        imageBuffer[i] = &imageBufferRows[i* width];
    }
    clearFilterBuffer(imageBuffer, abs(height), width);
    return imageBuffer;
}

void clearFilterBuffer(filterBuffer **buffer, signedDWORD height, signedDWORD width)
{
    memset(&buffer[0][0], 0, sizeof(filterBuffer) * height * width);
}

void freeFilterBuffer(filterBuffer ** buffer)
{
    free(&buffer[0][0]);
    free(buffer);

}

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

void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale, filterBuffer **bluredImage)
{
    clearFilterBuffer(bluredImage,height, width);

    if(blurScale < 2)
    {
        printf("Blur scale less than 2");
        return;
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
            bluredImage[mainRow][mainColumn].r = round(sumRed / foundPixels);
            bluredImage[mainRow][mainColumn].g = round(sumGreen / foundPixels);
            bluredImage[mainRow][mainColumn].b = round(sumBlue / foundPixels);        
            bluredImage[mainRow][mainColumn].a = 0;

            foundPixels = 0;
        }

    }

    

    for(int row = 0; row < abs(height); row++)
    {
        for(int column = 0; column < width; column++)
        {
            regularImage[row][column].red = bluredImage[row][column].r;
            regularImage[row][column].green = bluredImage[row][column].g;
            regularImage[row][column].blue = bluredImage[row][column].b;
            regularImage[row][column].alpha = bluredImage[row][column].a;
        }
    }
    

}


void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width, filterBuffer ** edgeImage)
{
    clearFilterBuffer(edgeImage,height, width);
    signedByte GXR[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    signedByte GYR[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

    

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
                edgeImage[mainRow][mainColumn].r = 255;
            else 
                edgeImage[mainRow][mainColumn].r = (BYTE)Gred;

            if(Ggreen > 255)
                edgeImage[mainRow][mainColumn].g = 255;
            else 
                edgeImage[mainRow][mainColumn].g = (BYTE)Ggreen;

            if(Gblue > 255)
                edgeImage[mainRow][mainColumn].b = 255;
            else 
                edgeImage[mainRow][mainColumn].b = (BYTE)Gblue;



        }
    }

    
    for(int row = 0; row < abs(height); row++)
    {
        for(int column = 0; column < width; column++)
        {
            regularImage[row][column].red = edgeImage[row][column].r;
            regularImage[row][column].green = edgeImage[row][column].g;
            regularImage[row][column].blue = edgeImage[row][column].b;
            regularImage[row][column].alpha = 0;
        }
    }

}

void errorDiffusionDithering(pixel** regularimage ,signedDWORD height, signedDWORD width, int diffusionType, scanType scanSetting, BYTE channelLevel, filterBuffer **buffer)
{
        
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
        {2,-2, 1},
        {2,-1, 3},
        {2, 0, 5},
        {2, 1, 3},
        {2, 2, 1}
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
    const BYTE numAtkinsonParams = 6;
    const BYTE atkinsonDivisor = 8;

    diffusionParams stuckiParams[] = {
        {0, 1, 8},
        {0, 2, 4},
        {1,-2, 2},
        {1,-1, 4},
        {1, 0, 8},
        {1, 1, 4},
        {1, 2, 2},
        {2,-2, 1},
        {2,-1, 2},
        {2, 0, 4},
        {2, 1, 2},
        {2, 2, 1}
    };
    const BYTE numStuckiParams = 12;
    const BYTE stuckiDivisor = 42;

    diffusionParams sierraParams[] = {
        {0, 1, 5},
        {0, 2, 3},
        {1,-2, 2},
        {1,-1, 4},
        {1, 0, 5},
        {1, 1, 4},
        {1, 2, 2},
        {2,-1, 2},
        {2, 0, 3},
        {2, 1, 2}
    };
    const BYTE numSierraParams = 10;
    const BYTE sierraDivisor = 32;

    const diffusionPattern floydSteinbergPattern ={floydSteinberParams, numFloydSteinbergParams, floydSteinbergDivisor};
    const diffusionPattern simple2DPattern ={simple2DParams, numSimple2DParams, simple2DDiviosr};
    const diffusionPattern jarvisJudiceNinkePattern ={jarvisJudiceNinkeParams, numJarvisJudiceNinkeParams, jarvisJudiceNinkeDivisor};
    const diffusionPattern atkinsonPattern ={atkinsonParams, numAtkinsonParams, atkinsonDivisor};
    const diffusionPattern stuckiPattern ={stuckiParams, numStuckiParams, stuckiDivisor};
    const diffusionPattern sierraPattern ={sierraParams, numSierraParams, sierraDivisor};

    diffusionPattern allDiffusionPaterns[] = {floydSteinbergPattern, simple2DPattern, jarvisJudiceNinkePattern, atkinsonPattern,
                                              stuckiPattern, sierraPattern};
    diffusionPattern *chosenPattern = &allDiffusionPaterns[diffusionType];
    diffusionParams *chosenParams = chosenPattern->params;
    int divisor = chosenPattern->divisor;


    
    height = abs(height);
    clearFilterBuffer(buffer,height, width);

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
        filterBuffer *currentBufferRow = buffer[row];
        
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
}


void orderedDithering(pixel** regularimage, signedDWORD height, signedDWORD width, int bayerScale, int level)
{

    float  step = 255 / (float)(level- 1);
    height = abs(height);

    const BYTE **bayerPattern = malloc(sizeof(BYTE*) * bayerScale);

    switch (bayerScale)
    {
    case 2:
        bayerPattern[0] = &bayerPattern2X2[0][0];
        bayerPattern[1] = &bayerPattern2X2[1][0];
        break;
    case 4:
        bayerPattern[0] = &bayerPattern4X4[0][0];
        bayerPattern[1] = &bayerPattern4X4[1][0];
        bayerPattern[2] = &bayerPattern4X4[2][0];
        bayerPattern[3] = &bayerPattern4X4[3][0];
        break;
    case 8:
        bayerPattern[0] = &bayerPattern8X8[0][0];
        bayerPattern[1] = &bayerPattern8X8[1][0];
        bayerPattern[2] = &bayerPattern8X8[2][0];
        bayerPattern[3] = &bayerPattern8X8[3][0];
        bayerPattern[4] = &bayerPattern8X8[4][0];
        bayerPattern[5] = &bayerPattern8X8[5][0];
        bayerPattern[6] = &bayerPattern8X8[6][0];
        bayerPattern[7] = &bayerPattern8X8[7][0];
        break;
    case 16:
        bayerPattern[0]  = &bayerPattern16X16[0][0];
        bayerPattern[1]  = &bayerPattern16X16[1][0];
        bayerPattern[2]  = &bayerPattern16X16[2][0];
        bayerPattern[3]  = &bayerPattern16X16[3][0];
        bayerPattern[4]  = &bayerPattern16X16[4][0];
        bayerPattern[5]  = &bayerPattern16X16[5][0];
        bayerPattern[6]  = &bayerPattern16X16[6][0];
        bayerPattern[7]  = &bayerPattern16X16[7][0];
        bayerPattern[8]  = &bayerPattern16X16[8][0];
        bayerPattern[9]  = &bayerPattern16X16[9][0];
        bayerPattern[10] = &bayerPattern16X16[10][0];
        bayerPattern[11] = &bayerPattern16X16[11][0];
        bayerPattern[12] = &bayerPattern16X16[12][0];
        bayerPattern[13] = &bayerPattern16X16[13][0];
        bayerPattern[14] = &bayerPattern16X16[14][0];
        bayerPattern[15] = &bayerPattern16X16[15][0];
        break;
    
    default:
        break;
    }

    for(int rows = 0; rows < height; rows++)
    {
        int bayerRowIndex = rows % bayerScale;
        pixel *rowPixels = regularimage[rows];
        for(int columns = 0; columns < width; columns++)
        {
            int bayerColumnIndex = columns % bayerScale;
            int threshold = bayerPattern[bayerRowIndex][bayerColumnIndex];
            
            int quantizedR = rowPixels[columns].red   - threshold;
            int quantizedG = rowPixels[columns].green - threshold;
            int quantizedB = rowPixels[columns].blue  - threshold;

            quantizedR = pixelClamp(quantizedR);
            quantizedG = pixelClamp(quantizedG);
            quantizedB = pixelClamp(quantizedB);

            quantizedR = (int)roundf(((float)(quantizedR * (level - 1) + 127) / 255.0f) * step);
            quantizedG = (int)roundf(((float)(quantizedG * (level - 1) + 127) / 255.0f) * step);
            quantizedB = (int)roundf(((float)(quantizedB * (level - 1) + 127) / 255.0f) * step);



            // quantizedR = (quantizedR < threshold) ? 0:255;
            // quantizedG = (quantizedG < threshold) ? 0:255;
            // quantizedB = (quantizedB < threshold) ? 0:255;

            

            rowPixels[columns].red = quantizedR;
            rowPixels[columns].green = quantizedG;
            rowPixels[columns].blue = quantizedB;

        }
    }

}