#ifndef OBJ_DETECT_H
#define OBJ_DETECT_H

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int Find_catface(Mat &img, Rect &rectCatFace);

#endif // OBJ_DETECT_H
