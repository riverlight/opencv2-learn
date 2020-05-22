#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>
#include <vector>

using namespace cv;
using namespace std;

void mat2vfp(Mat s0, float m, int *vfp)
{
	int i, j;
	uchar *p0;
	for (i = 0; i < s0.rows; i++)
	{
		p0 = s0.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			if (float(v0)>m)
				vfp[i * 8 + j] = 1;
			else
				vfp[i * 8 + j] = 0;
		}
	}
}

void mat2vfp(Mat s0, int *vfp)
{
	cvtColor(s0, s0, COLOR_RGB2GRAY);
	//equalizeHist(s0, s0);
	Size s = { 8, 8 };

	Mat img8;
	resize(s0, img8, s);
	Scalar m = mean(img8);
	mat2vfp(img8, m[0], vfp);
}

void get_videofingerprint(char *v, vector<int*> &vVFP)
{
	VideoCapture cap;
	cap.open(v);
	if (!cap.isOpened())
	{
		cout << "err" << endl;
		return;
	}

	int count = 0;
	Mat imgSrc, imgSrc_g, img8;
	while (1)
	{
		cap >> imgSrc;
		if (imgSrc.empty())
			break;
		if (count++ % 100 != 0)
			continue;
		cvtColor(imgSrc, imgSrc_g, COLOR_RGB2GRAY);
		Size s = { 8, 8 };
		resize(imgSrc_g, img8, s);
		Scalar m = mean(img8);

		int *vfp = new int[64];
		mat2vfp(img8, m[0], vfp);
		vVFP.push_back(vfp);
	}
}

int calcScore(int *v0, int *v1)
{
	int s = 0;
	for (int i = 0; i < 64; i++)
		if (v0[i] == v1[i])
			s++;
	
	return s;
}

void calc_vsd()
{
	char *v0 = "b-0.mp4";
	char *v1 = "b-352.mp4";
	vector<int*> vvfp0, vvfp1;
	get_videofingerprint(v0, vvfp0);
	get_videofingerprint(v1, vvfp1);

	cout << "v0 size : " << vvfp0.size() << endl;
	cout << "v1 size : " << vvfp1.size() << endl;

	int maxS = 0;
	int i, j;
	for (i = 0; i < vvfp0.size(); i++)
	{
		for (j = 0; j < vvfp1.size(); j++)
		{
			int s;
			s = calcScore(vvfp0[i], vvfp1[j]);
			if (s > maxS)
				maxS = s;
		}
	}
	cout << "ÏàËÆ¶ÈÎª£º %" << (float(maxS) / 64) * 100 << endl;
}

void calc_psd()
{
	Mat m0, m1;
	m0 = imread("346a-0-37-81.jpg");
	m1 = imread("346b-0-42-85.jpg");

	int *vfp0 = new int[64];
	int *vfp1 = new int[64];
	mat2vfp(m0, vfp0);
	mat2vfp(m1, vfp1);
	cout << "\n *** \n";
	cout << calcScore(vfp0, vfp1) << endl;
}

void mat2val(Mat &m, int val[256])
{
	Size s = { 128, 128 };
	Mat m16;
	resize(m, m16, s);
	imshow("77", m16);
	waitKey();
	cvtColor(m16, m16, CV_BGR2GRAY);
	
	for (int i = 0; i < 16; i++ )
	{
		uchar *p = m16.ptr<uchar>(i);
		for (int j = 0; j < 16; j++)
		{
			val[i * 16 + j] = p[j];
//			printf("%d ", p[j]);
		}
//		printf("\n");
	}

}

#define L_abs(x) ((x)>0 ? (x) : -(x))
int calcSad_sd(int v0[256], int v1[256])
{
	int count = 0;
	int sum = 0;
	for (int i = 0; i < 256; i++)
	{
		cout << L_abs(v0[i] - v1[i]) << " " << v0[i] << " " << v1[i] << endl;
		sum += L_abs(v0[i] - v1[i]);
		if (L_abs(v0[i] - v1[i]) <= 16)
			count++;
	}
	cout << (sum+128) / 256 << endl;
	return count;
}

void calc_psad()
{
	Mat m0, m1;
	m0 = imread("346a-0-37-81.jpg");
	m1 = imread("346b-0-42-85.jpg");

	int val0[256], val1[256];
	mat2val(m0, val0);
	mat2val(m1, val1);
	cout << calcSad_sd(val0, val1) << endl;
}

int main(int argc, char *argv[])
{
	cout << "Hi , this is a video-fingerprint program!" << endl;

	//calc_psd();
	calc_psad();

	return 0;
}
