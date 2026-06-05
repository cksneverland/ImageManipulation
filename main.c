#define _CRT_SECURE_NO_WARNINGS

#include "Headers/depends.h"
#include "Headers/bitmapfiles.h"
#include "Headers/filters.h"
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum argtypes{
    argError,
    Sobel,
    greyscale,
    invert,
    boxblur
};
typedef struct
{
    bool sucsess;
    bool debug;
    bool grayscale;
    bool boxblur;
    bool sobeledge;
    bool invert;
    char *inputfile;
    char *outputfile;
}args;
void outputHelp(void);
args argParser(char *argv[], int argc);
int checkOptinalArgs(char* arg);

// Function

int main(int argc, char *argv[])
{
    
    if (argc < 2)
    {
        printf("Invalid arguments\n Usage: ./imagedetec input output \n");
        outputHelp();
        return 1;
    }

    args parsedargs = argParser(argv, argc);

    if (parsedargs.sucsess == false)
    {
        printf("Invalid arguments\n Usage: ./imagedetec input output \n");
        outputHelp();
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

    bitmapfile imageData = get_Bitmapfile_Data(file);
    

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
    
    int finheight = abs(imageData.Height) - 1;
    int finwidth = imageData.Width - 1;

    printf("Image last pixel| R: %i G: %i B: %i  A:%i \n", imageData.ImagePixels[abs(finheight)][finwidth].red, imageData.ImagePixels[abs(finheight)][finwidth].green, imageData.ImagePixels[abs(finheight)][finwidth].blue, imageData.ImagePixels[abs(finheight)][finwidth].alpha);
    printf("R: %i B: %i G: %i \n", Colors.pixels[0xc3].red, Colors.pixels[0xc3].green, Colors.pixels[0xc3].blue);

    if(parsedargs.grayscale == true)
        grayscaleFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width);

    if(parsedargs.invert == true)
        invertedColorFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width);

    if(parsedargs.boxblur == true)
        boxBlurFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width, 3);

    if(parsedargs.sobeledge == true)
        sobelEdgeDetection(imageData.ImagePixels, abs(imageData.Height), imageData.Width);
    

    


    create_image_file(imageData, "test.bmp");

    
// Free everything

    if(Colors.count > 18)
    {
        printf("R: %i B: %i G: %i \n", Colors.pixels[0xc3].red, Colors.pixels[0xc3].green, Colors.pixels[0xc3].blue);
    }


    free_Image_Pixels(imageData.ImagePixels, imageData.Height);

    free(Colors.pixels);    
    free(file.Bytes);

}

void outputHelp(void)
{
    printf("Arguments:\n   -o: Output\n   -i: Input\n   -d: Debug\n   -sobel: Uses sobel edge detection\n   -greyscale: It's gray   \n   -cInvert: Inverts the color\n   -boxBlur: Blurs the image using box blur algoritim\n");
}

args argParser(char *argv[], int argc) 
{
    bool foundinput = false;
    bool foundoutput = false;
    args arguments;

    arguments.boxblur = false;
    arguments.debug = false;
    arguments.grayscale = false;
    arguments.invert = false;
    arguments.outputfile = NULL;
    arguments.sobeledge = false;

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
                    // Output
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
                    // Input
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
                        printf("Error with argument %s%s%s example.\n", "\033[4m", argv[i], "\033[0m"); // Just for underlining im crine
                        arguments.sucsess = false;
                        return arguments;
                        break;
                } 
            }
            
            else
            {
                switch (checkOptinalArgs(argv[i]))
                            {
                            case argError:
                                arguments.sucsess = false;
                                return arguments;
                                break;
                            case Sobel:
                                arguments.sobeledge = true;
                                break;
                            case greyscale:
                                arguments.grayscale = true;
                                break;
                            case invert:
                                arguments.invert = true;
                                break;
                            case boxblur:
                                arguments.boxblur = true;
                                break;
                            }
            }
            // BoxBlur, SobelEdge, Grayscale, Inverted
           
        
        }
        // Out find a input or output 
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

// optomise later w/ a single loop if i get more  args
int checkOptinalArgs(char* arg)
{
    if(strcmp(arg, "-sobel") == 0)
        return Sobel;
    
    if(!strcmp(arg, "-greyscale"))
        return greyscale;
    
    if(!strcmp(arg, "-cInvert"))
        return invert;
    
    if(!strcmp(arg, "-boxblur"))
        return boxblur;
    
    printf("Error with argument %s%s%s example.\n", "\033[4m", arg, "\033[0m"); // Just for underlining im crine
    return argError;
}