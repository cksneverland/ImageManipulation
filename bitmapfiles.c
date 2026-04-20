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
pixel** form_Image(BYTE *loadedBytes, DWORD startingOffset, WORD bitsPerPixel, signedDWORD width, signedDWORD height, Pallete *pallete)// Switch between diffrent functopns
{

    pixel **image = malloc(height * sizeof(pixel*));
    if(image == NULL)
    {
        return NULL;
    }

    if(bitsPerPixel % 2 != 0 && bitsPerPixel != 1)
    {
        printf("What are you dooing \n");
        return image;
    }

    BYTE *allaignedBytes = &loadedBytes[startingOffset];


    switch (bitsPerPixel)
    {
    case 4:
        image = create_4BitPallete_Image(allaignedBytes, width, height, pallete, image);
        break;
    case 8:
        image = create_8BitPallete_Image(allaignedBytes, width, height, pallete, image);
        break;
    case 16:
        image = create_16Bit_Image(allaignedBytes, width, height, image);
        break;
    case 24:
        image = create_24Bit_Image(allaignedBytes, width,  height, image);
        break;
    case 32:
        image = create_32Bit_Image(allaignedBytes, width,  height, image);
        break;
    default:
        break;
    }
    
    
    // Seperate palletes ones from pixel w
    return image;

}

// bitGroupIndex is for a bit shift using bits per pixel
BYTE get_Pallete_Index(BYTE byte, BYTE bitMask, BYTE bitsPerPixel, BYTE maskIndex)
{
    BYTE formattedByte = byte & bitMask;
    formattedByte = formattedByte >> (maskIndex  * bitsPerPixel);
    return formattedByte;
}

