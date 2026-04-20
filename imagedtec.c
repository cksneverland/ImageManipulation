#define _CRT_SECURE_NO_WARNINGS

#include "depends.h"
#include"bitmapfiles.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    bool sucsess;
    bool dubug;
    char *inputfile;
    char *outputfile;
}args;
args argParser(char *argv[], int argc);

// Function

int main(int argc, char *argv[])
{
    
    if (argc < 2)
    {
        printf("Invalid arguments\n Usage: ./imagedetec input output \n");
        printf("Flags:\n   -o: Output\n   -i: Input\n   -d; Debug\n");
        return 1;
    }

    args parsedargs = argParser(argv, argc);

    if (parsedargs.sucsess == false)
    {
        printf("Invalid arguments\n Usage: ./imagedetec input output \n");
        printf("Flags:\n   -o: Output\n   -i: Input\n   -d; Debug\n");
        return 2;
    }

    loadedfile file = loadFile(parsedargs.inputfile);
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

    imageData.ImageStartOffset = getBmpAtribute(file.Bytes, bmpPixelDataOffset, bmpPixelDataOffsetSize, littleEndianMode);
    imageData.FileSize = getBmpAtribute(file.Bytes, bmpFileSizeOffset, bmpFileSizeOffsetSize, littleEndianMode);
    imageData.Reserved = getBmpAtribute(file.Bytes, bmpReservedOffset, bmpReservedOffsetSize, littleEndianMode);
    imageData.HeaderSize = getBmpAtribute(file.Bytes, bmpHeaderSizeOffset, bmpHeaderSizeOffsetSize, littleEndianMode);
    imageData.Planes = getBmpAtribute(file.Bytes, bmpPlanesOffset, bmpPlaneOffsetSize, littleEndianMode);
    imageData.ImageSize = getBmpAtribute(file.Bytes, bmpImageSizeOffset, bmpImageSizeOffsetSize, littleEndianMode);
    imageData.XPixelsPerM = getBmpAtribute(file.Bytes, bmpXPixelsPerMOffset, bmpXPixelsPerMOffsetSize, littleEndianMode);
    imageData.YPixelsPerM = getBmpAtribute(file.Bytes, bmpYPixelsPerMOffset, bmpYPixelsPerMOffsetSize, littleEndianMode);
    imageData.ColorsUsed = getBmpAtribute(file.Bytes, bmpColorsUsedOffset, bmpColorsUsedMOffsetSize, littleEndianMode);
    imageData.ImportantColors = getBmpAtribute(file.Bytes, bmpImportantCollorsOffset, bmpImportantColorsMOffsetSize, littleEndianMode);
    imageData.Width = getBmpAtribute(file.Bytes, bmpWidthOffset, bmpWidthOffsetSize, littleEndianMode);
    imageData.Height = getBmpAtribute(file.Bytes, bmpHeightOffset, bmpHeightOffsetSize, littleEndianMode);
    imageData.BitsPerPixel = getBmpAtribute(file.Bytes, bmpBitsPerPixeltOffset, bmpBitsPerPixelOffsetSize, littleEndianMode);
    imageData.Compression = getBmpAtribute(file.Bytes, bmpCompressionOffset, bmpCompressionOffsetSize, littleEndianMode);

    // Debuging for now Should change latter
    printf("imageOffset: 0x%x\n", imageData.ImageStartOffset);
    printf("Width: %i\n", imageData.Width);
    printf("Height: %i\n", imageData.Height);
    printf("BitsPerPixel: %i\n", imageData.BitsPerPixel);
    printf("Compression: %i\n", imageData.Compression);
    printf("FileSize: %i\n", imageData.FileSize);
    printf("Reserved: %i\n", imageData.Reserved);
    printf("HeaderSize: %i\n", imageData.HeaderSize);
    printf("ImageSize: %i\n", imageData.ImageSize);
    printf("XPixelsPerM: %i\n", imageData.XPixelsPerM);
    printf("YPixelsPerM: %i\n", imageData.YPixelsPerM);
    printf("ColorsUsed: %i\n", imageData.ColorsUsed);
    printf("ImportantColors: %i\n", imageData.ImportantColors);
    printf("Planes: %i\n", imageData.Planes);
  

  
    Pallete Colors = get_All_Pallete_Colors(file.Bytes, imageData.BitsPerPixel);

    if(Colors.vlaid == false)
    {
        printf("Invalid Pallete \n");
        return 2;
    }


    // Getting image pixel data
    imageData.ImagePixels = form_Image(file.Bytes, imageData.ImageStartOffset, imageData.BitsPerPixel, imageData.Width, imageData.Height, &Colors);

    if(imageData.ImagePixels == NULL)
    {
        printf("Erororroroor \n");
        free(Colors.pixels);    
        free(file.Bytes);
    }
    
    int finheight = imageData.Height - 1;
    int finwidth = imageData.Width - 1;

    printf("Image last pixel| R: %i G: %i B: %i  A:%i \n", imageData.ImagePixels[abs(finheight)][finwidth].red, imageData.ImagePixels[abs(finheight)][finwidth].green, imageData.ImagePixels[abs(finheight)][finwidth].blue, imageData.ImagePixels[abs(finheight)][finwidth].alpha);
    printf("R: %i B: %i G: %i \n", Colors.pixels[0xc3].red, Colors.pixels[0xc3].green, Colors.pixels[0xc3].blue);

    create_image_file(imageData, "test.bmp");

// Free everything

    if(Colors.count > 18)
    {
        printf("R: %i B: %i G: %i \n", Colors.pixels[0xc3].red, Colors.pixels[0xc3].green, Colors.pixels[0xc3].blue);
    }

   
    for(int i = 0; i < imageData.Height; i++)
    {
        free(imageData.ImagePixels[i]);
    }
    free(imageData.ImagePixels);
    free(Colors.pixels);    
    free(file.Bytes);

}



