#ifndef ROTATE_H
#define ROTATE_H

#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

void rotate(Mat &s, float angle);
void pointAffine(Point &s, Point &d, Mat img, Mat &rotImg);
void rectAffine(Rect &rs, Rect &rd, Mat img, Mat &rotImg);
void rectAffine(Rect &rs, Rect &rd, Mat img, double angle);
void i_rotate(Mat &s, Mat &d, double angle, double scale);
void inv_rotate(Mat &s, Mat &t, double angle, double scale);

#endif // ROTATE_H
