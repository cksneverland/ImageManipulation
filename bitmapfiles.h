#ifndef BitmapLib

#define BitmapLib
#include "depends.h"

// masks to isolate the pallete number
enum TwoBitByteMasksPixelOrder{
    numOf4BitPixelMasks = 2,
    fourBitFirstPixelMask = 0xF0,
    fourBitSecondPixelMask = 0x0F
};


enum StaticBMPFileOffsets
{
    bmpSignatureStart = 0x00,
    bmpSignatureEnd = 0x01,
    bmpPixelDataOffset = 0x0A,
    bmpWidthOffset = 0x12,
    bmpHeightOffset = 0x16,
    bmpBitsPerPixeltOffset = 0x1c,
    bmpCompressionOffset = 0x1E,
    bmpInfoHeaderSizeOfset = 0x0E

};

enum StaticBMPFileOffsetsByteSize
{
    bmpSignatureSize = 0x02,
    bmpPixelDataOffsetSize = 0x04,
    bmpWidthOffsetSize = 0x04,
    bmpHeightOffsetSize = 0x04,
    bmpBitsPerPixelOffsetSize = 0x02,
    bmpCompressionOffsetSize = 0x04,
    bmpInfoHeaderSizeOfsetSize = 0x04
};

typedef struct
{
    DWORD ImageStartOffset;
    signedDWORD Width;
    signedDWORD Height;
    DWORD Compression;
    WORD BitsPerPixel;
    BYTE paddingSize;
    DWORD infoHeaderSize;

}bitmapfile;

typedef struct 
{
    pixel *pixels;
    WORD count;
    bool vlaid;
}Pallete;

#define BMPSIGNATURE 0x424d //BM
#define FULLHEADERSIZE 54

bool bitmapSignatureCheck(BYTE *loadedbytes);
QWORD getBmpAtribute(BYTE *loadedbytes, DWORD startingOffset, BYTE atributeSizeBytes, BYTE mode);
Pallete get_All_Pallete_Colors(BYTE *loadedBytes,DWORD bitsPerPixel);
pixel** form_Image(BYTE *loadedBytes, DWORD startingOffset, DWORD bitsPerPixel, signedDWORD width, signedDWORD height, Pallete *pallete); // Switch between diffrent functopns
pixel** create_4And8_BitPalletes(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, DWORD BitsPerPixel);


#endif