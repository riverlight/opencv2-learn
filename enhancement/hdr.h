#ifndef HDR_H
#define HDR_H

#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;

void ALTMRetinex(const Mat& src, Mat& dst, bool LocalAdaptation = false, bool ContrastCorrect = true);
int hdr2(cv::Mat input_img, cv::Mat out_img);


#endif // HDR_H
