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
    bmpFileSizeOffset = 0x02,
    bmpReservedOffset = 0x06,
    bmpPixelDataOffset = 0x0A,
    bmpHeaderSizeOffset = 0x0E,
    bmpWidthOffset = 0x12,
    bmpHeightOffset = 0x16,
    bmpPlanesOffset = 0x1A,
    bmpBitsPerPixeltOffset = 0x1c,
    bmpCompressionOffset = 0x1E,
    bmpImageSizeOffset = 0x22,
    bmpXPixelsPerMOffset = 0x26,
    bmpYPixelsPerMOffset = 0x2A,
    bmpColorsUsedOffset = 0x2E,
    bmpImportantCollorsOffset = 0x32,
    bmpColorTablesOffset = 0x36,
};

enum StaticBMPFileOffsetsByteSize
{
    bmpCompressionOffsetSize = 0x04,
    bmpFileSizeOffsetSize = 0x04,
    bmpHeaderSizeOffsetSize = 0x04,
    bmpReservedOffsetSize = 0x4,
    bmpPixelDataOffsetSize = 0x04,
    bmpWidthOffsetSize = 0x04,
    bmpHeightOffsetSize = 0x04,
    bmpImageSizeOffsetSize = 0x04,
    bmpXPixelsPerMOffsetSize = 0x04,
    bmpYPixelsPerMOffsetSize = 0x04,
    bmpColorsUsedMOffsetSize = 0x04,
    bmpImportantColorsMOffsetSize = 0x04,
    bmpSignatureSize = 0x02,
    bmpPlaneOffsetSize = 0x02,
    bmpBitsPerPixelOffsetSize = 0x02
    
};

enum Polarities{
    positive = 0,
    negative = 1
};

enum{bit32NuumberSignMask = 0b10000000000000000000000000000000};

enum bitmasksFor16BitPixels{
    red16BitMask   = 0b0111110000000000,
    green16BitMask = 0b0000001111100000,
    blue16BitMask  = 0b0000000000011111
};

enum bitmaskFor16BitPixelsShift{
    red16BitmaskShift   = 10,
    green16BitmaskShift = 4,
    blue16BitmaskShift = 3
};

typedef struct
{
    DWORD ImageStartOffset;
    DWORD FileSize;
    DWORD Reserved;
    DWORD HeaderSize;
    signedDWORD Width;
    signedDWORD Height;
    DWORD Planes;
    DWORD Compression;
    DWORD ImageSize;
    DWORD XPixelsPerM;
    DWORD YPixelsPerM;
    DWORD ColorsUsed;
    DWORD ImportantColors;
    WORD BitsPerPixel;
    BYTE paddingSize;
    pixel **ImagePixels;
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
void create_image_file(bitmapfile bitmap, char *filename);
bitmapfile get_Bitmapfile_Data(loadedfile file);

#endif