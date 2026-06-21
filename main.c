#define _CRT_SECURE_NO_WARNINGS

#include "Headers/depends.h"
#include "Headers/bitmapfiles.h"
#include "Headers/filters.h"
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    argError,
    sobelArg,
    greyscaleArg,
    invertArg,
    boxblurArg,
    errorDitheringArg,
    bayerDitheringArg
}argtypes;

// Union is basicly more than 1 struct in the same memory lwk glad i found it
typedef struct
{
    argtypes type;

    union
    {
        struct
        {
            WORD blurScale;
        } boxBlur;

        struct
        {
            int diffusionType;
            signedByte scan;
            BYTE level;
        } errorDither;

        struct
        {
            BYTE scale;
            BYTE level;
        } bayerDither;
    } args;
}argumentStorage;

typedef struct
{
    bool sucsess;
    bool debug;
    argumentStorage *argOrder;
    int numOfArgs;
    char *inputfile;
    char *outputfile;
}args;

void outputHelp(void);
args argParser(char *argv[], int argc);
int checkFunctionArgs(char* arg);
bool isanum(char *argv);
char *toUpper(char *word);
char *toLower(char *word);
scanType getScanType(char *choice);
diffusionPatterns getDitherType(char *choice);
int getNumOfFunctionArgs(char *argv[], int argc);



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
        if(parsedargs.argOrder != NULL)
        {
            free(parsedargs.argOrder);
        }
        outputHelp();
        return 2;
    }

    char* output = NULL;
    if(parsedargs.outputfile != NULL)
    {
        output = parsedargs.outputfile;
    }
    else{
        output = "test.bmp";
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
    imageData.debugmode = parsedargs.debug;
    

    // Debuging for now Should change latter
    if(parsedargs.debug == true)
    {
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
    }

  
    Pallete Colors = get_All_Pallete_Colors(file.Bytes, imageData.BitsPerPixel);

    if(Colors.vlaid == false)
    {
        printf("Invalid Pallete \n");
        return 2;
    }


    // Getting image pixel data
    
    imageData.ImagePixels = form_Image(file.Bytes, imageData.ImageStartOffset, imageData.BitsPerPixel, imageData.Width, imageData.Height, &Colors);
    imageData.imageFilterBuffer = createFilterBuffer(imageData.Height, imageData.Width);


    if(imageData.ImagePixels == NULL)
    {
        printf("Erororroroor \n");
        free(Colors.pixels);    
        free(file.Bytes);
    }
    
    int finheight = abs(imageData.Height) - 1;
    int finwidth = imageData.Width - 1;

    scanType scan = 0;
    BYTE level = 0;
    BYTE scale = 0;
    diffusionPatterns pattern;

    for(int i = 0; i < parsedargs.numOfArgs; i ++)
    {
        switch (parsedargs.argOrder[i].type)
        {
        case greyscaleArg:
            grayscaleFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width);
            break;
        case invertArg:
            invertedColorFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width);
            break;
        case boxblurArg:
            boxBlurFilter(imageData.ImagePixels, abs(imageData.Height), imageData.Width, parsedargs.argOrder[i].args.boxBlur.blurScale, imageData.imageFilterBuffer);
            break;
        case sobelArg:
            sobelEdgeDetection(imageData.ImagePixels, abs(imageData.Height), imageData.Width, imageData.imageFilterBuffer);            
            break;
        case errorDitheringArg: ;
                scan = parsedargs.argOrder[i].args.errorDither.scan;
                level = parsedargs.argOrder[i].args.errorDither.level;
                pattern = parsedargs.argOrder[i].args.errorDither.diffusionType;
                errorDiffusionDithering(imageData.ImagePixels, imageData.Height, imageData.Width, pattern, scan, level, imageData.imageFilterBuffer);
                break;
        case bayerDitheringArg: ;
                level = parsedargs.argOrder[i].args.bayerDither.level;
                scale = parsedargs.argOrder[i].args.bayerDither.scale;
                orderedDithering(imageData.ImagePixels, imageData.Height, imageData.Width, scale, level);
                break;
            
        default:
            break;
        }
    }
    

    

    create_image_file(imageData, output);

    
// Free everything



    // free_Image_Pixels(imageData.ImagePixels, imageData.Height);
    free_Image_Pixels(imageData.ImagePixels);
    freeFilterBuffer(imageData.imageFilterBuffer);
    if(parsedargs.argOrder != NULL)
        free(parsedargs.argOrder);
    free(Colors.pixels);    
    free(file.Bytes);

}

void outputHelp(void)
{
    printf("Arguments:\n"
           "   -o: Output\n"
           "   -i: Input\n"
           "   -d: Debug\n"
           "   -sobel: Uses sobel edge detection\n"
           "   -greyscale: It's gray\n"
           "   -cInvert: Inverts the color\n"
           "   -boxBlur: Blurs the image using box blur algoritim\n"
           "        -a: used to select blur area ex:3 = 3x3\n"
           "   -errorDiff: Uses error diffusion dithering\n"
           "        -n: used to select the dithering threshold level\n"
           "        -s: is the direction the scan will take\n"
           "            Options: Both, Left, Right\n"
           "        -t: this Selects the algorithim of error diffusion dithering\n"
           "            Options:\n"
           "                     Floyd\n"
           "                     Simple\n"
           "                     Atkinson\n"
           "                     Jarvis\n"
           "                     Stucki\n"
           "                     Sierra\n"
           "   -Bayer\n"
           "        -a: used to select bayer matrix valid matrixs are 2,4,8,16 ex:2 = 2x2\n"
           "        -n: used to select the dithering threshold level\n");
}

