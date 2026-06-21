# ImageManipulation

A command-line image processing tool written in C for applying various filters and dithering algorithms to BMP images.

## Features

* Grayscale conversion
* Color inversion
* Sobel edge detection
* Box blur
* Error diffusion dithering

  * Floyd-Steinberg
  * Simple
  * Atkinson
  * Jarvis-Judice-Ninke
  * Stucki
  * Sierra
* Bayer ordered dithering
* Debug mode
* Filter chaining

---

## Requirements

* Clang (or GCC)
* Windows, Linux, or any platform with a C compiler

---

## Building

### Using Clang

```bash
clang main.c Headers/depends.c Headers/bitmapfiles.c Headers/filters.c -o imagedtecv2
```

### PowerShell

```powershell
./make.ps1
```

---

## Usage

```bash
imagedtecv2 -i <input.bmp> -o <output.bmp> [filters]
```

If no output file is specified, the image is saved as:

```text
test.bmp
```

---

# Basic Examples

### Grayscale

```bash
imagedtecv2 -i input.bmp -o output.bmp -greyscale
```

---

### Invert Colors

```bash
imagedtecv2 -i input.bmp -o output.bmp -cInvert
```

---

### Sobel Edge Detection

```bash
imagedtecv2 -i input.bmp -o output.bmp -sobel
```

---

### Box Blur

Default blur size is 3×3.

```bash
imagedtecv2 -i input.bmp -o output.bmp -boxBlur
```

Custom blur size:

```bash
imagedtecv2 -i input.bmp -o output.bmp -boxBlur -a 7
```

---

### Error Diffusion Dithering

Default settings:

* Algorithm: Floyd-Steinberg
* Scan direction: Both
* Threshold level: 2

```bash
imagedtecv2 -i input.bmp -o output.bmp -errorDiff
```

#### Select algorithm

```bash
-t Floyd
-t Simple
-t Atkinson
-t Jarvis
-t Stucki
-t Sierra
```

Example:

```bash
imagedtecv2 -i input.bmp -o output.bmp -errorDiff -t Atkinson
```

#### Scan direction

```bash
-s Both
-s Left
-s Right
```

Example:

```bash
imagedtecv2 -i input.bmp -o output.bmp -errorDiff -s Right
```

#### Threshold level

```bash
-n <number>
```

Example:

```bash
imagedtecv2 -i input.bmp -o output.bmp -errorDiff -n 4
```

---

### Bayer Ordered Dithering

Default:

* Matrix size: 4×4
* Threshold level: 4

```bash
imagedtecv2 -i input.bmp -o output.bmp -Bayer
```

Matrix sizes supported:

```text
2
4
8
16
```

Example:

```bash
imagedtecv2 -i input.bmp -o output.bmp -Bayer -a 8 -n 4
```

---

## Filter Chaining

Filters are executed in the order they are provided.

Example:

```bash
imagedtecv2 -i input.bmp -o output.bmp -greyscale -boxBlur -a 5 -sobel
```

Pipeline:

1. Convert to grayscale
2. Apply 5×5 blur
3. Run Sobel edge detection

---

## Debug Mode

```bash
imagedtecv2 -i input.bmp -d
```

Displays bitmap information:

* Width
* Height
* Bits per pixel
* Compression
* File size
* Header size
* Image size
* Palette information

---

## Supported Input Format

Currently only BMP files are supported.

The program validates the BMP signature before processing.

---

## Project Structure

```
ImageManipulation/
│
├── main.c
├── Headers/
│   ├── depends.c
│   ├── depends.h
│   ├── bitmapfiles.c
│   ├── bitmapfiles.h
│   ├── filters.c
│   └── filters.h
│
├── images/
├── Bitmaps/
├── MoreBMPS/
├── test.bmp
├── make.ps1
└── README.md
```

---

## Examples

Grayscale:

```bash
imagedtecv2 -i stadium.bmp -o gray.bmp -greyscale
```

Floyd-Steinberg dithering:

```bash
imagedtecv2 -i stadium.bmp -o dither.bmp -errorDiff -t Floyd
```

Blur and edge detection:

```bash
imagedtecv2 -i stadium.bmp -o edge.bmp -boxBlur -a 5 -sobel
```

---

## Notes

* Filters are applied sequentially.
* Parameters such as `-a`, `-n`, `-t`, and `-s` belong to the filter immediately preceding them.
* Missing filter parameters use default values.
* Only BMP images are currently supported.

