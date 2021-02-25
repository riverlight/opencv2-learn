#ifndef PROCESS_H
#define PROCESS_H

#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void Process(unsigned char* yuv420, int width, int height);

#endif // PROCESS_H
