#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>//SIFT
#include <opencv2/legacy/legacy.hpp>//BFMatch暴力匹配

#include "tools.h"

using namespace cv;
using namespace std;


int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 4)
	{
		cout << "err input [find-qutoutiao-logo 封面 logo flag]" << endl;
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
	//定义SIFT特征检测类对象
	SiftFeatureDetector siftDetector;
	//定义KeyPoint变量
	vector<KeyPoint>keyPoints1;
	vector<KeyPoint>keyPoints2;
	//特征点检测
	siftDetector.detect(srcImg1, keyPoints1);
	siftDetector.detect(srcImg2, keyPoints2);
	//绘制特征点(关键点)
	Mat feature_pic1, feature_pic2;
	drawKeypoints(srcImg1, keyPoints1, feature_pic1, Scalar::all(-1));
	drawKeypoints(srcImg2, keyPoints2, feature_pic2, Scalar::all(-1));
	//显示原图
	//imshow("src1", srcImg1);
	//imshow("src2", srcImg2);
	//显示结果
	//imshow("feature1", feature_pic1);
	//imshow("feature2", feature_pic2);

	//计算特征点描述符 / 特征向量提取
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

	//进行BFMatch暴力匹配
	BruteForceMatcher<L2<float>>matcher;    //实例化暴力匹配器
	vector<DMatch>matches;   //定义匹配结果变量
	matcher.match(description1, description2, matches);  //实现描述符之间的匹配

	if (matches.size() < 12)
	{
		cout << "no-match" << endl;
		return 0;
	}

	//匹配结果筛选
	nth_element(matches.begin(), matches.begin() + 10, matches.end());   //提取出前30个最佳匹配结果     
	matches.erase(matches.begin() + 11, matches.end());    //剔除掉其余的匹配结果

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
		drawMatches(srcImg1, keyPoints1, srcImg2, keyPoints2, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//匹配特征点绿色，单一特征点颜色随机
		imshow("Match_Result", result);

		waitKey(0);
	}
	
	return 0;
}
