#pragma once

#include <opencv2/opencv.hpp>
#include <gtkmm/image.h>

#include <string>

#include "macros.hpp"
#include "color_spaces.hpp"

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
    /**
     * Make a copy of the image where the values of the channels are restricted as given by the parameters.
     * 
     * @param src: original source image in RGB
     * @param dst: output image (will be overwritten)
     * @param color_space: the color space to be used for restriction
     * @param bottom*: the lower bounds for the channels
     * @param top*: the upper bounds for the channels
    */
    void limitImageByChannels(
        const cv::Mat& src,
        cv::Mat& dst,
        const ColorSpace& color_space,
        const double bottom0,
        const double top0,
        const double bottom1 = 0.0,
        const double top1 = 0.0,
        const double bottom2 = 0.0,
        const double top2 = 0.0
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