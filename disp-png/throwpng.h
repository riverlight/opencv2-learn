#ifndef THROWPNG_H
#define THROWPNG_H


#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

void ThrowPng(Mat& imgBG, Rect r, Mat& imgPng, int angle = 0);

#endif // THROWPNG_H
