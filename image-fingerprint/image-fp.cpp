#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>
#include <vector>
#include "disp-png.h"


using namespace cv;
using namespace std;

void mat2fp(Mat s8x8, float mean_m, int *fp)
{
	int i, j;
	uchar *p0;
	for (i = 0; i < s8x8.rows; i++)
	{
		p0 = s8x8.ptr<uchar>(i);
		for (j = 0; j < s8x8.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			if (float(v0)>mean_m)
				fp[i * 8 + j] = 1;
			else
				fp[i * 8 + j] = 0;
		}
	}
}

void image2fp(Mat m, int fp[64])
{
	Mat m_g;
	cvtColor(m, m_g, COLOR_RGB2GRAY);

	Mat s8x8;
	Size s = { 8, 8 };
	resize(m_g, s8x8, s);

	Scalar m_mean = mean(s8x8);
	mat2fp(s8x8, m_mean[0], fp);
}

void image_phash(Mat m, int fp[64])
{
	Mat m_g;
	cvtColor(m, m_g, COLOR_RGB2GRAY);

	Mat s8x8, s32;
	Size s = { 32, 32 };
	resize(m_g, m_g, s);

	Mat m_g_f = Mat_<double>(m_g);
	cv::dct(m_g_f, s32);
	
	Rect r(0, 0, 8, 8);
	s8x8 = s32(r);

	Scalar m_mean = mean(s8x8);
	mat2fp(s8x8, m_mean[0], fp);
}

int calcScore(int *v0, int *v1)
{
	int s = 0;
	for (int i = 0; i < 64; i++)
		if (v0[i] == v1[i])
			s++;

	return s;
}

int main(int argc, char *argv[])
{
#if 0
	Mat logoimg = imread("d://gxhy//gaoxiao.png", -1);
	Mat bigimg = imread("d://gxhy//big-logo.png", -1);

	VideoCapture inputBG("d://gxhy//train.mp4");

	Size s_bg = Size(inputBG.get(CV_CAP_PROP_FRAME_WIDTH), inputBG.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open("d://gxhy//first.avi", k, inputBG.get(CV_CAP_PROP_FPS), s_bg))
		return -1;

	Mat bgimg;
	inputBG>>bgimg;
	bgimg /= 2;

	Rect r(210, 275, 1500, 450);
	DispPng(bgimg, r, logoimg);
	//resize(logoimg, logoimg, Size(r.width, r.height));
	//logoimg.copyTo(bgimg(r));

	Rect rr(50, 50, 200, 40);
	DispPng(bgimg, rr, bigimg);

	int count = 0;
	while (1)
	{
		cout << ".." << endl;
		if (count == 200)
			break;
		outputV.write(bgimg);
		count++;
	}
	return 0;
#else
	if (argc != 3)
	{
		cout << "err: image-fingerprint [image-0] [imag-1]" << endl;
		return -1;
	}

	Mat m0, m1;
	m0 = imread(argv[1]);
	m1 = imread(argv[2]);
	if (m0.empty() || m1.empty())
	{
		cout << "err: open image-0 or image-1 failed!" << endl;
		return -2;
	}

	void (*func)(Mat, int[64]);
	func = image_phash;
	//func = image2fp;

	int fp0[64], fp1[64];
	func(m0, fp0);
	func(m1, fp1);

	int score = calcScore(fp0, fp1);
	//cout << "ÏàËÆ¶ÈÎª£º %" << (float(score) / 64) * 100 << endl;
	cout << (float(score) / 64) * 100;
	return 0;
#endif
}

