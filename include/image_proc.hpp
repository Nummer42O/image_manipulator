#pragma once

#include <opencv2/opencv.hpp>

#include <string>
#include <map>

namespace image_proc {
    const std::map<std::array<std::string, 2>, std::vector<std::array<std::string, 2>>> conversion_constraints = {
//         ('BGR', ''): ('BGRA', '')
//              ('RGBA', '')
//              ('RGB', '')
//              ('GRAY', '')
//              ('BGR565', '')
//              ('BGR555', '')
//              ('XYZ', '')
//              ('YCrCb', '')
//              ('HSV', '')
//              ('Lab', '')
//              ('Luv', '')
//              ('HLS', '')
//              ('HSV', 'FULL')
//              ('HLS', 'FULL')
//              ('YUV', '')
//              ('YUV', 'I420')
//              ('YUV', 'YV12')
// ('BGRA', ''): ('BGR', '')
//               ('RGBA', '')
//               ('GRAY', '')
//               ('BGR565', '')
//               ('BGR555', '')
//               ('YUV', 'I420')
//               ('YUV', 'YV12')
// ('RGBA', ''): ('BGR', '')
//               ('GRAY', '')
//               ('BGR565', '')
//               ('BGR555', '')
//               ('mRGBA', '')
//               ('YUV', 'I420')
//               ('YUV', 'YV12')
// ('RGB', ''): ('GRAY', '')
//              ('BGR565', '')
//              ('BGR555', '')
//              ('XYZ', '')
//              ('YCrCb', '')
//              ('HSV', '')
//              ('Lab', '')
//              ('Luv', '')
//              ('HLS', '')
//              ('HSV', 'FULL')
//              ('HLS', 'FULL')
//              ('YUV', '')
//              ('YUV', 'I420')
//              ('YUV', 'YV12')
// ('GRAY', ''): ('BGR', '')
//               ('BGRA', '')
//               ('BGR565', '')
//               ('BGR555', '')
// ('BGR565', ''): ('BGR', '')
//                 ('RGB', '')
//                 ('BGRA', '')
//                 ('RGBA', '')
//                 ('GRAY', '')
// ('BGR555', ''): ('BGR', '')
//                 ('RGB', '')
//                 ('BGRA', '')
//                 ('RGBA', '')
//                 ('GRAY', '')
// ('XYZ', ''): ('BGR', '')
//              ('RGB', '')
// ('YCrCb', ''): ('BGR', '')
//                ('RGB', '')
// ('HSV', ''): ('BGR', '')
//              ('RGB', '')
//              ('BGR', 'FULL')
//              ('RGB', 'FULL')
// ('Lab', ''): ('BGR', '')
//              ('RGB', '')
//              ('LBGR', '')
//              ('LRGB', '')
// ('Luv', ''): ('BGR', '')
//              ('RGB', '')
//              ('LBGR', '')
//              ('LRGB', '')
// ('HLS', ''): ('BGR', '')
//              ('RGB', '')
//              ('BGR', 'FULL')
//              ('RGB', 'FULL')
// ('LBGR', ''): ('Lab', '')
//               ('Luv', '')
// ('LRGB', ''): ('Lab', '')
//               ('Luv', '')
// ('YUV', ''): ('BGR', '')
//              ('RGB', '')
// ('YUV', 'NV12'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
// ('YUV', 'NV21'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
// ('YUV', 'YV12'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
// ('YUV', 'IYUV'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
// ('YUV', '420'): ('GRAY', '')
// ('YUV', 'UYVY'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
//                  ('GRAY', '')
// ('YUV', 'YUY2'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
//                  ('GRAY', '')
// ('YUV', 'YVYU'): ('RGB', '')
//                  ('BGR', '')
//                  ('RGBA', '')
//                  ('BGRA', '')
// ('mRGBA', ''): ('RGBA', '')
    };
    
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
}