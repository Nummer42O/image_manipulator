#pragma once

#include <opencv2/opencv.hpp>
#include <gtkmm/image.h>

#include <string>
#include <map>

#include "macros.hpp"

/*
 * Channels of GRAY color space: 1
 * Channels of BGR color space: 3
 * Channels of BGR565 color space: 2
 * Channels of BGR555 color space: 2
 * Channels of XYZ color space: 3
 * Channels of YCrCb color space: 3
 * Channels of Lab color space: 3
 * Channels of Luv color space: 3
 * Channels of HSV color space: 3
 * Channels of HLS color space: 3
 * Channels of YUV color space: 3
 * Channels of YUV_I420 color space: 1
 * Channels of YUV_YV12 color space: 1
*/

namespace image_proc {
    enum ColorSpace {
        RGB,
        GRAY,                       BGR,                        BGR565,                     BGR555,                     XYZ,
        YCrCb,                      Lab,                        Luv,                        HSV,                        HLS,
        YUV,                        YUV_I420,                   YUV_YV12
    };
    const std::array<std::string, NR_COLOR_SPACES> color_space_names {
        "RGB",
        "GRAY",                     "BGR",                      "BGR565",                   "BGR555",                   "XYZ",
        "YCrCb",                    "Lab",                      "Luv",                      "HSV",                      "HLS",
        "YUV",                      "YUV_I420",                 "YUV_YV12",
    };
    const std::array<std::array<std::string, NR_CHANNELS>, NR_COLOR_SPACES> color_space_channels {{
        {{" R ", " G ", " B "}},
        {{" GRAY ", "", ""}},       {{" B ", " G ", " R "}},    {{" B ", " G ", " R "}},    {{" B ", " G ", " R "}},    {{" X ", " Y ", " Z "}},
        {{" Y ", " Cr ", " Cb "}},  {{" L ", " a ", " b "}},    {{" L ", " u ", " v "}},    {{" H ", " S ", " V "}},    {{" H ", " L ", " S "}},
        {{" Y ", " U ", " V "}},    {{" Y ", "", ""}},          {{" Y ", "", ""}},
    }};
    const std::array<cv::ColorConversionCodes, NR_COLOR_SPACES> convert_from_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_RGB2GRAY,         cv::COLOR_RGB2BGR,          cv::COLOR_RGB2BGR565,       cv::COLOR_RGB2BGR555,       cv::COLOR_RGB2XYZ,
        cv::COLOR_RGB2YCrCb,        cv::COLOR_RGB2Lab,          cv::COLOR_RGB2Luv,          cv::COLOR_RGB2HSV_FULL,     cv::COLOR_RGB2HLS_FULL,
        cv::COLOR_RGB2YUV,          cv::COLOR_RGB2YUV_I420,     cv::COLOR_RGB2YUV_YV12,
    };
    const std::array<cv::ColorConversionCodes, NR_COLOR_SPACES> convert_to_rgb {
        cv::COLOR_COLORCVT_MAX,
        cv::COLOR_GRAY2RGB,         cv::COLOR_BGR2RGB,          cv::COLOR_BGR5652RGB,       cv::COLOR_BGR5552RGB,       cv::COLOR_XYZ2RGB,
        cv::COLOR_YCrCb2RGB,        cv::COLOR_Lab2RGB,          cv::COLOR_Luv2RGB,          cv::COLOR_HSV2RGB_FULL,     cv::COLOR_HLS2RGB_FULL,
        cv::COLOR_YUV2RGB,          cv::COLOR_YUV2RGB_I420,     cv::COLOR_YUV2RGB_YV12,
    };
    
    /**
     * Convert preview matrices into their respective color space.
     * 
     * @param limit_preview_matrix_originals: original preview images to be converted
     * @param limit_preview_matrix_references: destination images
    */
    void convertScalePreviewColorSpaces(
        const std::array<const cv::Mat, NR_CHANNELS>& limit_preview_matrix_originals,
        const std::array<cv::Mat, NR_CHANNELS>& limit_preview_matrix_references,
        const ColorSpace& color_space
    );

    
    /**
     * Make a copy of the image where the values of the HSV color space are restricted as given by the parameters.
     * 
     * @param src: original source image in RGB
     * @param dst: output image (will be overwritten)
     * @param hue_bottom: lower restriction of hue
     * @param hue_top: upper restriction of hue
     * @param sat_bottom: lower restriction of saturation
     * @param sat_top: upper restriction of saturation
     * @param val_bottom: lower restriction of value
     * @param val_top: upper restriction of value
    */
    void limitImageByHSV(
        const cv::Mat& src,
        cv::Mat& dst,
        double hue_bottom,
        double hue_top,
        double sat_bottom,
        double sat_top,
        double val_bottom,
        double val_top
    );


    enum ChannelOption {
        ALL = -1,
        R = 0, G = 1, B = 2
    };
    enum ModifierOption {
        MIN = -1, AVG = -2, MAX = -4,
        RED = 0, GREEN = 1, BLUE = 2,
        HUE = 3, SAT   = 4, VAL  = 5
    };
    typedef cv::Vec<uint8_t, NR_CHANNELS> Pixel;

    /**
     * Change image look by manipulating certain channels.
     * 
     * @param src: source image in RGB color space
     * @param dst: output image (will be overwritten)
     * @param modifier: what modification to perform
     * @param channel: which channels should be affected
    */
    void manipulateChannels(
        const cv::Mat& src,
        cv::Mat& dst,
        const ModifierOption& modifier,
        const ChannelOption& channel
    );


    /**
     * Compress image (with losses) to lower bits per channel per pixel.
     * 
     * @param src: source image
     * @param dst: output image (will be overwritten)
     * @param compression_level: level of compression from 1 bit to 8 bits
    */
    void compressImage(
        const cv::Mat& src,
        cv::Mat& dst,
        double compression_level
    );


    /**
     * Load image from a file and converts it to RGB.
     * If the load fails, image will remain unchanged.
     * 
     * @param image: output image (will be overwritten)
     * @param filepath: path to image file
     * @return wether or not the load was successfull
    */
    bool loadImage(
        cv::Mat& image,
        const std::string& filepath
    );

    /**
     * Save RGB image to file.
     * 
     * @param image: source image
     * @param filepath: file path to save image to
     * @return wether or not the save was successfull
    */
    bool saveImage(
        const cv::Mat& image,
        const std::string& filepath
    );


    /**
     * Return a representation of the average color of the image.
     * Format: "Red: {0} Green: {1} Blue: {3}"
     * 
     * @param image: image to take the mean from
     * @return formatted mean string
    */
    std::string getAverageColorString(
        const cv::Mat& image
    );


    /**
     * Convert an image from a cv::Mat to an Gtk::Image.
     * 
     * @param src: source image to be converted (RGB, 3channel, 8bit)
     * @param dst: resulting Gtk image (existing pixbuf will be overwritten)
    */
    void convertCVtoGTK(
        const cv::Mat& src,
        Gtk::Image& dst
    );
}