#ifndef BLINK_H
#define BLINK_H
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "math.h"  

using namespace std;
using namespace cv;

void blink(Mat& imgIn, Point center, int w, int h, float strength);

#endif // BLINK_H
