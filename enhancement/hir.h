#ifndef HIR_H
#define HIR_H
#include <stdio.h>
#include <iostream>
#include <immintrin.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;

void Scratch_IR(Mat& src, Mat &dst);
void AdjustColor(Mat& src, Mat& dst);
void AdjustColor_2(Mat& src, Mat& dst, int idx0, int idx1, double basePoint);


#endif // HIR_H
