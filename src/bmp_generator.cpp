#include <opencv2/opencv.hpp>

#include "macros.hpp"

typedef cv::Vec<uint8_t, NR_CHANNELS> Pixel;
int main() {
    cv::Mat image(STD_PREVIEW_HEIGHT, STD_PREVIEW_WIDTH, CV_MAKETYPE(CV_8U, NR_CHANNELS), cv::Scalar(0.0, 0.0, 0.0, 0.0)),
            temp;

    for (size_t i = 0ul; i < NR_CHANNELS; i++) {
        image.copyTo(temp);

        temp.forEach<Pixel>(
            [i](Pixel& pixel, const int position[2]) {
                // position = {row, column};
                pixel[i] = 255u - position[0];
            }
        );

        std::string filename = "../resources/channel_" + std::to_string(i) + ".bmp";
        if (cv::imwrite(filename, temp)) {
            std::cout << i + 1 << '/' << NR_CHANNELS << " complete\r" << std::flush;
        } else {
            std::cout << "Unable to save file: " << filename << std::endl;

            return 1;
        }
    }
    std::cout << '\n';

    return 0;
}