pixel** create_4BitPallete_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, pixel **image)
{
    // check if image stored upsidedown
    BYTE polarity = get_Polarity(height);

    printf("Polarity: %i \n", polarity);

    if((height = revaluate_height(height)) == 0)
    {
        return NULL;
    }

    BYTE paddedBytes = calculatePaddedBytes(abs(width), halfByteSizeInBits);

   

    DWORD loadedByteIndex = 0;
    BYTE BitMaksModeDefault = 1; // Upper part of the byte
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
        
        for(int column = 0; column < width; column++)
        {
            BYTE currentByte = loadedBytes[loadedByteIndex];
                for(int j = 0; j < byteSizeInBits / halfByteSizeInBits; j++)
                {
                    BYTE palleteIndex;

                    // Change from four bit mask array later
                    if(j == 0)
                    {
                        palleteIndex = get_Pallete_Index(currentByte,FourBitMaskArray[j], halfByteSizeInBits, bitMaskMode);
                        tempColumns[column].red = pallete->pixels[palleteIndex].red;
                        tempColumns[column].green = pallete->pixels[palleteIndex].green;
                        tempColumns[column].blue = pallete->pixels[palleteIndex].blue;
                    }
                    else if(column + 1 < width)
                    {
                        column++;
                        palleteIndex = get_Pallete_Index(currentByte,FourBitMaskArray[j], halfByteSizeInBits, bitMaskMode);
                        tempColumns[column].red = pallete->pixels[palleteIndex].red;
                        tempColumns[column].green = pallete->pixels[palleteIndex].green;
                        tempColumns[column].blue = pallete->pixels[palleteIndex].blue;
                    }
                    else
                    {
                        break;
                    }
                    
                    bitMaskMode --; // Switch the mode
                    
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

pixel** create_8BitPallete_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, Pallete *pallete, pixel **image)
{
    // check if image stored upsidedown
    BYTE polarity = get_Polarity(height);

    printf("Polarity: %i \n", polarity);

    if((height = revaluate_height(height)) == 0)
    {
        return NULL;
    }

    BYTE paddedBytes = calculatePaddedBytes(abs(width), byteSizeInBits);

    DWORD loadedByteIndex = 0;

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
        
        for(int column = 0; column < width; column++)
        {
            BYTE currentByte = loadedBytes[loadedByteIndex];

            tempColumns[column].red = pallete->pixels[currentByte].red;
            tempColumns[column].green = pallete->pixels[currentByte].green;
            tempColumns[column].blue = pallete->pixels[currentByte].blue;
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

pixel** create_16Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image)
{
    // check if image stored upsidedown
    BYTE polarity = get_Polarity(height);
    printf("Polarity: %i \n", polarity);

    if((height = revaluate_height(height)) == 0)
    {
        return NULL;
    }

    BYTE paddedBytes = calculatePaddedBytes(abs(width), wordSizeInBits);

    DWORD currentIndex = 0;
    // making the image part
    for(int row = 0; row < height; row++)
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
        for(int column = 0; column < width; column++)
        {
            // comine two seqential Bytes
            WORD rawPixel = combine2HexVals(loadedBytes[currentIndex + 1], loadedBytes[currentIndex], byteSizeInBytes); // Rember this is little endian so lowest place value is colected first lmao

            // "Zeros" the values after bitmask applied
            tempColumns[column].alpha = 0;
            tempColumns[column].red = ((rawPixel & red16BitMask) >> red16BitmaskShift) * 255 / 31; // * 255 / 31 is to morph it into a byte val
            tempColumns[column].green = ((rawPixel & green16BitMask) >> green16BitmaskShift) * 255 / 63;  // * 255 / 63 is to morph it into a byte val 
            tempColumns[column].blue = ((rawPixel & blue16BitMask)) * 255 / 31;  // * 255 / 31 is to morph it into a byte val
            currentIndex += 2;

        }

         if(polarity == 0)
        {
            image[row] = tempColumns;
        }
        else
        {
            image[(height - 1) - row] = tempColumns;
        }
        currentIndex += paddedBytes;

    }
    printf("paddedBytes: %i\n", paddedBytes);
    return image;
}

pixel** create_24Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image)
{
     BYTE polarity = get_Polarity(height);

    printf("Polarity: %i \n", polarity);

    if((height = revaluate_height(height)) == 0)
    {
        return NULL;
    }

    BYTE paddedBytes = calculatePaddedBytes(abs(width), 24); // 24 fro 24 bits lmao


    DWORD currentIndex = 0;
    // making the image part
    for(int row = 0; row < height; row++)
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
        for(int column = 0; column < width; column++)
        {
            // comine two seqential Bytes

            // "Zeros" the values after bitmask applied
            tempColumns[column].alpha = 0;
            tempColumns[column].blue = loadedBytes[currentIndex];
            tempColumns[column].green = loadedBytes[currentIndex + 1];
            tempColumns[column].red =  loadedBytes[currentIndex + 2];

            // printf("CurentRIndex: %i \n", currentIndex);
            // printf("Debug: R: %x   G: %x,   B: %x \n",tempColumns[column].red, tempColumns[column].green, tempColumns[column].blue);
            currentIndex += 3;

        }


        if(polarity == 0)
        {
            image[row] = tempColumns;
        }
        else
        {
            image[(height - 1) - row] = tempColumns;
        }
        currentIndex += paddedBytes;

    }
    printf("paddedBytes: %i\n", paddedBytes);
    return image;
}

pixel** create_32Bit_Image(BYTE *loadedBytes, signedDWORD width, signedDWORD height, pixel **image)
{
     BYTE polarity = get_Polarity(height);

    printf("Polarity: %i \n", polarity);

    if((height = revaluate_height(height)) == 0)
    {
        return NULL;
    }

    BYTE paddedBytes = calculatePaddedBytes(abs(width), dwordSizeInBits); 


    DWORD currentIndex = 0;
    // making the image part
    for(int row = 0; row < height; row++)
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
        for(int column = 0; column < width; column++)
        {
            // comine two seqential Bytes

            // "Zeros" the values after bitmask applied
            
            tempColumns[column].blue = loadedBytes[currentIndex];
            tempColumns[column].green = loadedBytes[currentIndex + 1];
            tempColumns[column].red =  loadedBytes[currentIndex + 2];
            tempColumns[column].alpha = loadedBytes[currentIndex + 3];

            // printf("CurentRIndex: %i \n", currentIndex);
            // printf("Debug: R: %x   G: %x,   B: %x \n",tempColumns[column].red, tempColumns[column].green, tempColumns[column].blue);
            currentIndex += 4;

        }


        if(polarity == 0)
        {
            image[row] = tempColumns;
        }
        else
        {
            image[(height - 1) - row] = tempColumns;
        }
        currentIndex += paddedBytes;

    }
    printf("paddedBytes: %i\n", paddedBytes);
    return image;
}

