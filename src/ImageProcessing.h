#pragma once

#include <string>
#include <vector>

struct GrayImage
{
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

unsigned char Clip(int value);

bool SavePng(const std::string& path, const GrayImage& img);

bool SaveTxt(const std::string& path, const GrayImage& img);

GrayImage ToGrayscale(const unsigned char* rgba, int width, int height);

GrayImage Halftone(const GrayImage& gray);

GrayImage FloydSteinberg(const GrayImage& gray);

void RunAssignment(const std::string& inputPath);
