#pragma once

#include <opencv2/imgproc.hpp>

#include <string>

#include "macros.hpp"


namespace image_proc {
    enum ColorSpace: size_t {
        RGB,
        BGR,                        XYZ,                        YCrCb,                      Lab,
        Luv,                        HSV,                        HLS,                        YUV,

        LAST
    };
    const std::array<const std::string, ColorSpace::LAST> color_space_names {
        "RGB",
        "BGR",                      "XYZ",                      "YCrCb",                    "Lab",
        "Luv",                      "HSV",                      "HLS",                      "YUV",
    };

    const std::array<const std::array<const std::string, NR_CHANNELS>, ColorSpace::LAST> color_space_channels {{
        {{"R","G","B"}},
        {{"B","G","R"}},            {{"X","Y","Z"}},            {{"Y","Cr","Cb"}},          {{"L","a","b"}},
        {{"L","u","v"}},            {{"H","S","V"}},            {{"H","L","S"}},            {{"Y","U","V"}},
    }};

    const std::array<cv::ColorConversionCodes, ColorSpace::LAST> convert_from_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_RGB2BGR,          cv::COLOR_RGB2XYZ,          cv::COLOR_RGB2YCrCb,        cv::COLOR_RGB2Lab,
        cv::COLOR_RGB2Luv,          cv::COLOR_RGB2HSV_FULL,     cv::COLOR_RGB2HLS_FULL,     cv::COLOR_RGB2YUV,
    };
    const std::array<cv::ColorConversionCodes, ColorSpace::LAST> convert_to_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_BGR2RGB,          cv::COLOR_XYZ2RGB,          cv::COLOR_YCrCb2RGB,        cv::COLOR_Lab2RGB,
        cv::COLOR_Luv2RGB,          cv::COLOR_HSV2RGB_FULL,     cv::COLOR_HLS2RGB_FULL,     cv::COLOR_YUV2RGB,
    };
}