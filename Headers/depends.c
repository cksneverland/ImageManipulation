#define _CRT_SECURE_NO_WARNINGS
#include "depends.h"
#include<stdio.h>
#include<stdlib.h>


loadedfile loadFile(char *fileName)
{ 
    loadedfile returnValue;

    FILE *input = fopen(fileName ,"rb");
    if(input == NULL)
    {
        perror("Opening file");
        returnValue.workingFile = false;
        return returnValue;

    }

    int validReadCheck = 0;
    BYTE buffer;
    long count = 0;
    
    // Cycles through all the bytes and counts them
    while(fread(&buffer, sizeof(BYTE), 1, input) == 1)
    {
        count ++;
    }

    if (ferror(input) != 0)
    {
        fclose(input);
        returnValue.workingFile = false;
        perror("Read error");
        return returnValue;
    }
    

    rewind(input);
    returnValue.byte_count = count;

    BYTE *fileBytes = malloc(sizeof(BYTE)* count);

    validReadCheck = fread(fileBytes, sizeof(BYTE) * count, 1, input);
    if(ferror(input))
    {
        fclose(input);
        returnValue.workingFile = false;
        free(fileBytes);
        perror("Read error");
        return returnValue;
    }

    returnValue.Bytes = fileBytes;
    returnValue.workingFile = true;

    fclose(input);
    return returnValue;
}

//Combines TO a max of 64 bit int
QWORD combine2HexVals(DWORD higherPlaceVal, DWORD lowerPlaceVal, BYTE lowerPlaceValSizeBytes)
{ 

    return (higherPlaceVal << (lowerPlaceValSizeBytes *8)) | lowerPlaceVal;

}

QWORD combineMultipleBytes(BYTE *bytes, BYTE amountofBytes, BYTE mode) //Combines up to 8 bytes
{
    QWORD total = 0x00;

    if(amountofBytes > qwordSizeInBytes)
    {
        return -1;
    }

    if(mode == littleEndianMode)
    {
        // Placing bytes infront of the old ones
        for(int i = 0; i < amountofBytes; i++ )
        {
            total = total | (bytes[i] << (8 * i));
        }
    }
    else if(mode == bigEndianMode)
    {
        for(int i = 0; i < amountofBytes; i++ )
        {
            total = (total << 8) | bytes[i];
        }
    }
    
    return total;

}

int pixelClamp(int color)
{
    if(color > 255)
    {
        return 255;
    }
    else if(color  < 0)
    {
        return 0;
    }

    return color;
}