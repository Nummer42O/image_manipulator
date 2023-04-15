#pragma once

#include <opencv2/opencv.hpp>

namespace image_proc {
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
    typedef cv::Vec<uint8_t, 3> Pixel;

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


    enum CompressionMode {
        ONE_BIT = 0b1,
        TWO_BIT = 0b11,
        FOUR_BIT = 0b1111,
        NORMAL = 0
    };

    /**
     * Compress image (with losses) to lower bits per channel per pixel.
     * 
     * @param src: source image
     * @param dst: output image (will be overwritten)
     * @param mode: level of compression
    */
    void compressImage(
        const cv::Mat& src,
        cv::Mat& dst,
        const CompressionMode& mode
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
}