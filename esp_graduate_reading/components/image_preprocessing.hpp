#pragma once

#include <esp_camera.h>
#include <vector>
#include <cmath>

int BlurRadius_ = 3;
int Threshold_ = 0;
int ArrowColor_ = 0;

void GaussBlur(uint8_t* image, int width, int height) {
    int kernelSize = 2 * BlurRadius_ + 1;
    std::vector<std::vector<float>> kernel(kernelSize, std::vector<float>(kernelSize));
    float sigma = BlurRadius_ / 2.0;
    float sum = 0.0;

    for (int y = -BlurRadius_; y <= BlurRadius_; ++y) {
        for (int x = -BlurRadius_; x <= BlurRadius_; ++x) {
            float val = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            kernel[y + BlurRadius_][x + BlurRadius_] = val;
            sum += val;
        }
    }

    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            kernel[y][x] /= sum;
        }
    }

    std::vector<uint8_t> tempImage(width * height);

    for (int y = BlurRadius_; y < height - BlurRadius_; ++y) {
        for (int x = BlurRadius_; x < width - BlurRadius_; ++x) {
            float pixel = 0.0;
            for (int ky = -BlurRadius_; ky <= BlurRadius_; ++ky) {
                for (int kx = -BlurRadius_; ++kx) {
                    pixel += image[(y + ky) * width + (x + kx)] * kernel[ky + BlurRadius_][kx + BlurRadius_];
                }
            }
            tempImage[y * width + x] = static_cast<uint8_t>(pixel);
        }
    }

    memcpy(image, tempImage.data(), width * height);
}

int ComputeOtsuThreshold(uint8_t* image, int width, int height) {
    int histogram[256] = {0};
    for (int i = 0; i < width * height; ++i) {
        histogram[image[i]]++;
    }

    int total = width * height;
    float sum = 0;
    for (int t = 0; t < 256; ++t) sum += t * histogram[t];

    float sumB = 0;
    int wB = 0;
    int wF = 0;
    float varMax = 0;
    int threshold = 0;

    for (int t = 0; t < 256; ++t) {
        wB += histogram[t];
        if (wB == 0) continue;
        wF = total - wB;
        if (wF == 0) break;

        sumB += t * histogram[t];
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = wB * wF * (mB - mF) * (mB - mF);
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }

    return threshold;
}

std::vector<bool> Binarization(uint8_t* image, int width, int height) {
    std::vector<bool> binaryImage(width * height);

    int T = Threshold_ ? Threshold_ : ComputeOtsuThreshold(image, width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            binaryImage[index] = (image[index] >= T) ^ ArrowColor_;
        }
    }

    return binaryImage;
}
