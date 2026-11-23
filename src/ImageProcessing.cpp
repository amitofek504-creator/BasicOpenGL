#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <ImageProcessing.h>
#include <Canny.h>

#include <fstream>
#include <iostream>

/* ---------- Task 1: infrastructure ---------- */

unsigned char Clip(int value)
{
    if( value < 0)
        return 0;
    if(value > 255)
        return 255;
    return value;
}

bool SavePng(const std::string& path, const GrayImage& img)
{
    int result = stbi_write_png( path.c_str() , img.width , img.height , 1 , img.pixels.data() , img.width);
    return !(result == 0);
}

bool SaveTxt(const std::string& path, const GrayImage& img)
{
    std::ofstream file(path);
    if (!file)
        return false;
    for (size_t i = 0; i < img.pixels.size(); i++)
      if(i < img.pixels.size()-1 )
         file << (int)img.pixels[i] << ",";   
      else 
        file << (int)img.pixels[i];
    return true;
}

/* ---------- Task 2: Grayscale ---------- */

GrayImage ToGrayscale(const unsigned char* rgba, int width, int height)
{
  GrayImage gray;                                              
  gray.width = width;                                          
  gray.height = height;                                        
  gray.pixels = std::vector<unsigned char>(width * height);  
  for (int row = 0; row < height; row++)
    for (int col = 0; col < width; col++) {
        int p = row * width + col;
        int r = rgba[p * 4];        
        int g = rgba[p * 4 + 1];    
        int b = rgba[p * 4 + 2];    
        gray.pixels[p] = (r + g + b) / 3; 
    }
  return gray;
}

/* ---------- Task 3: Halftone ---------- */

GrayImage Halftone(const GrayImage& gray)
{
   GrayImage half;     
   int grayIndex;                                         
   half.width = (gray.width)*2;                                          
   half.height = (gray.height)*2;                                        
   half.pixels = std::vector<unsigned char>(((gray.width)*2) * ((gray.height)*2));  
   for (int row = 0; row < gray.height; row++)
    for (int col = 0; col < gray.width; col++){
         grayIndex = gray.pixels[row * gray.width + col];
         int UpperLeft = (2 * row) * half.width + (2 * col);
         int bottumLeft = UpperLeft + half.width;
         int upperRight = (2 * row) * half.width  + (2 * col) + 1;
         int buttumRight = UpperLeft + half.width + 1;
         if (grayIndex <51) {
            half.pixels[UpperLeft] = 0;
            half.pixels[upperRight] = 0;
            half.pixels[bottumLeft] = 0;
            half.pixels[buttumRight] = 0;
         }
        else if (grayIndex >50 && grayIndex < 102){
            half.pixels[UpperLeft] = 0;
            half.pixels[upperRight] = 0;
            half.pixels[bottumLeft] = 255;
            half.pixels[buttumRight] = 0;
        }
         else if (grayIndex >101 && grayIndex < 153){
            half.pixels[UpperLeft] = 0;
            half.pixels[upperRight] = 255;
            half.pixels[bottumLeft] = 255;
            half.pixels[buttumRight] = 0;
        }
         else if (grayIndex >152 && grayIndex < 204){
            half.pixels[UpperLeft] = 0;
            half.pixels[upperRight] = 255;
            half.pixels[bottumLeft] = 255;
            half.pixels[buttumRight] = 255;
        }
         else if (grayIndex >203 && grayIndex < 256){
            half.pixels[UpperLeft] = 255;
            half.pixels[upperRight] = 255;
            half.pixels[bottumLeft] = 255;
            half.pixels[buttumRight] = 255;
        }
    }
        return half;

}

/* ---------- Task 4: Floyd-Steinberg ---------- */