args argParser(char *argv[], int argc) 
{
    bool foundinput = false;
    bool foundoutput = false;
    args arguments;
    int numOfArgs = getNumOfFunctionArgs(argv, argc);
    argumentStorage *tempArgStore = malloc(sizeof(argumentStorage) * numOfArgs);
    if(!tempArgStore)
    {
        printf("Error alocating tempArgStore\n");
        arguments.sucsess = false;
        return arguments;

    }
    arguments.argOrder = tempArgStore;
    
    int currentarg = 0;
    int argsIndex = -1;// cus there isnt even 1 function arg found yet

    arguments.debug = false;
    arguments.outputfile = NULL;

    for (int i = 1; i < argc; i++)
    {

        
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
                    case 'n': // numbers
                        if(currentarg > 0)
                        {
                            if(i + 1 >= argc)
                            {
                                printf("no numbers giver for -n\n");
                                        arguments.sucsess = false;
                                        return arguments;

                            }

                            int number=0;
                            if(isanum(argv[i+1]) == true)
                            {
                                number = atoi(argv[i+1]);
                            }
                            else{
                                    printf("-n accecpt numbers only \n");
                                    arguments.sucsess = false;
                                    return arguments;
                                }


                            switch (tempArgStore[argsIndex].type)
                            {
                            case errorDitheringArg:                                 
                                tempArgStore[argsIndex].args.errorDither.level = number;
                                i++;
                                break;

                            case bayerDitheringArg:
                                tempArgStore[argsIndex].args.bayerDither.level = number;
                                i++;
                                break;
                            
                            default:
                                printf("-n suports boxblur, errordithering, and bayer dithering \n");
                                arguments.sucsess = false;
                                return arguments;
                                break;
                            }
                        }
                        break;
                    
                    case 's': //scan type
                            if(currentarg > 0)
                            {
                                if(i+1 < argc)
                                {
                                    scanType scanChoice = getScanType(argv[i+1]);
                                    if(scanChoice == scanErr)
                                    {
                                        printf("Not valid scan type/Direction \n");
                                        arguments.sucsess = false;
                                        return arguments;
                                    }

                                    switch (tempArgStore[argsIndex].type)
                                    {
                                    case errorDitheringArg:
                                        tempArgStore[argsIndex].args.errorDither.scan = scanChoice;
                                        i++;
                                        break;
                                    
                                    default:
                                        printf("-s Suports errordiffusion \n");
                                        arguments.sucsess = false;
                                        return arguments;
                                        break;
                                    }
                                    

                                }
                            }
                        break;

                    case 't': //dither type mostly
                            if(currentarg > 0)
                            {
                                if(i+1 < argc)
                                {
                                    diffusionPatterns diffuisonChoice = getDitherType(argv[i+1]);
                                    if(diffuisonChoice == diffusionPatternError)
                                    {
                                        printf("Not valid scan diffusionType \n");
                                        arguments.sucsess = false;
                                        return arguments;
                                    }

                                    switch (tempArgStore[argsIndex].type)
                                    {
                                    case errorDitheringArg:
                                        tempArgStore[argsIndex].args.errorDither.diffusionType = diffuisonChoice;
                                        i++;
                                        break;
                                    
                                    default:
                                        printf("-t Suports error diffusion \n");
                                        arguments.sucsess = false;
                                        return arguments;
                                        break;
                                    }
                                    

                                }
                            }
                        break;

                    case 'a':
                        if(currentarg > 0)
                        {
                            if(i + 1 >= argc)
                            {
                                printf("no numbers giver for -a\n");
                                        arguments.sucsess = false;
                                        return arguments;

                            }
                            int number = 0;
                            if(isanum(argv[i+1]) == true)
                            {
                                number = atoi(argv[i+1]);
                            }
                            switch (tempArgStore[argsIndex].type)
                            {
                                case boxblurArg:                                 
                                    tempArgStore[argsIndex].args.boxBlur.blurScale = number;
                                    i++;
                                    break;

                                case bayerDitheringArg:
                                    
                                    switch (number)
                                    {
                                        case 2:
                                        case 4:
                                        case 8:
                                        case 16:
                                            tempArgStore[argsIndex].args.bayerDither.scale = number;
                                            i++;
                                            break;
                                    
                                    default:
                                        printf("Bayer dithering only suports scales of 2, 4, 8, and 16\n");
                                        arguments.sucsess = false;
                                        return arguments;
                                        break;
                                    }

                                    break;
                                
                                default:
                                        printf("-a suports Blur and bayer\n");
                                        arguments.sucsess = false;
                                        return arguments;
                                        break;
                            }
                        
                            
                        break;      
                                

                    
                    }
                    default:
                        printf("Error with argument %s%s%s example.\n", "\033[4m", argv[i], "\033[0m"); // Just for underlining im crine
                        arguments.sucsess = false;
                        return arguments;
                        break;
                } 
            }
            
            else
            {
                argsIndex++;
                currentarg++;
                // pretty up later ie never
                switch (checkFunctionArgs(argv[i]))
                            {                          
                            case sobelArg:
                                tempArgStore[argsIndex].type = sobelArg;
                                break;
                            case greyscaleArg:
                                tempArgStore[argsIndex].type = greyscaleArg;
                                break;
                            case invertArg:
                                tempArgStore[argsIndex].type = invertArg;
                                break;
                            case boxblurArg:
                                tempArgStore[argsIndex].type = boxblurArg; 
                                tempArgStore[argsIndex].args.boxBlur.blurScale = 3; //setting default
                                break;
                            case errorDitheringArg:
                                tempArgStore[argsIndex].type = errorDitheringArg;
                                tempArgStore[argsIndex].args.errorDither.diffusionType = diffusionPatternFloyd; //setting default
                                tempArgStore[argsIndex].args.errorDither.scan = scanBothWays; //setting default
                                tempArgStore[argsIndex].args.errorDither.level = 2; //setting default
                                break;
                            case bayerDitheringArg:
                                tempArgStore[argsIndex].type = bayerDitheringArg;
                                tempArgStore[argsIndex].args.bayerDither.level = 4; //setting default
                                tempArgStore[argsIndex].args.bayerDither.scale = 4; //setting default

                                break;
                            default:
                                printf("%s%s%s is not a valid argument\n", "\033[4m", argv[i], "\033[0m"); // Just for underlining im crine

                                arguments.sucsess = false;
                                return arguments;
                                break;
                            }


            }
            // Major args here
           
        
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

    arguments.numOfArgs = numOfArgs;
    arguments.sucsess = true;
    return arguments;

}

