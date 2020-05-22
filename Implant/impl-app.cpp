#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

#include "Implant.h"

using namespace std;
using namespace cv;

void func0()
{
	Mat s = imread("88.png");
	cvtColor(s, s, COLOR_RGB2GRAY);
	Point poi[4];
	poi[0].x = 100;
	poi[0].y = 100;
	poi[1].x = 50;
	poi[1].y = 150;
	poi[2].x = 100;
	poi[2].y = 200;
	poi[3].x = 150;
	poi[3].y = 150;
	vector<vector<Point>> ctss;
	vector<Point> cts;
	int i;
	for (i = 0; i<4; i++)
		cts.push_back(poi[i]);
	ctss.push_back(cts);

	cout << contourArea(cts) << endl;

	Mat imgHull = Mat::zeros(s.size(), CV_8UC1);
	drawContours(s, ctss, 0, CV_RGB(0, 0, 0), CV_FILLED, 4, vector<Vec4i>(), 0, Point());//»æÖÆÍ¹°ü
	imshow("88", s);
	waitKey();
	cout << CountMatNotZeroNumber(s) << endl;
}

int main(int argc, char *argv[])
{
	//func0();
	//return 0;
	if (argc != 4)
	{
		cout << "Implant [bg-file] [obj-file] [output-file]" << endl;
		return -1;
	}
	char *bgfile = argv[1];
	char *objfile = argv[2];
	char *outfile = argv[3];

	CImplant *impl = new CImplant();
	
	VideoCapture inputBG(bgfile);
	VideoCapture inputObj(objfile);
	if (!inputBG.isOpened() || !inputObj.isOpened())
	{
		cout << "open input failed" << endl;
		return -1;
	}

	Size s_bg = Size(inputBG.get(CV_CAP_PROP_FRAME_WIDTH), inputBG.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open(outfile, k, inputBG.get(CV_CAP_PROP_FPS), s_bg))
		return -1;

	Mat bgImg, objImg;
	while (1)
	{
		inputObj >> objImg;
		if (objImg.empty())
			break;
		inputBG >> bgImg;
		if (bgImg.empty())
			break;
		impl->Do(bgImg, objImg);
		cout << ". " << endl;
		outputV.write(bgImg);
	}

	delete impl;

	return 0;
}
