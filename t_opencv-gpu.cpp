// \see http://stackoverflow.com/questions/3770939/simple-frame-by-frame-video-decoder-library

#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"

int
test_opencv_videocapture(int argc, char **argv)
{
    cv::VideoCapture capture(argv[1]);
    if (capture.grab()) {
        cv::Mat_<char> frame;
        capture.retrieve(frame);
    }
    return 0;
}

int main (int argc, char* argv[])
{
    try {
        cv::Mat src_host = cv::imread("file.png", CV_LOAD_IMAGE_GRAYSCALE);

        cv::gpu::GpuMat dst, src;
        src.upload(src_host);
        cv::gpu::threshold(src, dst, 128.0, 255.0, CV_THRESH_BINARY);
        cv::Mat result_host = dst;

        cv::imshow("Result", result_host);
        cv::waitKey();
    } catch(const cv::Exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return 0;
}
