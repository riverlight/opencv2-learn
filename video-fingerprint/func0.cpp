#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>

using namespace cv;
using namespace std;

#define LABS(x) ((x)>0 ? (x) : -(x))
float sum_abs_diff_mean(Mat s0, int m)
{
	int sum = 0;

	int i, j;
	uchar *p0;
	for (i = 0; i < s0.rows; i++)
	{
		p0 = s0.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			sum += LABS(v0 - m);
		}
	}
	return sum;
}

#define LABS(x) ((x)>0 ? (x) : -(x))
float meanAbsDiffMatU8(Mat&s0, Mat s1, int nonZeroCount)
{
	int sum = 0;

	int i, j;
	uchar *p0, *p1;
	for (i = 0; i < s0.rows; i++)
	{
		p0 = s0.ptr<uchar>(i);
		p1 = s1.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			v1 = p1[j];
			sum += LABS(v0 - v1);
		}
	}
	return float(sum) / nonZeroCount;
}

void videoFeatureMat(char *v, Mat &imgFt)
{
	VideoCapture cap;
	cap.open(v);
	if (!cap.isOpened())
	{
		cout << "err" << endl;
		return;
	}
	int count = 0;

	Mat imgSrc, imgBest;
	int sadmMax = 0;
	while (1)
	{
		cap >> imgSrc;

		if (imgSrc.empty())
			break;
		Mat imgEdge, imgSrc_g, img12;
		cvtColor(imgSrc, imgSrc_g, COLOR_RGB2GRAY);

		Size s = { 16, 16 };
		resize(imgSrc_g, img12, s);

		Scalar m = mean(img12);
		int sadm = sum_abs_diff_mean(img12, int(m(0)));
		if (sadm > sadmMax)
		{
			sadmMax = sadm;
			imgSrc.copyTo(imgBest);
			img12.copyTo(imgFt);
			cout << "count : " << count << endl;
		}
		count++;
	}

	imwrite("11.jpg", imgBest);
	//imshow("best", imgBest);
	//waitKey();
}

void func0()
{
	char *v0 = "b-1.mp4";
	char *v1 = "b-352.mp4";

	Mat f0, f1;
	videoFeatureMat(v0, f0);
#if 1
	videoFeatureMat(v1, f1);

	cout << "f0 " << endl;
	cout << f0 << endl;
	cout << "f1 " << endl;
	cout << f1 << endl;
	cout << meanAbsDiffMatU8(f0, f1, 256) << endl;
#endif
}
