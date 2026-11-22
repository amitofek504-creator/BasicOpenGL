#include <Canny.h>

#include <cmath>

/* 6a. Noise reduction with a Gaussian filter (slide 5) */
static GrayImage GaussianBlur(const GrayImage& in)
{
    // TODO: 3x3 (1/16) or 5x5 (1/159) kernel - what happens at the image borders?
    return {};
}

/* 6b. Gradient magnitude and direction for every pixel (slides 6-7).
   magnitude and direction hold one value per pixel, in the same order as GrayImage::pixels. */
static void ComputeGradients(const GrayImage& in, std::vector<float>& magnitude, std::vector<float>& direction)
{
    // TODO: Sobel or central difference for X and Y, L2 or L1 norm, atan2 for the angle
}

/* 6c. Non-max suppression (slide 8): keep a pixel only if it is the strongest along its gradient direction */
static std::vector<float> NonMaxSuppression(const std::vector<float>& magnitude, const std::vector<float>& direction,
                                            int width, int height)
{
    // TODO: round each angle to one of 4 directions and compare with the 2 neighbours on that line
    return {};
}

/* 6d-1. Double thresholding (slide 9): split the pixels into strong / weak / non-relevant */
static GrayImage DoubleThreshold(const std::vector<float>& suppressed, int width, int height)
{
    // TODO: choose a low and a high threshold, and decide how to mark strong vs weak pixels
    return {};
}

/* 6d-2. Hysteresis (slide 10): a weak pixel touching a strong pixel becomes 255, everything else 0 */
static GrayImage Hysteresis(const GrayImage& thresholded)
{
    // TODO
    return {};
}

GrayImage Canny(const GrayImage& gray)
{
    // The stages in order (slide 3).
    // Tip: save a debug .png after each stage and compare with page 4 of the assignment PDF.
    GrayImage blurred = GaussianBlur(gray);

    std::vector<float> magnitude;
    std::vector<float> direction;
    ComputeGradients(blurred, magnitude, direction);

    std::vector<float> suppressed = NonMaxSuppression(magnitude, direction, gray.width, gray.height);
    GrayImage thresholded = DoubleThreshold(suppressed, gray.width, gray.height);
    return Hysteresis(thresholded);
}
