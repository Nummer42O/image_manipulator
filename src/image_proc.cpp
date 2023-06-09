#include <memory>

#include "image_proc.hpp"

#define MAX_8BIT 0xFF


void image_proc::limitImageByChannels(const cv::Mat& src, cv::Mat& dst, const ColorSpace& color_space,
                                      const double bottom0, const double top0, const double bottom1, const double top1, const double bottom2, const double top2) {
    const cv::Scalar lower_boundary(bottom0, bottom1, bottom2),
                     upper_boundary(top0,    top1,    top2);

    // make a mask for the areas which are within the given ranges
    cv::Mat mask, temp;
    if (color_space) { // color_space 0 is RGB, so it does not need to be converted
        cv::cvtColor(src, temp, image_proc::convert_from_rgb[color_space]);
        cv::inRange(temp, lower_boundary, upper_boundary, mask);
    } else {
        cv::copyTo(src, temp, cv::noArray());
        cv::inRange(temp, lower_boundary, upper_boundary, mask);
    }

    // create gray 3-channel background image
    cv::Mat gray, foreground, background;
    cv::cvtColor(temp, gray, cv::COLOR_RGB2GRAY);
    cv::cvtColor(gray, gray, cv::COLOR_GRAY2RGB);

    // mask the gray background with the colorful original image
    std::cout << mask.size() << temp.size() << cv::typeToString(mask.type()) << std::endl;
    cv::bitwise_or(temp, temp, foreground, mask);
    cv::bitwise_not(mask, mask);
    cv::bitwise_or(gray, gray, background, mask);
    cv::bitwise_or(foreground, background, dst);
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
    int output_channel = channel;

    cv::Mat temp;
    int input_channel;
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
            input_channel = modifier;

            break;
        case ModifierOption::HUE:
        case ModifierOption::SAT:
        case ModifierOption::VAL:
            cv::cvtColor(src, temp, cv::COLOR_RGB2HSV_FULL);
            input_channel = modifier - 3;

            break;
    }

    std::array<cv::Mat, 3> channels;
    cv::split(temp, channels);

    cv::Mat output(src.rows, src.cols, CV_8UC3),
            empty_channel(src.rows, src.cols, CV_8UC1, cv::Scalar(0.0)),
            selected_channel = channels[input_channel];

    switch (channel) {
        case ChannelOption::ALL:
            cv::cvtColor(selected_channel, output, cv::COLOR_GRAY2RGB); //pretend to be GRAY
            break;
        case ChannelOption::R:
            {
                std::array<cv::Mat, 3> new_channels = {selected_channel, empty_channel, empty_channel};

                cv::merge(new_channels, output);
            }
            break;
        case ChannelOption::G:
            {
                std::array<cv::Mat, 3> new_channels = {empty_channel, selected_channel, empty_channel};

                cv::merge(new_channels, output);
            }
            break;
        case ChannelOption::B:
            {
                std::array<cv::Mat, 3> new_channels = {empty_channel, empty_channel, selected_channel};

                cv::merge(new_channels, output);
            }
            break;
    }

    dst = std::move(output);
}


void image_proc::compressImage(const cv::Mat& src, cv::Mat& dst, double compression_level) {
    src.copyTo(dst);

    if (compression_level == 8.0) {
        return;
    }

    // const uint8_t max_value = compression_level;
    dst.forEach<Pixel>(
        [compression_level](Pixel& pixel, const int*) -> void {
            uint8_t pixel_0 = pixel[0] * (compression_level / MAX_8BIT),
                    pixel_1 = pixel[1] * (compression_level / MAX_8BIT),
                    pixel_2 = pixel[2] * (compression_level / MAX_8BIT);

            pixel[0] = pixel_0 * MAX_8BIT / compression_level; //max_value;
            pixel[1] = pixel_1 * MAX_8BIT / compression_level; //max_value;
            pixel[2] = pixel_2 * MAX_8BIT / compression_level; //max_value;
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


std::string image_proc::getAverageColorString(const cv::Mat& image) {
    std::stringstream avg_color_string;

    cv::Scalar average = cv::mean(image);
    avg_color_string << std::fixed << std::setfill('0') << std::setprecision(2)
                     <<   "R: " << average[0]
                     << "\tG: " << average[1]
                     << "\tB: " << average[2];

    return avg_color_string.str();
}


void image_proc::convertCVtoGTK(const cv::Mat& src, Gtk::Image& dst) {
    assert(src.data != NULL);

    Glib::RefPtr<Gdk::Pixbuf> image_buffer = Gdk::Pixbuf::create_from_data(src.data, Gdk::COLORSPACE_RGB, false, 8, src.cols, src.rows, src.step);

    dst.set(image_buffer);
}