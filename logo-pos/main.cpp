#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>//SIFT
#include <opencv2/legacy/legacy.hpp>//BFMatch����ƥ��

using namespace cv;
using namespace std;

void func0()
{
	Mat f, m, e;
	f = imread("0.jpg");
	m = imread("56.jpg");
	e = imread("117.jpg");
	cvtColor(f, f, COLOR_BGR2GRAY);
	cvtColor(m, m, COLOR_BGR2GRAY);
	cvtColor(e, e, COLOR_BGR2GRAY);

	Mat d_fm, d_fe, d;
	absdiff(f, m, d_fm);
	absdiff(f, e, d_fe);
	threshold(d_fm, d_fm, 10, 255, THRESH_BINARY_INV);
	threshold(d_fe, d_fe, 10, 255, THRESH_BINARY_INV);
	d_fm.copyTo(d, d_fe);

	int i = 1;
	//Mat element = getStructuringElement(0, Size(1, 1), Point(0, 0));
	//erode(d, d, element);

	Mat element1 = getStructuringElement(0, Size(3, 3), Point(1, 1));
	dilate(d, d, element1);

	Mat src, dst;
	src = imread("0.jpg");
	src.copyTo(dst, d);
	imshow("dd", dst);
	waitKey();
	imwrite("t-0-diff.png", dst);
}

int main(int argc, char *argv[])
{
#if 1
	func0();
#else 
	Mat srcImg1 = imread("xm-nb.png");
	//Mat srcImg1 = imread("logo.png");
	Mat srcImg2 = imread("qutoutiao-LOGO.jpg");
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
	imshow("feature1", feature_pic1);
	imshow("feature2", feature_pic2);

	//���������������� / ����������ȡ
	SiftDescriptorExtractor descriptor;
	Mat description1;
	descriptor.compute(srcImg1, keyPoints1, description1);
	Mat description2;
	descriptor.compute(srcImg2, keyPoints2, description2);
	cout << description1.cols << endl;
	cout << description1.rows << endl;
	cout << description2.cols << endl;
	cout << description2.rows << endl;

	//����BFMatch����ƥ��
	BruteForceMatcher<L2<float>>matcher;    //ʵ��������ƥ����
	vector<DMatch>matches;   //����ƥ��������
	matcher.match(description1, description2, matches);  //ʵ��������֮���ƥ��
	

	//ƥ����ɸѡ
	nth_element(matches.begin(), matches.begin() + 10, matches.end());   //��ȡ��ǰ30�����ƥ����     
	matches.erase(matches.begin() + 11, matches.end());    //�޳��������ƥ����

	cout << matches.size() << endl;
	int i;
	for (i = 0; i < matches.size(); i++)
	{
		cout << "dist : " << matches[i].distance << endl;
		cout << "idx : " << matches[i].queryIdx << " " << matches[i].trainIdx << endl;
		cout << "cor : " << keyPoints1[matches[i].queryIdx].pt.x << " " << keyPoints1[matches[i].queryIdx].pt.y << endl;
		cout << "cor : " << keyPoints2[matches[i].trainIdx].pt.x << " " << keyPoints2[matches[i].trainIdx].pt.y << endl;
	}

	Mat result;
	drawMatches(srcImg1, keyPoints1, srcImg2, keyPoints2, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//ƥ����������ɫ����һ��������ɫ���
	imshow("Match_Result", result);

	waitKey(0);
#endif

	return 0;
}
