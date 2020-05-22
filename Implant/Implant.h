#ifndef IMPLANT_H
#define IMPLANT_H

#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

#include "mat-tools.h"

using namespace std;
using namespace cv;

class CImplant
{
public:
	CImplant();
	virtual ~CImplant();

public:
	int Do(Mat &bgImg, Mat &objImg);

private:
	int GetVertexes(Mat &greenMask, Point poi[4], float &angle, float &area);
	void mattingGreen(Mat &img);
	int ImplObj(Mat &bgImg, Mat &obgImg, Mat &greenMask, Point poi[4], float angle);
	void AdjustVertexes(Mat &greenMask, const Point poi[4]);

	int IsBigger(Point poiCur, Point poiBase, int pos);

	static void Copy_mask_green(Mat &dst, Mat &src, Mat &mask);
	static void SaveVertexImage(Mat &greenImg, Mat &vtxImg, const Point poi[4], int pos);
	static void SetImageEmpty(Mat &img);
	static int CountSAD(Mat &m0, Mat &m1);
	
private:
	int _nFrameCount = 0;
	float _fLastAngle, _fLastArea;
	Point _poiLast[4], _poiBase[4];
	Mat _imgBase[4];
};

#endif // IMPLANT_H
