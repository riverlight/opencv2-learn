#ifndef ACE_H
#define ACE_H

#include <stdio.h>
#include <iostream>
#include <immintrin.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace cv::ml;
using namespace std;


namespace ACE {
	Mat getACE(Mat src, int ratio, int radius);
}

#endif // ACE_H
