#ifndef MAGIC_H
#define MAGIC_H

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void sketch(Mat srcImg, Mat &resultImg);
void cartoon(Mat srcImg, Mat &resultImg);
void MaxFrame(Mat  srcImg, Mat &resultImg);
void MinFrame(Mat  srcImg, Mat &resultImg);

void waterCircle(Mat &img);

#endif // MAGIC_H
