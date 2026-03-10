#define _CRT_SECURE_NO_WARNINGS

#include "depends.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BMPSIGNATURE 0x424d //BM
#define FULLHEADERSIZE 54

// masks to isolate the pallete number
enum TwoBitByteMasksPixelOrder{
    numOf4BitPixelMasks = 2,
    fourBitFirstPixelMask = 0xF0,
    fourBitSecondPixelMask = 0x0F
};
const BYTE FourBitMaskArray[2] = {fourBitFirstPixelMask, fourBitSecondPixelMask};

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


// Function
bool bitmapSignatureCheck(BYTE *loadedbytes);
QWORD getBmpAtribute(BYTE *loadedbytes, DWORD startingOffset, BYTE atributeSizeBytes, BYTE mode);
Pallete get_All_Pallete_Colors(BYTE *loadedBytes,DWORD bitsPerPixel);
pixel** form_Image(BYTE *loadedBytes, DWORD startingOffset, DWORD bitsPerPixel, signedDWORD width, signedDWORD height, Pallete *pallete); // Switch between diffrent functopns
pixel** create_4And8_BitPalletes(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, DWORD BitsPerPixel);

int main(int argc, char *argv[])
{
    
    if (argc < 2 || argc > 3)
    {
        printf("Invalid arguments\n Usage: ./imagedetec input.file-ext output.file-ext");
        return 1;
    }


    loadedfile file = loadFile(argv[1]);
    if (file.workingFile == false)
    {
        printf("ERROR WITH FILE \n");
        return 2;
    }


    if(bitmapSignatureCheck(file.Bytes) == true)
    {
        printf("Valid BMP file so far \n");
    }
    else
    {
        printf("Ts aint no bmp file\n");
        return 3;
    }

    bitmapfile imageData;

    imageData.ImageStartOffset = 0;

    imageData.ImageStartOffset = getBmpAtribute(file.Bytes, bmpPixelDataOffset, bmpPixelDataOffsetSize, littleEndianMode);
    imageData.Width = getBmpAtribute(file.Bytes, bmpWidthOffset, bmpWidthOffsetSize, littleEndianMode);
    imageData.Height = getBmpAtribute(file.Bytes, bmpHeightOffset, bmpHeightOffsetSize, littleEndianMode);
    imageData.BitsPerPixel = getBmpAtribute(file.Bytes, bmpBitsPerPixeltOffset, bmpBitsPerPixelOffsetSize, littleEndianMode);
    imageData.Compression = getBmpAtribute(file.Bytes, bmpCompressionOffset, bmpCompressionOffsetSize, littleEndianMode);
    imageData.infoHeaderSize = getBmpAtribute(file.Bytes, bmpInfoHeaderSizeOfset, bmpInfoHeaderSizeOfsetSize, littleEndianMode);

    // Debuging for now Should change latter
    printf("imageOffset: 0x%x\n", imageData.ImageStartOffset);
    printf("Width: %i\n", imageData.Width);
    printf("Height: %i\n", imageData.Height);
    printf("BitsPerPixel: %i\n", imageData.BitsPerPixel);
    printf("Compression: %i\n", imageData.BitsPerPixel);
    printf("Info header size: %i\n", imageData.BitsPerPixel);

  
    Pallete Colors = get_All_Pallete_Colors(file.Bytes, imageData.BitsPerPixel);

    if(Colors.vlaid == false)
    {
        printf("Invalid Pallete \n");
        return 2;
    }


    pixel **image = form_Image(file.Bytes, imageData.ImageStartOffset, imageData.BitsPerPixel, imageData.Width, imageData.Height, &Colors);

    if(image == NULL)
    {
        printf("Erororroroor \n");
        free(Colors.pixels);    
        free(file.Bytes);
    }
    

    printf("Image last pixel| R: %i G: %i B: %i \n", image[abs(imageData.Height) - 1][imageData.Width - 1].red, image[abs(imageData.Height) -1][imageData.Width - 1].green, image[abs(imageData.Height) -1][imageData.Width - 1].blue);
    printf("R: %i B: %i G: %i \n", Colors.pixels[9].red, Colors.pixels[9].green, Colors.pixels[9].blue);
    if(Colors.count > 18)
    {
            printf("R: %i B: %i G: %i \n", Colors.pixels[0xc3].red, Colors.pixels[0xc3].green, Colors.pixels[0xc3].blue);


    }

    
    for(int i = 0; i < imageData.Height; i++)
    {
        free(image[i]);
    }
    free(image);
    free(Colors.pixels);    
    free(file.Bytes);

}

bool bitmapSignatureCheck(BYTE *loadedbytes)
{
    WORD possibleSignature = combine2HexVals(loadedbytes[bmpSignatureStart],loadedbytes[bmpSignatureEnd], sizeof(loadedbytes[bmpSignatureEnd]));

    if(possibleSignature == BMPSIGNATURE)
    {
        return true;
    }
    else
    {
        return false;
    }

}

QWORD getBmpAtribute(BYTE *loadedbytes, DWORD startingOffset, BYTE atributeSizeBytes, BYTE mode)
{
    // Slpilts the starting point to the offest were looking for to properly loop over
    BYTE *imageOfsetBytes = &loadedbytes[startingOffset];

    QWORD total = combineMultipleBytes(imageOfsetBytes, atributeSizeBytes, mode);
    
    return total;
}

