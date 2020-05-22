#include "mat-tools.h"

int CountMatNotZeroNumber(Mat m)
{
	int count = 0;
	uchar *p;
	int i, j;
	for (i = 0; i < m.rows; i++)
	{
		p = m.ptr<uchar>(i);
		for (j = 0; j < m.cols; j++)
		{
			if (p[j] != 0)
				count += 1;
		}
	}
	return count;
}

int CountMatNotZeroNumber(Mat m, const Point ptVert[4])
{
	Mat mc = m.clone();
	vector<vector<Point>> ctss;
	vector<Point> cts;
	int i;
	for (i = 0; i<4; i++)
		cts.push_back(ptVert[i]);
	ctss.push_back(cts);
	drawContours(mc, ctss, 0, CV_RGB(0, 0, 0), CV_FILLED, 4, vector<Vec4i>(), 0, Point());//»æÖÆÍ¹°ü
	return CountMatNotZeroNumber(mc);
}

int CountVertexArea(const Point ptVert[4])
{
	vector<Point> cts;
	int i;
	for (i = 0; i<4; i++)
		cts.push_back(ptVert[i]);
	return contourArea(cts);
}

void CreateGreenMask(Mat &img, Mat &mask)
{
	mask = Mat::zeros(img.rows, img.cols, CV_8UC1) + 255;

	int thres = 10;
	int i, j;
	uchar *p;
	uchar *m;
	for (i = 0; i < img.rows; i++)
	{
		p = img.ptr<uchar>(i);
		m = mask.ptr<uchar>(i);
		for (j = 0; j < img.cols; j++)
		{
			int b, g, r;
			b = p[3 * j + 0];
			g = p[3 * j + 1];
			r = p[3 * j + 2];
			if ((g - b < thres) || (g - r < thres))
			{
				m[j] = 0;
			}
		}
	}
}

void DbgMat(Mat &img)
{
	Mat img1 = img.clone();

	resize(img1, img1, Size(img1.cols / 2, img1.rows / 2));
	imshow("88", img1);
	waitKey();
}
