#include "bitmapfiles.h"
#include<stdio.h>
#include<stdlib.h>

const BYTE FourBitMaskArray[2] = {fourBitFirstPixelMask, fourBitSecondPixelMask};

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
