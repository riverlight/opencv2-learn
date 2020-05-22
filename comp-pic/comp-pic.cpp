#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>//SIFT
#include <opencv2/legacy/legacy.hpp>//BFMatch����ƥ��

#include "tools.h"

using namespace cv;
using namespace std;

bool comp_bydist(const DMatch &a, const DMatch &b)
{
	return a.distance < b.distance;
}

int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 4)
	{
		cout << "err input [comp-pic pic0 pic1 flag]" << endl;
		return 0;
	}
	Mat srcImg1 = imread(argv[1]);
	//Mat srcImg1 = imread("logo.png");
	Mat srcImg2 = imread(argv[2]);

	Rect r0 = { 0, 0, srcImg1.cols, srcImg1.rows/8 };
	Rect r1 = { 0, srcImg1.rows * 7 / 8 - 1, srcImg1.rows, srcImg1.rows / 8 };
#if 1
	srcImg1(r0) = Scalar::all(0);
	srcImg1(r1) = Scalar::all(0);
	srcImg2(r0) = Scalar::all(0);
	srcImg2(r1) = Scalar::all(0);
#endif

	if (srcImg1.empty() || srcImg2.empty())
	{
		cout << " err input image" << endl;
		return 0;
	}
	//����SIFT������������
	SiftFeatureDetector siftDetector;
	//����KeyPoint����
	vector<KeyPoint>keyPoints1;
	vector<KeyPoint>keyPoints2;
	//��������
	siftDetector.detect(srcImg1, keyPoints1);
	siftDetector.detect(srcImg2, keyPoints2);

	//���������������� / ����������ȡ
	SiftDescriptorExtractor descriptor;
	Mat description1;
	descriptor.compute(srcImg1, keyPoints1, description1);
	Mat description2;
	descriptor.compute(srcImg2, keyPoints2, description2);
	if (description1.rows==0 || description2.rows==0)
	{
		if (description1.rows==description2.rows)
		{
			cout << "ok";
		}
		else
			cout << "no-match ( rows no equal )";
		return 0;

	}
#if 0
	cout << description1.cols << endl;
	cout << description1.rows << endl;
	cout << description2.cols << endl;
	cout << description2.rows << endl;
#endif

	//����BFMatch����ƥ��
	BruteForceMatcher<L2<float>>matcher;    //ʵ��������ƥ����
	vector<DMatch>matches;   //����ƥ��������
	matcher.match(description1, description2, matches);  //ʵ��������֮���ƥ��

	if (matches.size() < 10)
	{
		cout << "no-match (size is low)" << endl;
		return 0;
	}
	sort(matches.begin(), matches.end(), comp_bydist);

	if (matches[9].distance > 150)
	{
		cout << "no-match (dist is long) " << matches[9].distance << endl;
	}
	else
		cout << "ok";

	for (int i = 0; i < 10; i++)
	{
		//cout << "dist : " << matches[i].distance << endl;
	}
	//cout << vmin(vx) << " " << vmin(vy) << " " << vmax(vx) - vmin(vx) << " " << vmax(vy)-vmin(vy) << endl;

	if (argc == 4)
	{
		//ƥ����ɸѡ
		nth_element(matches.begin(), matches.begin() + 10, matches.end());   //��ȡ��ǰ30�����ƥ����     
		matches.erase(matches.begin() + 11, matches.end());    //�޳��������ƥ����

		Mat result;
		drawMatches(srcImg1, keyPoints1, srcImg2, keyPoints2, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//ƥ����������ɫ����һ��������ɫ���
		imshow("Match_Result", result);

		waitKey(0);
	}

	return 0;
}