args argParser(char *argv[], int argc) 
{
    bool foundinput = false;
    bool foundoutput = false;
    args arguments;

    for (int i = 0; i < argc; i++)
    {
        char *currentarg = argv[i];
        if(argv[i][0] == '-' )
        {
            // Single letters onlt
            if (strlen(argv[i]) == 2)
            {
                // Using -
                switch (argv[i][1])
                {
                    case 'd':
                        printf("Debugging \n");
                        break;
                    case 'o':
                        if(i+1 < argc && foundoutput == false)
                        {
                            printf("output: %s \n", argv[i+1]);
                            arguments.outputfile = argv[i+1];
                            i++;
                            foundoutput = true;
                            break;
                        }
                        else if(foundoutput == true) {
                            printf("More than 1 output given: -o %s\n",argv[i+1]);
                        }
                        else{
                            printf("No argument given after -o \n");
                        }
                        arguments.sucsess = false;
                        return arguments;

                        break;

                    case 'i':
                        if(i+1 < argc && foundinput == false)
                        {
                            printf("input: %s \n", argv[i+1]);
                            arguments.inputfile = argv[i+1];
                            foundinput = true;
                            i++;
                            break;
                        }
                        else if(foundinput == true) {
                            printf("More than 1 output given: -i %s\n",argv[i+1]);
                        }
                        else{
                            printf("No argument given after -i \n");
                        }
                        arguments.sucsess = false;
                        return arguments;

                        break;
                    
                    default:
                        printf("Others \n");
                        break;
                } 
            }
        
        }
        else if(i > 0 && foundinput == false || i > 0 && foundoutput == false)
        {
            if(foundinput == false)
            {
                arguments.inputfile = argv[i];
                foundinput = true;
            }
            else
            {
                arguments.outputfile = argv[i];
                foundoutput = true;
            }
        }
        else if(i > 0)
        {
            printf("Invlaid structure \n");
            arguments.sucsess = false;
            return arguments;
        }
    }

    arguments.sucsess = true;
    return arguments;

}
