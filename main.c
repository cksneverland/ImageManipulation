#define _CRT_SECURE_NO_WARNINGS

#include "Headers/depends.h"
#include "Headers/bitmapfiles.h"
#include "Headers/filters.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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


    // grayscaleFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width);

    // Black n white test Quantization / dithering
    // for(int i = 0; i < abs(imageData.Height); i++)
    // {
    //     for(int j = 0; j < imageData.Width; j++)
    //     {
    //         int brightness = round(0.299*imageData.ImagePixels[i][j].red + 0.587 *imageData.ImagePixels[i][j].green+ 0.114*imageData.ImagePixels[i][j].blue);
    //         if(brightness <= 128)
    //         {
    //             //turn black son
    //             imageData.ImagePixels[i][j].red = 0;
    //             imageData.ImagePixels[i][j].green = 0;
    //             imageData.ImagePixels[i][j].blue = 0;
    //         }
    //         else
    //         {
    //             imageData.ImagePixels[i][j].red = 255;
    //             imageData.ImagePixels[i][j].green = 255;
    //             imageData.ImagePixels[i][j].blue = 255;
    //         }
    //     }
    // }

    //SIngle line error diffusion BL\lack and white
    // for(int i = 0; i < abs(imageData.Height); i++)
    // {
    //     signed int errorDifusion =0;
    //     for(int j = 0; j < imageData.Width; j++)
    //     {
    //         BYTE gray = imageData.ImagePixels[i][j].red;
    //         int brightness = round(0.299*gray + 0.587 *gray + 0.114*gray);
    //         brightness += errorDifusion;
    //         if(brightness <= 128)
    //         {
    //             //turn black son
    //             imageData.ImagePixels[i][j].red = 0;
    //             imageData.ImagePixels[i][j].green = 0;
    //             imageData.ImagePixels[i][j].blue = 0;
    //             errorDifusion = brightness;
    //         }
    //         else
    //         {
    //             imageData.ImagePixels[i][j].red = 255;
    //             imageData.ImagePixels[i][j].green = 255;
    //             imageData.ImagePixels[i][j].blue = 255;
    //             errorDifusion = brightness - 255;
    //         }
    //     }
    // }

    // Floyd-Steinberg dithering Black and white COoked up some bullshit here add no directional bias leter
    // int width = imageData.Width;
    // int height = abs(imageData.Height);
    // float *current = malloc(sizeof(float) * width);
    // float *next = malloc(sizeof(float) * width);
    // if (!current || !next) return 6;
    // memset(current,0,sizeof(float) *width);
    // memset(next,0,sizeof(float) *width);
    // for(int i =0; i < height; i++)
    // {
    //     for(int j = 0; j< width; j++)
    //     {
    //         float gray = 0.299f*imageData.ImagePixels[i][j].red + 0.587f *imageData.ImagePixels[i][j].green + 0.114f*imageData.ImagePixels[i][j].blue;
    //         float old_pixel = gray + current[j];
    //         if (old_pixel < 0) old_pixel = 0;
    //         if (old_pixel > 255) old_pixel = 255;
    //         float new_pixel = (old_pixel < 128) ? 0: 255;
    //         float error = old_pixel - new_pixel;
    //         imageData.ImagePixels[i][j].red = new_pixel;
    //         imageData.ImagePixels[i][j].green = new_pixel;
    //         imageData.ImagePixels[i][j].blue = new_pixel;
    //         if(j + 1 < width)
    //         {
    //             current[j+1] += error * (7.0f/16.0f);
    //         }
    //         if(i + 1 < height)
    //         {
    //             next[j] += error * (5.0f/16.0f);
    //             if(j-1 >= 0)
    //                 next[j-1] += error * (3.0f/16.0f);
    //             if(j+1 < width)
    //                 next[j+1] += error * (1.0f/16.0f);
    //         }
    //     }
    //     float *temp = current;
    //     current = next;
    //     next = temp;
    //     memset(next,0,sizeof(float) *width);
    // }
    // free(current);
    // free(next);

    //Multi error dithering w/ color
    typedef struct
    {
        int r,g,b;
    }errorDiffusion;
    

    typedef struct
    {
        int relativeRow;
        int relativeColumn;
        int weight;
        
    }diffusionParams;
    typedef struct
    {
        diffusionParams *params;
        BYTE numOfParams;
        BYTE divisor;
    }diffusionPattern;

    diffusionParams floydSteinberParams[] = {
        {0, 1, 7},
        {1,-1, 3},
        {1, 0, 5},
        {1, 1, 1}
    };
    const BYTE numFloydSteinbergParams = 4;
    const BYTE floydSteinbergDivisor =16;
    diffusionPattern floydSteinbergPattern ={floydSteinberParams, numFloydSteinbergParams, floydSteinbergDivisor};
    

    diffusionParams simple2DParams[] = {
        {0, 1, 1},
        {1, 0, 1}
    };
    const BYTE numSimple2DParams = 4;
    const BYTE simple2DDiviosr = 2;
    diffusionPattern simple2DPattern ={simple2DParams, numSimple2DParams, simple2DDiviosr};
    

    diffusionParams jarvisJudiceNinkeParams[] = {
        {0, 1, 7},
        {0, 2, 5},
        {1,-2, 3},
        {1,-1, 5},
        {1, 0, 7},
        {1, 1, 5},
        {1, 2, 3},
        {1,-2, 1},
        {1,-1, 3},
        {1, 0, 5},
        {1, 1, 3},
        {1, 2, 1}
    };
    const BYTE numJarvisJudiceNinkeParams = 12;
    const BYTE jarvisJudiceNinkeDivisor = 48;
    diffusionPattern jarvisJudiceNinkePattern ={jarvisJudiceNinkeParams, numJarvisJudiceNinkeParams, jarvisJudiceNinkeDivisor};
    

    diffusionParams atkinsonParams[] = {
        {0, 1, 1},
        {0, 2, 1},
        {1,-1, 1},
        {1, 0, 1},
        {1, 2, 1},
        {2, 0, 1},
    };
    const BYTE numAtkinsonParams = 12;
    const BYTE atkinsonDivisor = 8;
    diffusionPattern atkinsonPattern ={atkinsonParams, numAtkinsonParams, atkinsonDivisor};

    enum{
        floyd,
        simple2D,
        jarvisJudiceNinke,
        atkinson
    }diffusionChoices;

    diffusionPattern allDiffusionPaterns[] = {floydSteinbergPattern, simple2DPattern, jarvisJudiceNinkePattern, atkinsonPattern};
    diffusionPattern *chosenPattern = &allDiffusionPaterns[floyd];
    int divisor = chosenPattern->divisor;


    const int Width = imageData.Width;
    const int Height = abs(imageData.Height);
    errorDiffusion **buffer = NULL;
    errorDiffusion *bufferRows = NULL;

    bufferRows = malloc(sizeof(errorDiffusion) * Width * Height);
    buffer = malloc(sizeof(errorDiffusion*) * Height);
    
    if(!bufferRows || !buffer)
    {
        free(buffer);
        free(bufferRows);
        printf("Buffer Rows or Buffer had an error \n");
        return 7;
    }
    memset(bufferRows, 0, sizeof(errorDiffusion) * Width * Height);
    for(int i =0; i < Height; i++)
    {
        buffer[i] = &bufferRows[i * Width];
    }


    BYTE channelLevel = 3;
    float step = 255 / (float)(channelLevel- 1);

    BYTE quantizeLookup[256];
    int quantizeErrorLookup[256];
    for(int i = 0; i < 256; i++)
    {
        int index = (i * (channelLevel - 1) + 127) / 255; // rounding be adding .5 technically just learend ts
        quantizeLookup[i] = index * step;
    }
    for(int i = 0; i < 256; i++)
    {
        quantizeErrorLookup[i] = i - quantizeLookup[i];
    }

    for(int row = 0; row < Height; row++)
    {
        pixel *currentImageRow = imageData.ImagePixels[row];
        errorDiffusion *currentBufferRow = buffer[row];
        

        for(int column = 0; column < Width; column++)
        {
            int oldPixelr = currentImageRow[column].red   + ((currentBufferRow[column].r + (divisor >> 1))/ divisor); // >> 1 = /2 incase i forget
            int oldPixelg = currentImageRow[column].green + ((currentBufferRow[column].g + (divisor >> 1))/ divisor);
            int oldPixelb = currentImageRow[column].blue  + ((currentBufferRow[column].b + (divisor >> 1))/ divisor);

            // Not over 255 not under 0
            oldPixelr = pixelClamp(oldPixelr);
            oldPixelg = pixelClamp(oldPixelg);
            oldPixelb = pixelClamp(oldPixelb);

            int errorR = quantizeErrorLookup[oldPixelr];
            int errorG = quantizeErrorLookup[oldPixelg];
            int errorB = quantizeErrorLookup[oldPixelb];

            currentImageRow[column].red   = quantizeLookup[oldPixelr];
            currentImageRow[column].green = quantizeLookup[oldPixelg];
            currentImageRow[column].blue  = quantizeLookup[oldPixelb];



            for(int i = 0; i < chosenPattern->numOfParams; i++)
            {
                int relRow = row + chosenPattern->params[i].relativeRow;
                int relColumn = column + chosenPattern->params[i].relativeColumn;

                if(relRow >= 0 && relRow < Height)
                {
                    if(relColumn >= 0 && relColumn < Width)
                    {
                        buffer[relRow][relColumn].r += errorR * chosenPattern->params[i].weight;
                        buffer[relRow][relColumn].g += errorG * chosenPattern->params[i].weight;
                        buffer[relRow][relColumn].b += errorB * chosenPattern->params[i].weight;

                    }

                }

            }


        }
    }

    free(buffer);
    free(bufferRows);

    
    

    // boxBlurFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width, 3);



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
