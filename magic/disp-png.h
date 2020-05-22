#ifndef DISP_PNG_H
#define DISP_PNG_H

#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;


void DispPng(Mat& imgBG, Rect r, Mat& imgPng, int angle=0);
void DispPng2(Mat& imgBG, Rect r, Mat& imgPng);
void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
	cv::Mat &output, cv::Point2i location);

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
	cv::Point2i location);
void overlayImage2(const cv::Mat &foreground,
	cv::Mat &output, cv::Point2i location);

#endif // DISP_PNG_H