int getNumOfFunctionArgs(char *argv[], int argc)
{
    int numFunctionArgs = 0;
    for(int i = 0; i < argc; i++)
    {
        if(checkFunctionArgs(argv[i]) != argError)
        {
            numFunctionArgs++;
        }

    }
    return numFunctionArgs;

}

// optomise later w/ a single loop if i get more  args
int checkFunctionArgs(char* arg)
{
    char* temparg = toLower(arg);

    if(strcmp(temparg, "-sobel") == 0)
        return sobelArg;
    
    if(strcmp(temparg, "-greyscale") == 0)
        return greyscaleArg;
    if(strcmp(temparg, "-grayscale") == 0)
        return greyscaleArg;
    
    if(strcmp(temparg, "-cinvert") == 0)
        return invertArg;
    
    if(strcmp(temparg, "-boxblur") == 0)
        return boxblurArg;

    if(strcmp(temparg, "-errordiff") == 0)
        return errorDitheringArg;

    if(strcmp(temparg, "-bayer") == 0)
        return bayerDitheringArg;
    
    return argError;
}

bool isanum(char *argv)
{
    for(int i = 0; i < strlen(argv); i++)
    {
        if(argv[i] < '0' || argv[i] > '9')
        {
            return false;
        }
    }
    return true;
}

char *toUpper(char *word)
{
    BYTE distanceBetweencases =32; // a - A
    for(int i = 0; i < strlen(word); i++)
    {
        if(word[i] >= 'a' && word[i] <= 'z')
        {
            word[i] = word[i] - distanceBetweencases;

        }
    }
    return word;
}
char *toLower(char *word)
{
    BYTE distanceBetweencases =32; // a - A
    for(int i = 0; i < strlen(word); i++)
    {
        if(word[i] >= 'A' && word[i] <= 'Z')
        {
            word[i] = word[i] + distanceBetweencases;

        }
    }
    return word;
}

diffusionPatterns getDitherType(char *choice)
{
    char *word = toUpper(choice);
                                    

    if(strcmp("FLOYD", word) == 0)
    {
        return diffusionPatternFloyd;
    }
    else if(strcmp("ATKINSON", word) == 0)
    {
        return diffusionPatternAtkinson;
    }
    else if(strcmp("JARVIS", word) == 0)
    {
        return diffusionPatternJarvisJudiceNinke;
    }
    else if(strcmp("SIMPLE", word) == 0)
    {
        return diffusionPatternSimple2D;
    }
    else if(strcmp("SIERRA", word) == 0)
    {
        return diffusionPatternSierra;;
    }
    else if(strcmp("STUCKI", word) == 0)
    {
        return diffusionPatternStucki;
    }

    return diffusionPatternError;


}

scanType getScanType(char *choice)
{
    char *word = toUpper(choice);
                                    

    if(strcmp("BOTH", word) == 0)
    {
        return scanBothWays;
    }
    else if(strcmp("RIGHT", word) == 0)
    {
        return scanRight;
    }
    else if(strcmp("LEFT", word) == 0)
    {
        return scanLeft;
    }

    return scanErr;

}