// Check to see if its already been added
#ifndef DETECTORLIB

#define DETECTORLIB



#include <stdint.h>
#include<stdbool.h>

enum hexBuilderModes{
    littleEndianMode = 0,
    bigEndianMode = 1
};
enum hexSizesinbits{
    halfByteSizeInBits = 4,
    byteSizeInBits = 8,
    wordSizeInBits = 16,
    dwordSizeInBits = 32,
    qwordSizeInBits = 64
};
enum hexSizesinbytes{
    byteSizeInBytes = 1,
    wordSizeInBytes = 2,
    dwordSizeInBytes = 4,
    qwordSizeInBytes = 8
};

typedef uint8_t BYTE;
typedef int8_t signedByte;
typedef uint16_t WORD;
typedef int16_t signedWORD;
typedef uint32_t DWORD;
typedef int32_t signedDWORD;
typedef uint64_t QWORD;

typedef struct
{
    BYTE red;
    BYTE green;
    BYTE blue;
    BYTE alpha;
}pixel;

typedef struct LoadedFiles
{
    unsigned long byte_count;
    uint8_t *Bytes; // list lmao
    bool workingFile;

}loadedfile;


loadedfile loadFile(char *fileName);
QWORD combine2HexVals(DWORD higherPlaceVal, DWORD lowerPlaceVal, BYTE lowerPlaceValSizeBytes);
QWORD combineMultipleBytes(BYTE *bytes, BYTE amountofBytes, BYTE mode); //Combines up to 8 bytes
BYTE pixelClamp(int pixelVal);

#endif
