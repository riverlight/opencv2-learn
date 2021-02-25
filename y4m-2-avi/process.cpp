#include "process.h"

void Process(unsigned char* yuv420, int width, int height)
{
	Mat m(height + height / 2, width, CV_8UC1, (unsigned char*)yuv420);
	cvtColor(m, m, COLOR_YUV2BGR_I420);
	imshow("1", m);
	waitKey(0);
}
