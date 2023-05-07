#pragma once

#include <opencv2/imgproc.hpp>

#include <string>

#include "macros.hpp"


namespace image_proc {
    enum ColorSpace: size_t {
        RGB,
        GRAY,                       BGR,                        XYZ,                        YCrCb,                      Lab,
        Luv,                        HSV,                        HLS,                        YUV,                        YUV_I420,
        YUV_YV12,

        LAST
    };
    const std::array<const std::string, ColorSpace::LAST> color_space_names {
        "RGB",
        "GRAY",                     "BGR",                      "XYZ",                      "YCrCb",                    "Lab",
        "Luv",                      "HSV",                      "HLS",                      "YUV",                      "YUV_I420",
        "YUV_YV12",
    };

    const std::array<const std::array<const std::string, NR_CHANNELS>, ColorSpace::LAST> color_space_channels {{
        {{" R ", " G ", " B "}},
        {{" GRAY ", "", ""}},       {{" B ", " G ", " R "}},    {{" X ", " Y ", " Z "}},    {{" Y ", " Cr ", " Cb "}},  {{" L ", " a ", " b "}},
        {{" L ", " u ", " v "}},    {{" H ", " S ", " V "}},    {{" H ", " L ", " S "}},    {{" Y ", " U ", " V "}},    {{" Y ", "", ""}},
        {{" Y ", "", ""}},
    }};
    const std::array<const size_t, ColorSpace::LAST> color_space_nr_channels {
        /*RGB*/ 3ul,
        /*GRAY*/ 1ul,               /*BGR*/ 3ul,                /*XYZ*/ 3ul,                /*YCrCb*/ 3ul,              /*Lab*/ 3ul,
        /*Luv*/ 3ul,                /*HSV*/ 3ul,                /*HLS*/ 3ul,                /*YUV*/ 3ul,                /*YUV_I420*/ 1ul,
        /*YUV_YV12*/ 1ul,
    };

    const std::array<cv::ColorConversionCodes, ColorSpace::LAST> convert_from_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_RGB2GRAY,         cv::COLOR_RGB2BGR,          cv::COLOR_RGB2XYZ,          cv::COLOR_RGB2YCrCb,        cv::COLOR_RGB2Lab,
        cv::COLOR_RGB2Luv,          cv::COLOR_RGB2HSV_FULL,     cv::COLOR_RGB2HLS_FULL,     cv::COLOR_RGB2YUV,          cv::COLOR_RGB2YUV_I420,
        cv::COLOR_RGB2YUV_YV12,
    };
    const std::array<cv::ColorConversionCodes, ColorSpace::LAST> convert_to_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_GRAY2RGB,         cv::COLOR_BGR2RGB,          cv::COLOR_XYZ2RGB,          cv::COLOR_YCrCb2RGB,        cv::COLOR_Lab2RGB,
        cv::COLOR_Luv2RGB,          cv::COLOR_HSV2RGB_FULL,     cv::COLOR_HLS2RGB_FULL,     cv::COLOR_YUV2RGB,          cv::COLOR_YUV2RGB_I420,
        cv::COLOR_YUV2RGB_YV12,
    };
}