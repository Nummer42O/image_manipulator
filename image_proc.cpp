#include "image_proc.hpp"

#include <memory>

#define MAX_8BIT 0xFF

void image_proc::limitImageByHSV(const cv::Mat& src, cv::Mat& dst, double hue_bottom, double hue_top, double sat_bottom, double sat_top, double val_bottom, double val_top) {
    //make the "base" grayscale
    //cv::cvtColor(src, dst, cv::COLOR_RGB2GRAY);

    //make a mask for the areas which are within the given ranges
    cv::Scalar lower_boundary(hue_bottom, sat_bottom, val_bottom),
               upper_boundary(hue_top, sat_top, val_top);
    cv::Mat mask, temp;
    cv::cvtColor(src, temp, cv::COLOR_RGB2HSV_FULL);
    cv::inRange(temp, lower_boundary, upper_boundary, dst);

    //use the mask to add color back in to the image
    cv::copyTo(src, dst, mask);
}

/**
 * Iterate over every pixel and set selected channels to the minimum value of that pixel.
 * 
 * @param src: Source image in RGB color space
 * @param dst: Output image (will be overwritten)
 * @param output_channel: wether to choose channel 0, 1, 2 or all (-1)
*/
void setChannelsToMin(const cv::Mat& src, cv::Mat& dst, const image_proc::ChannelOption& output_channel) {
    src.copyTo(dst);

    if (output_channel == image_proc::ChannelOption::ALL) {
        dst.forEach<image_proc::Pixel>(
            [](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t min = std::min(pixel[0], pixel[1]);
                min = std::min(min, pixel[2]);

                pixel[0] = min; pixel[1] = min; pixel[2] = min;
            }
        );
    } else {
        dst.forEach<image_proc::Pixel>(
            [output_channel](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t min = std::min(pixel[0], pixel[1]);
                min = std::min(min, pixel[2]);

                pixel[output_channel] = min;
                pixel[(output_channel + 1) % 3] = 0;
                pixel[(output_channel + 2) % 3] = 0;
            }
        );
    }
}

/**
 * Iterate over every pixel and set selected channels to average of all values of that pixel.
 * 
 * @param src: Source image in RGB color space
 * @param dst: Output image (will be overwritten)
 * @param output_channel: wether to choose channel 0, 1, 2 or all (-1)
*/
void setChannelsToAvg(const cv::Mat& src, cv::Mat& dst, const image_proc::ChannelOption& output_channel) {
    src.copyTo(dst);

    if (output_channel == image_proc::ChannelOption::ALL) {
        dst.forEach<image_proc::Pixel>(
            [](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t avg = (pixel[0] + pixel[1] + pixel[2]) / 3u;

                pixel[0] = avg; pixel[1] = avg; pixel[2] = avg;
            }
        );
    } else {
        dst.forEach<image_proc::Pixel>(
            [output_channel](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t avg = (pixel[0] + pixel[1] + pixel[2]) / 3u;

                pixel[output_channel] = avg;
                pixel[(output_channel + 1) % 3] = 0;
                pixel[(output_channel + 2) % 3] = 0;
            }
        );
    }
}

/**
 * Iterate over every pixel and set selected channels to the median of all values of that pixel.
 * 
 * @param src: Source image in RGB color space
 * @param dst: Output image (will be overwritten)
 * @param output_channel: wether to choose channel 0, 1, 2 or all (-1)
void setChannelsToMedian(const cv::Mat& src, cv::Mat& dst, const image_proc::ChannelOption& output_channel) {
    src.copyTo(dst);

    if (output_channel == image_proc::ChannelOption::ALL) {
        dst.forEach<image_proc::Pixel>(
            [](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t min = std::min(pixel[0], pixel[1]);
                min = std::min(min, pixel[2]);
                uint8_t max = std::max(pixel[0], pixel[1]);
                max = std::max(max, pixel[2]);
                uint8_t median = pixel[0] + pixel[1] + pixel[2] - min - max; //only works for 3 values;

                pixel[0] = median; pixel[1] = median; pixel[2] = median;
            }
        );
    } else {
        dst.forEach<image_proc::Pixel>(
            [output_channel](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t min = std::min(pixel[0], pixel[1]);
                min = std::min(min, pixel[2]);
                uint8_t max = std::max(pixel[0], pixel[1]);
                max = std::max(max, pixel[2]);
                uint8_t median = pixel[0] + pixel[1] + pixel[2] - min - max; //only works for 3 values;

                pixel[output_channel] = median;
                pixel[(output_channel + 1) % 3] = 0;
                pixel[(output_channel + 2) % 3] = 0;
            }
        );
    }
}
*/

