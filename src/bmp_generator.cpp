#include <opencv2/opencv.hpp>

#include "macros.hpp"
#include "color_spaces.hpp"

const std::array<const std::array<const double, NR_CHANNELS>, NR_COLOR_SPACES> color_space_base_layouts {{
    {{0.0, 0.0, 0.0}},
    {{0.0, 0.0, 0.0}},          {{0.0, 0.0, 0.0}},          {{0.0, 0.0, 0.0}},          {{0.5, 0.5, 0.5}},          {{0.5, 0.5, 0.5}},
    /*                          Not the best solution for HSV/HLS but better not to complex either                                  */
    {{0.5, 0.5, 0.5}},          {{0.5, 0.5, 0.5}},          {{0.5, 0.5, 0.5}},          {{0.5, 0.5, 0.5}},          {{0.0, 0.0, 0.0}},
    {{0.0, 0.0, 0.0}}
}};
const std::string default_save_location = "../resources/";


typedef cv::Vec<uint8_t, 1ul> Pixel;
cv::Mat createRangedChannel() {
    cv::Mat channel(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_8UC1, cv::Scalar(0.0));

    channel.forEach<Pixel>(
        [](Pixel& pixel, const int position[2]) {
            pixel[0] = STD_PREVIEW_HEIGHT - position[0] - 1u;
        }
    );

    return channel;
}

int main() {
    const cv::Mat ranged_channel = std::move(createRangedChannel());
    cv::Mat single_value_channel(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_8UC1),
            output;
    std::array<cv::Mat, NR_CHANNELS> channels = {
        cv::Mat(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_8UC1),
        cv::Mat(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_8UC1),
        cv::Mat(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_8UC1),
    };

    cv::ColorConversionCodes conversion_code;
    std::string save_location_base_name, save_location;
    cv::Mat channel_backup;
    for (size_t i = 0ul; i < NR_COLOR_SPACES; i++) {
        conversion_code = image_proc::convert_to_rgb[i];
        save_location_base_name = default_save_location + image_proc::color_space_names[i] + '_';

        if (image_proc::color_space_nr_channels[i] == 1ul) {
            if (conversion_code != cv::COLOR_COLORCVT_MAX) {
                cv::cvtColor(ranged_channel, output, conversion_code);
            } else {
                ranged_channel.copyTo(output);
            }

            //appending the channel is unnecessary but makes it easier to streamline loading them further down the line
            save_location = save_location_base_name + image_proc::color_space_channels[i][0] + ".bmp";
            if (!cv::imwrite(save_location, output)) {
                std::cerr << "Unable to save file " << save_location << ". Skipping." << std::endl;

                continue;
            }
        } else {
            channels[0].setTo(cv::Scalar(255.0 * color_space_base_layouts[i][0]));
            channels[1].setTo(cv::Scalar(255.0 * color_space_base_layouts[i][1]));
            channels[2].setTo(cv::Scalar(255.0 * color_space_base_layouts[i][2]));

            for (size_t channel_idx = 0ul; channel_idx < NR_CHANNELS; channel_idx++) {
                channel_backup = channels[channel_idx];
                channels[channel_idx] = ranged_channel;

                cv::merge(channels, output);
                if (conversion_code != cv::COLOR_COLORCVT_MAX) {
                    cv::cvtColor(output, output, conversion_code);
                }

                save_location = save_location_base_name + image_proc::color_space_channels[i][channel_idx] + ".bmp";
                if (!cv::imwrite(save_location, output)) {
                    std::cerr << "Unable to save file " << save_location << ". Skipping." << std::endl;

                    continue;
                }

                channels[channel_idx] = channel_backup;
            }
        }
    }

    return 0;
}