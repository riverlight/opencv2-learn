#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "disp-png.h"
#include "throwpng.h"


using namespace cv;
using namespace std;

void roi_test()
{
	Mat imgBG;
	imgBG = imread("cat.jpg");

	Rect r;
	r.x = 566;
	r.y = 755;
	r.width = 181;
	r.height = 137;
	Mat roi = Mat(imgBG, r);
	roi = 0;

	imshow("77", imgBG);
	waitKey();
	imwrite("abc.jpg", imgBG);
}

void DispPng_test()
{
	Mat imgPng, imgPng_g;
	Mat imgBG;

	imgBG = imread("cat.jpg");
	imgPng = imread("2.png", -1);

	Rect r;
	r.x = 0;
	r.y = 0;
	r.width = imgPng.cols * 3;
	r.height = imgPng.rows * 3;
	DispPng(imgBG, r, imgPng, 60);
	imshow("77", imgBG);
	waitKey();
	imwrite("addpng.jpg", imgBG);
}

void DispPng2_test()
{
	Mat imgPng, imgPng_g;
	Mat imgBG;

	imgBG = imread("cat.jpg");
	imgPng = imread("2.png");

	Rect r;
	r.x = 0;
	r.y = 0;
	r.width = imgPng.cols * 3;
	r.height = imgPng.rows * 3;
	DispPng2(imgBG, r, imgPng);
	imshow("77", imgBG);
	waitKey();
	imwrite("addpng.jpg", imgBG);
}


void ThrowPng_test()
{
	Mat imgPng, imgPng_g;
	Mat imgBG;

	imgBG = imread("cat.jpg");
	imgPng = imread("2.png", -1);

	Rect r;
	r.x = 300;
	r.y = 300;
	r.width = imgPng.cols*3;
	r.height = imgPng.rows*3;
	ThrowPng(imgBG, r, imgPng, 0);
//	imshow("77", imgBG);
//	waitKey();
	imwrite("addpng.jpg", imgBG);
}

int main(int argc, char *argv[])
{
	ThrowPng_test();
	return 0;
}
