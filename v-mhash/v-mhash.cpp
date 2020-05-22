#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>

using namespace cv;
using namespace std;

static const int cScoreThreshold = 58;
static const int cOutputThreshold = 10;

typedef struct KeyFrame_s
{
	int *_vfp;
	float *_cm;
	int _pos = -1;
	int _bend = 0;

	float _weight = 0; // 表示代表的场景帧数
} KeyFrame;

void int2string(string &str, int n)
{
	stringstream stream;
	stream << n;
	stream >> str;
}

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

void mat2vfp(Mat img, int *vfp)
{
	Mat s0;
	cvtColor(img, s0, COLOR_RGB2GRAY);
	equalizeHist(s0, s0);
	Size s = { 8, 8 };

	Mat img8;
	resize(s0, img8, s);
	Scalar m = mean(img8);
	mat2vfp(img8, m[0], vfp);
}

int calcScore(int *v0, int *v1)
{
	int s = 0;
	for (int i = 0; i < 64; i++)
		if (v0[i] == v1[i])
			s++;

	return s;
}

double leon_pow(double x, double y)
{
	if (x > 0)
		return pow(x, y);
	else
		return -pow(-x, y);
}

float *mat2cm(Mat img)
{
	float *cm = new float[9];

	Mat s0;
	cvtColor(img, s0, COLOR_BGR2HSV);
	//s0 = img.clone();
	Scalar m = mean(s0);
	
	double sum_via[3] = { 0, 0, 0 }, sum_ske[3] = { 0, 0, 0 };
	int i, j;
	for (i = 0; i<s0.rows; i++)
	{
		uchar *p = s0.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int h, s, v;
			h = p[3 * j + 0] - m[0];
			s = p[3 * j + 1] - m[1];
			v = p[3 * j + 2] - m[2];
			sum_via[0] += h*h;
			sum_via[1] += s*s;
			sum_via[2] += v*v;
			sum_ske[0] += h*h*h;
			sum_ske[1] += s*s*s;
			sum_ske[2] += v*v*v;
		}
	}
	float via[3], ske[3];
	via[0] = leon_pow(double(sum_via[0]) / (s0.cols*s0.rows), 1 / 2.0);
	via[1] = leon_pow(double(sum_via[1]) / (s0.cols*s0.rows), 1 / 2.0);
	via[2] = leon_pow(double(sum_via[2]) / (s0.cols*s0.rows), 1 / 2.0);
	ske[0] = leon_pow(double(sum_ske[0]) / (s0.cols*s0.rows), 1 / 3.0);
	ske[1] = leon_pow(double(sum_ske[1]) / (s0.cols*s0.rows), 1 / 3.0);
	ske[2] = leon_pow(double(sum_ske[2]) / (s0.cols*s0.rows), 1 / 3.0);
	cm[0] = m[0];
	cm[1] = m[1];
	cm[2] = m[2];
	for (i = 0; i < 3; i++)
	{
		cm[i] = m[i];
		cm[3+i] = via[i];
		cm[6 + i] = ske[i];
		//cout << cm[i] << " " << cm[3 + i] << " " << cm[6 + i] << endl;
	}
	return cm;
}

bool comp(const KeyFrame &a, const KeyFrame &b){
	return a._weight > b._weight;
}

void printVFPs(vector<KeyFrame> &keys, int count)
{
	int size = keys.size();
	for (int i = 0; i < size; i++)
	{
		if (keys[i]._bend == 0)
		{
			if (i==size-1)
				keys[i]._weight = count - keys[i]._pos;
			else
			{
				if (keys[i + 1]._bend == 0)
					keys[i]._weight = keys[i + 1]._pos - keys[i]._pos;
				else
					keys[i]._weight = float(keys[i + 1]._pos - keys[i]._pos + 1) / 2;
			}
		}
		else
			keys[i]._weight = float(keys[i]._pos - keys[i - 1]._pos + 1) / 2;
	}
	sort(keys.begin(), keys.end(), comp);

	int outputsize = size;
	if (outputsize > cOutputThreshold)
		outputsize = cOutputThreshold;
	float sum_weight = 0;
	for (int i = 0; i < outputsize; i++)
		sum_weight += keys[i]._weight;
	//cout << "sum : " << sum_weight << " count : " << count << endl;

	for (int i = 0; i < outputsize; i++)
	{
		//for (int j = 0; j < 64; j++)
		//	cout << keys[i]._vfp[j];
		for (int j = 0; j < 9; j++)
		{
			cout << keys[i]._cm[j];
			if (j != 8)
				cout << ",";
		}
		cout << ":" << keys[i]._weight * 100 / sum_weight;
		if (i!=outputsize-1)
			cout << " ";
	}
}

int vmhash(char *vfile)
{
	VideoCapture cap;
	cap.open(vfile);
	if (!cap.isOpened())
	{
		//cout << "err" << endl;
		return -1;
	}

	vector<int*> vVFP;
	vector<KeyFrame> vKeys;
	int count = 0;
	Mat imgSrc, imgLast;
	while (1)
	{
		cap >> imgSrc;
		if (imgSrc.empty())
		{
			break;
		}

		int *vfp = new int[64];
		mat2vfp(imgSrc, vfp);

		if (count == 0)
		{
			KeyFrame key;
			key._vfp = vfp;
			key._pos = count;
			key._bend = 0;
			key._cm = mat2cm(imgSrc);
			vKeys.push_back(key);
		}
			
		int size = vVFP.size();
		if (size != 0)
		{
			int score = calcScore(vfp, vVFP[size - 1]);
			if (score < cScoreThreshold) // 发现场景切换
			{
				KeyFrame key;
				key._vfp = vfp;
				key._bend = 0;
				key._pos = count;
				key._cm = mat2cm(imgSrc);
				vKeys.push_back(key);
			}
		}

		vVFP.push_back(vfp);
		imgLast = imgSrc.clone();
		count++;
	}
	//cout << count << endl;
	printVFPs(vKeys, count);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		return -1;

	return vmhash(argv[1]);
}