/**
 * Iterate over every pixel and set selected channels to the maximum value of that pixel.
 * 
 * @param src: Source image in RGB color space
 * @param dst: Output image (will be overwritten)
 * @param output_channel: wether to choose channel 0, 1, 2 or all (-1)
*/
void setChannelsToMax(const cv::Mat& src, cv::Mat& dst, const image_proc::ChannelOption& output_channel) {
    src.copyTo(dst);

    if (output_channel == image_proc::ChannelOption::ALL) {
        dst.forEach<image_proc::Pixel>(
            [](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t max = std::max(pixel[0], pixel[1]);
                max = std::max(max, pixel[2]);

                pixel[0] = max; pixel[1] = max; pixel[2] = max;
            }
        );
    } else {
        dst.forEach<image_proc::Pixel>(
            [output_channel](image_proc::Pixel& pixel, const int*) -> void {
                uint8_t max = std::max(pixel[0], pixel[1]);
                max = std::max(max, pixel[2]);

                pixel[output_channel] = max;
                pixel[(output_channel + 1) % 3] = 0;
                pixel[(output_channel + 2) % 3] = 0;
            }
        );
    }}

void image_proc::manipulateChannels(const cv::Mat& src, cv::Mat& dst, const ModifierOption& modifier, const ChannelOption& channel) {
    cv::Mat temp;
    int channel_index;
    switch (modifier) {
        case ModifierOption::MIN:
            setChannelsToMin(src, dst, channel);
            return;
        case ModifierOption::AVG:
            setChannelsToAvg(src, dst, channel);
            return;
        case ModifierOption::MAX:
            setChannelsToMax(src, dst, channel);
            return;
        case ModifierOption::RED:
        case ModifierOption::GREEN:
        case ModifierOption::BLUE:
            src.copyTo(temp);
            channel_index = modifier;

            break;
        case ModifierOption::HUE:
        case ModifierOption::SAT:
        case ModifierOption::VAL:
            cv::cvtColor(src, temp, cv::COLOR_RGB2HSV_FULL);
            channel_index = modifier - 3;

            break;
    }

    std::array<cv::Mat, 3> channels;
    cv::split(temp, channels);
    cv::Mat empty_temp(src.rows, src.cols, CV_8UC3);
    if (channel == ChannelOption::ALL) {
        cv::Mat channel_mat = channels[channel_index];
        
        cv::cvtColor(channel_mat, empty_temp, cv::COLOR_GRAY2RGB); //pretend to be GRAY
    } else {
        cv::Mat empty_channel(src.rows, src.cols, src.type(), cv::Scalar(0, 0, 0));
        channels[(channel_index + 1) % 3] = empty_channel;
        channels[(channel_index + 2) % 3] = empty_channel;

        cv::merge(channels, empty_temp);
    }
    dst = std::move(empty_temp);
}

void image_proc::compressImage(const cv::Mat& src, cv::Mat& dst, const CompressionMode& mode) {
    src.copyTo(dst);

    if (mode == CompressionMode::NORMAL) {
        return;
    }

    uint8_t max_value = mode;
    dst.forEach<Pixel>(
        [max_value](Pixel& pixel, const int*) -> void {
            pixel[0] = max_value * pixel[0] / MAX_8BIT;
            pixel[1] = max_value * pixel[1] / MAX_8BIT;
            pixel[2] = max_value * pixel[2] / MAX_8BIT;
        }
    );
}

bool image_proc::loadImage(cv::Mat& image, const std::string& filepath) {
    cv::Mat temp = cv::imread(filepath);
    
    if (temp.empty()) {
        return false;
    } else {
        cv::cvtColor(temp, image, cv::COLOR_BGR2RGB);

        return true;
    }
}

bool image_proc::saveImage(const cv::Mat& image, const std::string& filepath) {
    cv::Mat temp;
    cv::cvtColor(image, temp, cv::COLOR_RGB2BGR);

    return cv::imwrite(filepath, temp);
}