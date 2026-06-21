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

#include <stdint.h>

static const BYTE bayerPattern16X16[16][16] =   {   //  16x16 Bayer Dithering Matrix.  Color levels: 256
                                            {     0, 191,  48, 239,  12, 203,  60, 251,   3, 194,  51, 242,  15, 206,  63, 254  }, 
                                            {   127,  64, 175, 112, 139,  76, 187, 124, 130,  67, 178, 115, 142,  79, 190, 127  },
                                            {    32, 223,  16, 207,  44, 235,  28, 219,  35, 226,  19, 210,  47, 238,  31, 222  },
                                            {   159,  96, 143,  80, 171, 108, 155,  92, 162,  99, 146,  83, 174, 111, 158,  95  },
                                            {     8, 199,  56, 247,   4, 195,  52, 243,  11, 202,  59, 250,   7, 198,  55, 246  },
                                            {   135,  72, 183, 120, 131,  68, 179, 116, 138,  75, 186, 123, 134,  71, 182, 119  },
                                            {    40, 231,  24, 215,  36, 227,  20, 211,  43, 234,  27, 218,  39, 230,  23, 214  },
                                            {   167, 104, 151,  88, 163, 100, 147,  84, 170, 107, 154,  91, 166, 103, 150,  87  },
                                            {     2, 193,  50, 241,  14, 205,  62, 253,   1, 192,  49, 240,  13, 204,  61, 252  },
                                            {   129,  66, 177, 114, 141,  78, 189, 126, 128,  65, 176, 113, 140,  77, 188, 125  },
                                            {    34, 225,  18, 209,  46, 237,  30, 221,  33, 224,  17, 208,  45, 236,  29, 220  },
                                            {   161,  98, 145,  82, 173, 110, 157,  94, 160,  97, 144,  81, 172, 109, 156,  93  },
                                            {    10, 201,  58, 249,   6, 197,  54, 245,   9, 200,  57, 248,   5, 196,  53, 244  },
                                            {   137,  74, 185, 122, 133,  70, 181, 118, 136,  73, 184, 121, 132,  69, 180, 117  },
                                            {    42, 233,  26, 217,  38, 229,  22, 213,  41, 232,  25, 216,  37, 228,  21, 212  },
                                            {   169, 106, 153,  90, 165, 102, 149,  86, 168, 105, 152,  89, 164, 101, 148,  85  }
};

static const BYTE bayerPattern8X8[8][8] = {
                                          {0  , 130, 32 , 162, 8  , 138, 40 , 170},
                                          {194, 65 , 227, 97 , 202, 73 , 235, 105},
                                          {49 , 178, 16 , 146, 57 , 186, 24 , 154},
                                          {243, 113, 210, 81 , 251, 121, 219, 89 },
                                          {12 , 142, 45 , 174, 4  , 134, 36 , 166},
                                          {206, 77 , 239, 109, 198, 69 , 231, 101},
                                          {61 , 190, 28 , 158, 53 , 182, 20 , 150},
                                          {255, 125, 223, 93 , 247, 117, 215, 85 }
};

static const BYTE bayerPattern4X4[4][4] = {
                                          {0  , 136, 34 , 170},
                                          {204, 68 , 238, 102},
                                          {51 , 187, 17 , 153},
                                          {255, 119, 221, 85 }
};

static const BYTE bayerPattern2X2[2][2] = {
                                          {0, 170},
                                          {255, 85}
};

void grayscaleFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void invertedColorFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale, filterBuffer **bluredImage);
void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width, filterBuffer ** edgeImage);
void errorDiffusionDithering(pixel** regularimage ,signedDWORD height, signedDWORD width, int diffusionType, scanType scanSetting, BYTE channelLevel, filterBuffer **buffer);
filterBuffer **createFilterBuffer(signedDWORD height, signedDWORD width);
void clearFilterBuffer(filterBuffer **buffer, signedDWORD height, signedDWORD width);
void freeFilterBuffer(filterBuffer ** buffer);
void orderedDithering(pixel** regularimage, signedDWORD height, signedDWORD width, int bayerScale, int level);

#endif