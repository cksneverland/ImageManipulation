#ifndef BitmapLib

#define BitmapLib
#include "depends.h"

// masks to isolate the pallete number
enum TwoBitByteMasksPixelOrder{
    numOf4BitPixelMasks    = 2,
    fourBitFirstPixelMask  = 0xF0,
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

enum Polarities{
    positive = 0,
    negative = 1
};

enum{bit32NuumberSignMask = 0b10000000000000000000000000000000};

enum bitmasksFor16BitPixels{
    red16BitMask   = 0b1111100000000000,
    green16BitMask = 0b0000011111100000,
    blue16BitMask  = 0b0000000000011111
};

enum bitmaskFor16BitPixelsShift{
    red16BitmaskShift   = 11,
    green16BitmaskShift = 5,
    blue16BitmaskShift = 3
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
pixel** form_Image(BYTE *loadedBytes, DWORD startingOffset, WORD bitsPerPixel, signedDWORD width, signedDWORD height, Pallete *pallete); // Switch between diffrent functopns
pixel** create_8BitPallete_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, pixel **image);
pixel** create_4BitPallete_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, pixel **image);
pixel** create_16Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image);
pixel** create_24Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image);
pixel** create_32Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image);
BYTE calculatePaddedBytes(DWORD width, DWORD bitsPerPixel);
BYTE get_Polarity(signedDWORD height);
DWORD revaluate_height(signedDWORD height);


#endif