#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>//SIFT
#include <opencv2/legacy/legacy.hpp>//BFMatch����ƥ��

#include "tools.h"

using namespace cv;
using namespace std;


int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 4)
	{
		cout << "err input [find-qutoutiao-logo ���� logo flag]" << endl;
		return 0;
	}
	Mat srcImg1 = imread(argv[1]);
	//Mat srcImg1 = imread("logo.png");
	Mat srcImg2 = imread(argv[2]);

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
	//����������(�ؼ���)
	Mat feature_pic1, feature_pic2;
	drawKeypoints(srcImg1, keyPoints1, feature_pic1, Scalar::all(-1));
	drawKeypoints(srcImg2, keyPoints2, feature_pic2, Scalar::all(-1));
	//��ʾԭͼ
	//imshow("src1", srcImg1);
	//imshow("src2", srcImg2);
	//��ʾ���
	//imshow("feature1", feature_pic1);
	//imshow("feature2", feature_pic2);

	//���������������� / ����������ȡ
	SiftDescriptorExtractor descriptor;
	Mat description1;
	descriptor.compute(srcImg1, keyPoints1, description1);
	Mat description2;
	descriptor.compute(srcImg2, keyPoints2, description2);
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

	if (matches.size() < 12)
	{
		cout << "no-match" << endl;
		return 0;
	}

	//ƥ����ɸѡ
	nth_element(matches.begin(), matches.begin() + 10, matches.end());   //��ȡ��ǰ30�����ƥ����     
	matches.erase(matches.begin() + 11, matches.end());    //�޳��������ƥ����

	//cout << matches.size() << endl;
	vector<float> vx, vy;
	int i;
	for (i = 0; i < matches.size(); i++)
	{
		//cout << "dist : " << matches[i].distance << endl;
		//cout << "idx : " << matches[i].queryIdx << " " << matches[i].trainIdx << endl;
		//cout << "cor : " << keyPoints1[matches[i].queryIdx].pt.x << " " << keyPoints1[matches[i].queryIdx].pt.y << endl;
		//cout << "cor : " << keyPoints2[matches[i].trainIdx].pt.x << " " << keyPoints2[matches[i].trainIdx].pt.y << endl;
		vx.push_back(keyPoints1[matches[i].queryIdx].pt.x);
		vy.push_back(keyPoints1[matches[i].queryIdx].pt.y);
	}
	cout << "dist : " << matches[10].distance << endl;
	//cout << vmin(vx) << " " << vmin(vy) << " " << vmax(vx) - vmin(vx) << " " << vmax(vy)-vmin(vy) << endl;

	if (matches[10].distance < 150)
	{
		cout << vmin(vx) << " " << vmin(vy) << " " << vmax(vx) - vmin(vx) << " " << vmax(vy) - vmin(vy) << endl;
		for (int i = 0; i < 10; i++)
			cout << vx[i] << " " << vy[i] << endl;
	}

	if (argc == 4)
	{
		Mat result;
		drawMatches(srcImg1, keyPoints1, srcImg2, keyPoints2, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//ƥ����������ɫ����һ��������ɫ���
		imshow("Match_Result", result);

		waitKey(0);
	}
	
	return 0;
}
