#ifndef ZOOMOUT_H
#define ZOOMOUT_H
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "math.h"  

using namespace std;
using namespace cv;

int zoomout(Mat &img, float alpha);

#endif // ZOOMOUT_H
