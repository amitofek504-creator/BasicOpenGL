#include <Canny.h>

#include <cmath>

static GrayImage GaussianBlur(const GrayImage& in)
{
    GrayImage gaussian;
    gaussian.width = in.width;                                          
    gaussian.height = in.height; 
    gaussian.pixels = std::vector<unsigned char>(in.width * in.height);  
    for (int row = 0; row < in.height; row++)
         for (int col = 0; col < in.width; col++){
              int sum=0;
              int up    = std::max(row - 1, 0);
              int down  = std::min(row + 1, in.height - 1);
              int left  = std::max(col - 1, 0);
              int right = std::min(col + 1, in.width - 1);
              sum = 1 * in.pixels[up   * in.width + left] + 2 * in.pixels[up   * in.width + col] + 1 * in.pixels[up   * in.width + right]
                  + 2 * in.pixels[row  * in.width + left] + 4 * in.pixels[row  * in.width + col] + 2 * in.pixels[row  * in.width + right]
                  + 1 * in.pixels[down * in.width + left] + 2 * in.pixels[down * in.width + col] + 1 * in.pixels[down * in.width + right];
              gaussian.pixels[row * in.width + col] = sum / 16;
        }
    return gaussian;    
}

static void ComputeGradients(const GrayImage& in, std::vector<float>& magnitude, std::vector<float>& direction)
{  
    magnitude = std::vector<float>(in.width * in.height);
    direction = std::vector<float>(in.width * in.height);
    for (int row = 0; row < in.height; row++)
    for (int col = 0; col < in.width; col++){
        int p = row * in.width + col;
        int up    = std::max(row - 1, 0);
        int down  = std::min(row + 1, in.height - 1);
        int left  = std::max(col - 1, 0);
        int right = std::min(col + 1, in.width - 1);
        int gx = 1 * in.pixels[up   * in.width + left] - 1 * in.pixels[up   * in.width + right]
               + 2 * in.pixels[row  * in.width + left] - 2 * in.pixels[row  * in.width + right]
               + 1 * in.pixels[down * in.width + left] - 1 * in.pixels[down * in.width + right];
        int gy = 1 * in.pixels[up   * in.width + left] + 2 * in.pixels[up   * in.width + col] + 1 * in.pixels[up   * in.width + right]
               - 1 * in.pixels[down * in.width + left] - 2 * in.pixels[down * in.width + col] - 1 * in.pixels[down * in.width + right];
         magnitude[p] = sqrt(gx*gx + gy*gy);
         direction[p] = atan2(gy, gx);
        }
}

static std::vector<float> NonMaxSuppression(const std::vector<float>& magnitude, const std::vector<float>& direction,
                                            int width, int height)
{
  std:: vector<float> suppression(magnitude.size(), 0.0f);
  for (int row = 0; row < height; row++)
    for (int col = 0; col < width; col++) {
        int p = row * width + col;
        int up = std::max(row - 1, 0);
        int down = std::min(row + 1, height - 1);
        int left = std::max(col - 1, 0);
        int right = std::min(col + 1, width - 1);
        float angle = direction[p] * 180.0f / 3.14159265f;
        if (angle < 0)
          angle += 180.0f;
        int n1, n2;
        if (angle < 22.5f || angle >= 157.5f) {
            n1 = row * width + left;
            n2 = row * width + right;
        }
        else if (angle < 67.5f) {
            n1 = up * width + left;
            n2 = down * width + right;
        }
        else if (angle < 112.5f) {
            n1 = up * width + col;
            n2 = down * width + col;
        }
        else {
            n1 = up * width + right;
            n2 = down * width + left;
        }

        if (magnitude[p] >= magnitude[n1] && magnitude[p] >= magnitude[n2])
            suppression[p] = magnitude[p];
    }
  return suppression;
}
static GrayImage DoubleThreshold(const std::vector<float>& suppressed, int width, int height)
{
    GrayImage Double;
    Double.width = width;                                          
    Double.height = height; 
    Double.pixels = std::vector<unsigned char>(width * height);    
    int high = 110;
    int low = 55;
    for (size_t i = 0; i < suppressed.size(); i++)
        if(suppressed[i] >= high)
            Double.pixels[i] = 255;
        else if (suppressed[i] < high && suppressed[i] >= low)
            Double.pixels[i] = 128;
    return Double;       
}

static GrayImage Hysteresis(const GrayImage& thresholded)
{
    GrayImage hysteresis;
    hysteresis.width = thresholded.width;                                          
    hysteresis.height = thresholded.height; 
    hysteresis.pixels = std::vector<unsigned char>(thresholded.width * thresholded.height);
    for (int row = 0; row < thresholded.height; row++)
         for (int col = 0; col < thresholded.width; col++){
            int up    = std::max(row - 1, 0);
            int down  = std::min(row + 1, thresholded.height - 1);
            int left  = std::max(col - 1, 0);
            int right = std::min(col + 1, thresholded.width - 1);
            if (thresholded.pixels[row * thresholded.width + col] == 255)
                hysteresis.pixels[row * thresholded.width + col] = 255;
            else if (thresholded.pixels[row * thresholded.width + col] == 128)
                    if (thresholded.pixels[up   * thresholded.width + left ] == 255 ||
                        thresholded.pixels[up   * thresholded.width + col  ] == 255 ||
                        thresholded.pixels[up   * thresholded.width + right] == 255 ||
                        thresholded.pixels[row  * thresholded.width + left ] == 255 ||
                        thresholded.pixels[row  * thresholded.width + right] == 255 ||
                        thresholded.pixels[down * thresholded.width + left ] == 255 ||
                        thresholded.pixels[down * thresholded.width + col  ] == 255 ||
                        thresholded.pixels[down * thresholded.width + right] == 255)
                        hysteresis.pixels[row * thresholded.width + col] = 255;         
    }
    return hysteresis;
}

GrayImage Canny(const GrayImage& gray)
{
    GrayImage blurred = GaussianBlur(gray);

    std::vector<float> magnitude;
    std::vector<float> direction;
    ComputeGradients(blurred, magnitude, direction);

    std::vector<float> suppressed = NonMaxSuppression(magnitude, direction, gray.width, gray.height);
    GrayImage thresholded = DoubleThreshold(suppressed, gray.width, gray.height);
    return Hysteresis(thresholded);
}
