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
                        }
                        else{
                            printf("no ouptput given or more than 1 output \n");
                            arguments.sucsess = false;
                            return arguments;
                        }
                        break;
                    case 'i':
                        if(i+1 < argc && foundinput == false)
                        {
                            printf("input: %s \n", argv[i+1]);
                            arguments.inputfile = argv[i+1];
                            foundinput = true;
                            i++;
                        }
                        else{
                            printf("no input given or more than 1 input \n");
                            arguments.sucsess = false;
                            return arguments;
                        }
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