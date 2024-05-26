#pragma once

#include <esp_camera.h>
#include <vector>
#include <algorithm>
#include <cstring>

std::vector<bool> BackModel_;
int DialateRad_ = 3;
int ErosionRad_ = 2;

void background_estimation(const std::vector<bool>& img1, const std::vector<bool>& img2, int width, int height) {
    BackModel_.resize(width * height);
    for (int i = 0; i < width * height; ++i) {
        BackModel_[i] = img1[i] | img2[i];
        BackModel_[i] = ~BackModel_[i];
    }
}

void background_subtraction(const std::vector<bool>& img, int width, int height) {
    std::vector<bool> subtractedImage(width * height);
    for (int i = 0; i < width * height; ++i) {
        img[i] = img[i] | BackModel_[i];
    }
}

void dilate(std::vector<bool>& img, int width, int height, int radius) {
    std::vector<bool> temp = img;
    int kernelSize = 2 * radius + 1;
    for (int y = radius; y < height - radius; ++y) {
        for (int x = radius; x < width - radius; ++x) {
            bool maxValue = false;
            for (int ky = -radius; ky <= radius; ++ky) {
                for (int kx = -radius; kx <= radius; ++kx) {
                    maxValue = maxValue || temp[(y + ky) * width + (x + kx)];
                }
            }
            img[y * width + x] = maxValue;
        }
    }
}

void erode(std::vector<bool>& img, int width, int height, int radius) {
    std::vector<bool> temp = img;
    int kernelSize = 2 * radius + 1;
    for (int y = radius; y < height - radius; ++y) {
        for (int x = radius; x < width - radius; ++x) {
            bool minValue = true;
            for (int ky = -radius; ky <= radius; ++ky) {
                for (int kx = -radius; kx <= radius; ++kx) {
                    minValue = minValue && temp[(y + ky) * width + (x + kx)];
                }
            }
            img[y * width + x] = minValue;
        }
    }
}

void clousing_filter(std::vector<bool>& img, int width, int height) {
    dilate(img, width, height, DialateRad_);
    erode(img, width, height, ErosionRad_);
}

std::vector<bool> needle_detection(const std::vector<bool>& img, int width, int height) {
    std::vector<bool> subtractedImage = background_subtraction(img, width, height);
    clousing_filter(subtractedImage, width, height);
    return subtractedImage;
}
