#ifndef FilterLib

#define FilterLib

#include "depends.h" 

void grayscaleFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void invertedColorFilter(pixel** regularImage, signedDWORD height, signedDWORD width);
void boxBlurFilter(pixel** regularImage, signedDWORD height, signedDWORD width, WORD blurScale);
void sobelEdgeDetection(pixel** regularImage, signedDWORD height, signedDWORD width);

#endif