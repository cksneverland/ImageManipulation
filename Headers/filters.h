#ifndef FilterLib

#define FilterLib

#include "depends.h" 

typedef struct
{
    int r,g,b,a;
}filterBuffer;
    

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

typedef enum{
        diffusionPatternFloyd,
        diffusionPatternSimple2D,
        diffusionPatternJarvisJudiceNinke,
        diffusionPatternAtkinson,
        diffusionPatternStucki,
        diffusionPatternSierra,
        diffusionPatternError
    }diffusionPatterns;

typedef enum SCANTYPE
{
    scanBothWays= 0,
    scanRight = 1,
    scanLeft = -1,
    scanErr = 2,
}scanType;

    

void grayscaleFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void invertedColorFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale, filterBuffer **bluredImage);
void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width, filterBuffer ** edgeImage);
void errorDiffusionDithering(pixel** regularimage ,signedDWORD height, signedDWORD width, int diffusionType, scanType scanSetting, BYTE channelLevel, filterBuffer **buffer);
filterBuffer **createFilterBuffer(signedDWORD height, signedDWORD width);
void clearFilterBuffer(filterBuffer **buffer, signedDWORD height, signedDWORD width);
void freeFilterBuffer(filterBuffer ** buffer);

#endif