GrayImage FloydSteinberg(const GrayImage& gray)
{
   GrayImage FS;
   FS.width = gray.width;                                          
   FS.height = gray.height; 
   FS.pixels = std::vector<unsigned char>(gray.width * gray.height);  
    std::vector<float> tmp(gray.pixels.size());
   for (size_t i = 0; i < tmp.size(); i++)
       tmp[i] = gray.pixels[i];
   for (int row = 0; row < gray.height; row++)
    for (int col = 0; col < gray.width; col++){
        float level = Clip(tmp[row * gray.width + col]) / 16;
        FS.pixels[row * gray.width + col] = 16 * (int)level;
        bool hasRight = col < gray.width - 1;
        bool hasDown = row < gray.height - 1;
        bool hasDownLeft = hasDown && col > 0;
        bool hasDownRight = hasDown && hasRight;
        int count = hasRight + hasDownLeft + hasDown + hasDownRight;
        if (count == 4) {
        tmp[(row * gray.width + col) +1] += (Clip(tmp[row * gray.width + col]) % 16) * (7.0f/16);
        tmp[(row * gray.width + col) + gray.width - 1] += (Clip(tmp[row * gray.width + col]) % 16) * (3.0f/16);
        tmp[(row * gray.width + col) + gray.width] += (Clip(tmp[row * gray.width + col]) % 16) * (5.0f/16);
        tmp[(row * gray.width + col) + gray.width + 1] += (Clip(tmp[row * gray.width + col]) % 16) * (1.0f/16);
        }
        else if (count > 0) {
            float share = (Clip(tmp[row * gray.width + col]) % 16) / (float)count;
            if (hasRight)
                tmp[(row * gray.width + col) +1] += share;
            if (hasDownLeft)
                tmp[(row * gray.width + col) + gray.width - 1] += share;
            if (hasDown)
                tmp[(row * gray.width + col) + gray.width] += share;
            if (hasDownRight)
                tmp[(row * gray.width + col) + gray.width + 1] += share;
        }
    } 
    return FS;      
}
/* ---------- Runs everything ---------- */

void RunAssignment(const std::string& inputPath)
{
    int width, height, channels;
    unsigned char* rgba = stbi_load(inputPath.c_str(), &width, &height, &channels, 4);
    if (rgba == nullptr) {
        std::cout << "Failed to load the image" << std::endl;
        return;
    }   
    std::cout << width << " " << height << " " << channels << std::endl;
    GrayImage gray = ToGrayscale(rgba, width, height);
    if (!SavePng("../Grayscale.png", gray))
        std::cout << "Failed to save Grayscale.png" << std::endl;
    if (!SaveTxt("../Grayscale.txt",gray))
        std::cout << "Failed to save Grayscale.txt" << std::endl;
    GrayImage half = Halftone(gray);
    if (!SavePng("../Haftone.png", half))
        std::cout << "Failed to save Haftone.png" << std::endl;
    GrayImage halfTxt = half;                                   
    for (size_t i = 0; i < halfTxt.pixels.size(); i++)          
        halfTxt.pixels[i] = halfTxt.pixels[i] / 255;            
    if (!SaveTxt("../Halftone.txt", halfTxt))                  
        std::cout << "Failed to save Halftone.txt" << std::endl;
     GrayImage FS = FloydSteinberg(gray);
    if (!SavePng("../FloyedSteinberg.png", FS))
        std::cout << "Failed to save FloyedSteinberg.png" << std::endl;
    GrayImage FSTxt = FS;                                   
    for (size_t i = 0; i <FSTxt.pixels.size(); i++)          
        FSTxt.pixels[i] = FSTxt.pixels[i] / 16;            
    if (!SaveTxt("../FloyedSteinberg.txt", FSTxt))                  
        std::cout << "Failed to save FloyedSteinberg.txt" << std::endl;
    // TODO (task 6): Canny           -> Canny.png           + Canny.txt            (values 0-1 in the .txt)
    //
    // File names are exactly as written in the assignment ("Haftone.png" is spelled that way there).
    // The program runs from bin/, so "../" writes the files to the project folder.
     stbi_image_free(rgba);
}
