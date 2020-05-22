#ifndef MAT_TOOLS_H
#define MAT_TOOLS_H

#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

#include "mat-tools.h"

using namespace std;
using namespace cv;

int CountMatNotZeroNumber(Mat m);
int CountMatNotZeroNumber(Mat m, const Point ptVert[4]);
int CountVertexArea(const Point ptVert[4]);
void CreateGreenMask(Mat &img, Mat &mask);
void DbgMat(Mat &img);

#endif // MAT_TOOLS_H
