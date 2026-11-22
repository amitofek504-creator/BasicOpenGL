#pragma once

#include <ImageProcessing.h>

/* ---------- Task 6: Canny edge detection (slides 3-10) ---------- */

/* Returns an edges image with pixels 0 or 255 only */
GrayImage Canny(const GrayImage& gray);
