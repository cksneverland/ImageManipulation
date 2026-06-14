#ifndef FilterLib

#define FilterLib

#include "depends.h" 

typedef struct
{
    int r,g,b;
}errorDiffusion;
    

typedef struct
{
    int relativeRow;
    int relativeColumn;
    int weight;
    
}diffusionParams;
typedef struct
{
    diffusionParams *params;
    BYTE numOfParams;
    BYTE divisor;
}diffusionPattern;

enum DiffusionPatterns{
        floyd,
        simple2D,
        jarvisJudiceNinke,
        atkinson
    };

typedef enum SCANTYPE
{
    scanBothWays= 0,
    scanRight = 1,
    scanLeft = -1,
}scanType;

    

void grayscaleFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void invertedColorFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale);
void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width);
void errorDiffusionDithering(pixel** regularimage ,signedDWORD height, signedDWORD width, int diffusionType, scanType scanSetting);

#endif