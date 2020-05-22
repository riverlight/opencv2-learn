#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>//SIFT
#include <opencv2/legacy/legacy.hpp>//BFMatch±©¡¶∆•≈‰

using namespace std;
using namespace cv;

void calc_mat_description(Mat imgSrc, Mat &imgDescription);
void calc_mat_description2(Mat imgSrc, vector<KeyPoint> &keyPoints, Mat &imgDescription);
float calc_struct_fp(Mat m0, Mat m1);

#endif // UTIL_H
