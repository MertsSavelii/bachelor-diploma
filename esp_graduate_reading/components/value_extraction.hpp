#pragma once

#include <vector>
#include <cmath>

int CentrX_ = 160;
int CentrY_ = 120;
float StartAngle_ = 0.0;
float AngularIncreas_ = 1.0;

float fast_atan2(float y, float x) {
    if (x == 0.0f) {
        if (y > 0.0f) return M_PI_2;
        if (y == 0.0f) return 0.0f;
        return -M_PI_2;
    }
    float atan;
    float z = y / x;
    if (fabs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f) {
            if (y < 0.0f) return atan - M_PI;
            return atan + M_PI;
        }
    } else {
        atan = M_PI_2 - z / (z * z + 0.28f);
        if (y < 0.0f) return atan - M_PI;
    }
    return atan;
}

void calculate_center_of_mass(const std::vector<bool>& img, int width, int height, float &centerX, float &centerY) {
    float sumX = 0.0;
    float sumY = 0.0;
    float sum = 0.0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (img[y * width + x]) {
                sumX += x;
                sumY += y;
                sum += 1.0;
            }
        }
    }

    if (sum == 0.0) {
        centerX = 0.0;
        centerY = 0.0;
    } else {
        centerX = sumX / sum;
        centerY = sumY / sum;
    }
}

float extract_value(const std::vector<bool>& img, int width, int height) {
    float centerX, centerY;
    calculate_center_of_mass(img, width, height, centerX, centerY);

    float deltaX = centerX - CentrX_;
    float deltaY = centerY - CentrY_;
    float angle = fast_atan2(deltaY, deltaX);

    float correctedAngle = angle - StartAngle_;
    float value = correctedAngle * AngularIncreas_;

    return value;
}
