#pragma once

#include <string>
#include <vector>

/* A grayscale image: one byte (0..255) per pixel, stored row by row.
   Pixel (row, col) is at pixels[row * width + col]. */
struct GrayImage
{
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

/* ---------- Task 1: infrastructure ---------- */

/* Clamp any value to the 0..255 range (see "General tips" in the assignment) */
unsigned char Clip(int value);

/* Write a grayscale image as a 1-channel .png file. Returns true on success. */
bool SavePng(const std::string& path, const GrayImage& img);

/* Write the pixel values as numbers separated by commas, without spaces. Returns true on success.
   Note: each .txt file expects a different value range (see the "Debug" section of the assignment). */
bool SaveTxt(const std::string& path, const GrayImage& img);

/* ---------- Task 2: Grayscale (slide 4) ---------- */

/* rgba comes from stbi_load with 4 channels: R,G,B,A for each pixel */
GrayImage ToGrayscale(const unsigned char* rgba, int width, int height);

/* ---------- Task 3: Halftone (slide 11) ---------- */

/* Every input pixel becomes 2x2 output pixels, each 0 or 255.
   The output is (2 * width) x (2 * height). */
GrayImage Halftone(const GrayImage& gray);

/* ---------- Task 4: Floyd-Steinberg (slide 12) ---------- */

/* Reduce 256 gray levels to 16, spreading the error to the neighbours (7/16, 3/16, 5/16, 1/16). */
GrayImage FloydSteinberg(const GrayImage& gray);

/* ---------- Runs everything ---------- */

/* Loads the input image, runs all the filters and writes the 4 .png + 4 .txt files */
void RunAssignment(const std::string& inputPath);