Pallete get_All_Pallete_Colors(BYTE *loadedBytes,DWORD bitsPerPixel)
{
    Pallete returnPallette;
    // The size of a header works to go past the place iin an array caus we start from 1 not 0 lmao futre me
    BYTE *palletebytes = &loadedBytes[FULLHEADERSIZE];
    returnPallette.count = 2; // 2 by default because you atleast have 1 bit
    //2 ot the Power of bits per pixel
    for(int i = 0; i < bitsPerPixel - 1; i++)
    {
        returnPallette.count *= 2;
    }
    DWORD Index = 0;

    
    returnPallette.pixels = malloc(sizeof(pixel) * returnPallette.count);
    if(returnPallette.pixels == NULL)
    {
        printf("Error alocating bytes to pallete");
        returnPallette.vlaid = false;
        return returnPallette;
    }
    returnPallette.vlaid = true;
    
    for(int i = 0; i < returnPallette.count; i++)
    {
        pixel tempPixel;

        tempPixel.blue = palletebytes[Index];
        tempPixel.green = palletebytes[Index + 1];
        tempPixel.red = palletebytes[Index + 2];
        tempPixel.alpha = palletebytes[Index + 3];
        Index += 4;

        returnPallette.pixels[i] = tempPixel;

    }
    return returnPallette;

}

// Intermidiary to all the other form image functions use this one only
pixel** form_Image(BYTE *loadedBytes, DWORD startingOffset, DWORD bitsPerPixel, signedDWORD width, signedDWORD height, Pallete *pallete)// Switch between diffrent functopns
{

    pixel **image = NULL;

    if(bitsPerPixel % 2 != 0 && bitsPerPixel != 1)
    {
        printf("What are you dooing \n");
        return image;
    }

    BYTE *allaignedBytes = &loadedBytes[startingOffset];


    switch (bitsPerPixel)
    {
    case 4:
        image = create_4And8_BitPalletes(allaignedBytes, width, height, pallete, bitsPerPixel);
        break;
    
    case 8:
        image = create_4And8_BitPalletes(allaignedBytes, width, height, pallete, bitsPerPixel);
    
    default:
        break;
    }
    
    
    // Seperate palletes ones from pixel w
    return image;

}

// bitGroupIndex is for a bit shift using bits per pixel
BYTE format_Pallete_Bytes(BYTE byte, BYTE bitMask, BYTE bitsPerPixel, BYTE maskIndex)
{
    BYTE formattedByte = byte & bitMask;
    formattedByte = formattedByte >> (maskIndex  * bitsPerPixel);
    return formattedByte;
}

// 4 and 8 bit pallets
pixel** create_4And8_BitPalletes(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, DWORD BitsPerPixel)
{
    DWORD bit32NuumberSignMask = 0b10000000000000000000000000000000;
    // check if image stored upsidedown
    BYTE polarity = (height & bit32NuumberSignMask) >> (dwordSizeInBits -1);
    printf("Polarity: %i \n", polarity);

    if(polarity > 1 || polarity < 0)
    {
        return NULL;
    }
    if (polarity == 1)
    {
        height = ~height + 1; // turn it positive
    }

    // Each row needs to be divisible by the size of dword in terms of bytes if not then it's padded
    DWORD totalWidthBytes;

    // Genuine witch craft
    totalWidthBytes = ((width * BitsPerPixel + 7) / 8); //Round up to full Byte
    DWORD pixelBytes = (totalWidthBytes + 3) & ~3;  // Alligns the byte by dword sizes
    
    BYTE paddedBytes = pixelBytes - totalWidthBytes;

    pixel **image = malloc(height * sizeof(pixel*));
    if(image == NULL)
    {
        return NULL;
    }

    DWORD loadedByteIndex = 0;
    BYTE BitMaksModeDefault = (byteSizeInBits / BitsPerPixel) - 1;
    BYTE bitMaskMode = BitMaksModeDefault;

    // Making the Image part
    for(int row = 0; row < height; row += 1)
    {   
        pixel *tempColumns = malloc(sizeof(pixel) * width);
        if (tempColumns == NULL)
        {
            printf("Error allocating bytes \n");
            free(tempColumns);
            for(int i = 0; i < row;i++)
            {
                free(image[row]);
            }
            free(image);
            return NULL;
        }
        
        for(int i = 0; i < width; i++)
        {
            BYTE currentByte = loadedBytes[loadedByteIndex];

            if(BitsPerPixel < byteSizeInBits) // 4 bit pallete
            {
                for(int j = 0; j < byteSizeInBits / BitsPerPixel; j++)
                {
                    BYTE palleteIndex;

                    // Change from four bit mask array later
                    if(j == 0)
                    {
                        palleteIndex = format_Pallete_Bytes(currentByte,FourBitMaskArray[j], BitsPerPixel, bitMaskMode);
                        tempColumns[i].red = pallete->pixels[palleteIndex].red;
                        tempColumns[i].green = pallete->pixels[palleteIndex].green;
                        tempColumns[i].blue = pallete->pixels[palleteIndex].blue;
                    }
                    else if(i + 1 < width)
                    {
                        i++;
                        palleteIndex = format_Pallete_Bytes(currentByte,FourBitMaskArray[j], BitsPerPixel, bitMaskMode);
                        tempColumns[i].red = pallete->pixels[palleteIndex].red;
                        tempColumns[i].green = pallete->pixels[palleteIndex].green;
                        tempColumns[i].blue = pallete->pixels[palleteIndex].blue;
                    }
                    else
                    {
                        break;
                    }
                    
                    bitMaskMode --;
                    
                }
            }
            
            else // 8 bit pallete
            {
                tempColumns[i].red = pallete->pixels[currentByte].red;
                tempColumns[i].green = pallete->pixels[currentByte].green;
                tempColumns[i].blue = pallete->pixels[currentByte].blue;
            }
           bitMaskMode = BitMaksModeDefault;

            loadedByteIndex ++ ;
            
        }
        

        if(polarity == 0)
        {
            image[row] = tempColumns;
        }
        else
        {
            image[(height - 1) - row] = tempColumns;
        }

        loadedByteIndex += paddedBytes;

    }

    return image;
}