void create_image_file(bitmapfile bitmap, char *filename)
{
    FILE *imageFile = fopen(filename, "wb");
    if (imageFile == NULL)
    {
        return;
    }

    // Modify some values to corespond to what the bitmap will be
    bitmap.ImageSize = (bitmap.Height * bitmap.Width) * dwordSizeInBytes;
    bitmap.FileSize = bitmap.ImageSize +  54;// size of full file header
    bitmap.ImageStartOffset = 54; // size 54 but like we count from 0
    bitmap.BitsPerPixel = 32;
    BYTE padding = calculatePaddedBytes(bitmap.Width, dwordSizeInBits);
    printf("Padding: %i \n", padding);
    int infoHeaderSize = 40;
    const BYTE zero = 0;

    fwrite("BM", 2, 1, imageFile);
    printf("NewFileSize: %i\n", bitmap.FileSize);
    fwrite(&bitmap.FileSize, 4, 1, imageFile);
    fwrite(&bitmap.Reserved, 4, 1, imageFile);
    fwrite(&bitmap.ImageStartOffset, 4, 1, imageFile);
    fwrite(&infoHeaderSize, 4, 1, imageFile);// bit map info header size always 40 change later when i'm bored
    fwrite(&bitmap.Width, 4, 1, imageFile);
    fwrite(&bitmap.Height, 4, 1, imageFile);
    fwrite(&bitmap.Planes, 2, 1, imageFile);
    fwrite(&bitmap.BitsPerPixel, 2, 1, imageFile);
    fwrite(&bitmap.Compression, 4, 1, imageFile);
    fwrite(&bitmap.ImageSize, 4, 1, imageFile);
    fwrite(&bitmap.XPixelsPerM, 4, 1, imageFile);
    fwrite(&bitmap.YPixelsPerM, 4, 1, imageFile);
    fwrite(&bitmap.ColorsUsed, 4, 1, imageFile);
    fwrite(&bitmap.ImportantColors, 4, 1, imageFile);

    for (int i = 0; i < bitmap.Height; i++){
        for(int k = 0; k < bitmap.Width; k++){
            fwrite(&bitmap.ImagePixels[i][k], sizeof(bitmap.ImagePixels[i][k]), 1, imageFile);
        }

        if (padding > 0){
            for(int j = 0; j < padding; j++){
                fwrite(&zero, 1, 1, imageFile);
            }
        }
    }




    fclose(imageFile);
}

BYTE calculatePaddedBytes(DWORD width, DWORD bitsPerPixel)
{
     // Each row needs to be divisible by the size of dword in terms of bytes if not then it's padded
    DWORD totalWidthBytes;

    // Genuine witch craft
    totalWidthBytes = ((width * bitsPerPixel + 7) / 8); //Round up to full Byte
    DWORD pixelBytes = (totalWidthBytes + 3) & ~3;  // Alligns the byte by dword sizes
    
    return pixelBytes - totalWidthBytes; 

}

BYTE get_Polarity(signedDWORD height)
{
    return (height & bit32NuumberSignMask) >> (dwordSizeInBits -1);
}

DWORD revaluate_height(signedDWORD height)
{
    BYTE polarity = 0;

    if(polarity != positive && polarity != negative)
    {
        return 0;
    }
    else if (polarity == negative)
    {
        return height = ~height + 1; // turn it positive
    }

    return height;

}