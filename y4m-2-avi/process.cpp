#include "process.h"

void Process(unsigned char* yuv420, int width, int height)
{
	//Mat m(height + height / 2, width, CV_8UC1, (unsigned char*)yuv420);
	Mat m1 = Mat(height + height / 2, width, CV_8UC1, (unsigned char*)yuv420);
	cvtColor(m1, m1, COLOR_YUV2BGR_I420);
	imshow("1", m1);
	waitKey(0);
}
