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
    // TODO: stbi_write_png 
    //the last parameter is bytes per row - how many channels does a gray image have?
    int result = stbi_write_png( path.c_str() , img.width , img.height , 1 , img.pixels.data() , img.width);
    return !(result == 0);
}

bool SaveTxt(const std::string& path, const GrayImage& img)
{
    // TODO: std::ofstream, numbers separated by commas, no spaces
    std::ofstream file(path);
    if (!file)
        return false;
    for (int i = 0; i < img.pixels.size(); i++)
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
    // TODO: output size is 2x in each direction; thresholds and fill order are on slide 11
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
    // TODO: which data type can hold the value after adding a negative or >255 error?
    //       missing neighbours at the edges: split the error equally between the existing ones
    return {};
}

/* ---------- Runs everything ---------- */

void RunAssignment(const std::string& inputPath)
{
    // TODO (task 1): load the input with stbi_load (ask for 4 channels), check it is not nullptr,
    //                and free it with stbi_image_free when you are done
    int width, height, channels;
    unsigned char* rgba = stbi_load(inputPath.c_str(), &width, &height, &channels, 4);
    if (rgba == nullptr) {
        std::cout << "Failed to load the image" << std::endl;
        return;
    }   
    std::cout << width << " " << height << " " << channels << std::endl;
    // Task 2: Grayscale. The .txt keeps the raw 0-255 values (the assignment text is ambiguous: 0-15 or 0-255)
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
    // TODO (task 3): Halftone        -> Haftone.png         + Halftone.txt         (values 0-1 in the .txt)
    // TODO (task 4): FloydSteinberg  -> FloyedSteinberg.png + FloyedSteinberg.txt  (values 0-15 in the .txt)
    // TODO (task 6): Canny           -> Canny.png           + Canny.txt            (values 0-1 in the .txt)
    //
    // File names are exactly as written in the assignment ("Haftone.png" is spelled that way there).
    // Relative paths are written to bin/, because the program runs from there.
     stbi_image_free(rgba);
}
