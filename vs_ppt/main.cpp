#include "util.h"
#include "vs_ppt.h"

using namespace std;
using namespace cv;

float g_c_fPosX[4] = { 0.125, 0.625, 0.125, 0.625 };
float g_c_fPosY[4] = { 0.125, 0.125, 0.625, 0.625 };

static bool comp_dist(const DMatch &m0, const DMatch &m1)
{
	return m0.distance < m1.distance;
}


int vs_ppt_1(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "err: vs_ppt snap-0 snap-1 snap-2" << endl;
		return -1;
	}

	Mat srcImg[3];
	for (int i = 0; i < 3; i++)
	{
		srcImg[i] = imread(argv[i + 1], IMREAD_GRAYSCALE);
		if (srcImg[i].empty())
		{
			cout << "err : input file is empty" << endl;
			return -2;
		}
		Size s;
		int base_wh = 320;
		if (srcImg[i].cols > srcImg[i].rows)
		{
			s.width = base_wh;
			s.height = srcImg[i].rows * float(base_wh) / srcImg[i].cols;
		}
		else
		{
			s.height = base_wh;
			s.width = srcImg[i].cols * float(base_wh) / srcImg[i].rows;
		}
		resize(srcImg[i], srcImg[i], s);
	}

	int matchflags[2];
	Mat description[3];
	vector<KeyPoint> keyPoints[3];
	calc_mat_description2(srcImg[0], keyPoints[0], description[0]);
	calc_mat_description2(srcImg[2], keyPoints[2], description[2]);
	Rect r(0.25 * srcImg[1].cols, 0.25 * srcImg[1].rows, srcImg[1].cols / 2, srcImg[1].rows / 2);
	calc_mat_description2(srcImg[1](r), keyPoints[1], description[1]);
	//imshow("11", srcImg[1](r));
	//waitKey();
	cout << "middle point size :  " << keyPoints[1].size() << endl;
	for (int i = 0; i < 2; i++)
	{
		//进行BFMatch暴力匹配
		BruteForceMatcher<L2<float>>matcher;    //实例化暴力匹配器
		vector<DMatch>matches;   //定义匹配结果变量
		if (i==0)
			matcher.match(description[1], description[0], matches);  //实现描述符之间的匹配
		else
			matcher.match(description[1], description[2], matches);  //实现描述符之间的匹配
		cout << "matcher size : " << matches.size() << endl;
		if (matches.size() < 4)
		{
			matchflags[i] = 0;
			continue;
		}
		
		int len = matches.size();
		sort(matches.begin(), matches.end(), comp_dist);

		for (int kk = 0; kk < 4; kk++)
		{
			cout << matches[kk].distance << endl;
			int qid = matches[kk].queryIdx;
			int tid = matches[kk].trainIdx;
			cout << i << " ** " << kk << " * " << qid << " " << tid << endl;
			cout << keyPoints[1][qid].pt.x + srcImg[1].cols/4 << "," << keyPoints[1][qid].pt.y+srcImg[1].rows/4  << endl;
			if (i==0)
				cout << keyPoints[0][tid].pt << endl;
			else
				cout << keyPoints[2][tid].pt << endl;
		}
		if (matches.size() < 4)
			continue;
		matches.erase(matches.begin() + 4, matches.end());

		Mat result;
		if (i==0)
			drawMatches(srcImg[1](r), keyPoints[1], srcImg[0], keyPoints[0], matches, result, Scalar(0, 255, 0), Scalar::all(-1));//匹配特征点绿色，单一特征点颜色随机
		else
			drawMatches(srcImg[1](r), keyPoints[1], srcImg[2], keyPoints[2], matches, result, Scalar(0, 255, 0), Scalar::all(-1));//匹配特征点绿色，单一特征点颜色随机
		imshow("Match_Result", result);
		waitKey();

#if 0
		//定义SIFT特征检测类对象
		SiftFeatureDetector siftDetector;
		//定义KeyPoint变量
		vector<KeyPoint>keyPoints1;
		vector<KeyPoint>keyPoints2;
		//特征点检测
		if (i == 0)
		{
			siftDetector.detect(srcImg[0], keyPoints1);
			siftDetector.detect(srcImg[1](r), keyPoints2);

			Mat result;
			drawMatches(srcImg[1](r), keyPoints2, srcImg[0], keyPoints1, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//匹配特征点绿色，单一特征点颜色随机
			imshow("Match_Result", result);
		}
		else
		{
			siftDetector.detect(srcImg[1](r), keyPoints1);
			siftDetector.detect(srcImg[2], keyPoints2);
			Mat result;
			drawMatches(srcImg[1](r), keyPoints1, srcImg[2], keyPoints2, matches, result, Scalar(0, 255, 0), Scalar::all(-1));//匹配特征点绿色，单一特征点颜色随机
			imshow("Match_Result", result);
		}
		
		waitKey(0);
#endif
	}
	if (matchflags[0] != 0 && matchflags[1] != 0)
		cout << 1;
	else
		cout << 0;
	//cout << matchflags[0] << "," << matchflags[1];

	return 0;
}

int vs_ppt_0(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "err: argc must be 3" << endl;
		return -1;
	}

	Mat srcImg1 = imread(argv[1]);
	Mat srcImg2 = imread(argv[2]);
	if (srcImg1.empty() || srcImg2.empty())
	{
		cout << "err : input file is empty" << endl;
		return -2;
	}
	cvtColor(srcImg1, srcImg1, CV_BGR2GRAY);
	resize(srcImg1, srcImg1, Size(srcImg1.cols / 2, srcImg1.rows / 2));
	cvtColor(srcImg2, srcImg2, CV_BGR2GRAY);
	resize(srcImg2, srcImg2, Size(srcImg2.cols / 2, srcImg2.rows / 2));

	int matchflags[4];
	Mat description2;
	calc_mat_description(srcImg2, description2);
	for (int i = 0; i < 4; i++)
	{
		Rect r(g_c_fPosX[i] * srcImg1.cols, g_c_fPosY[i] * srcImg1.rows, srcImg1.cols / 4, srcImg1.rows / 4);
		Mat zoneImg = srcImg1(r);
		Mat description1;
		calc_mat_description(zoneImg, description1);

		//cout << description1.cols << " " << description1.rows << endl;

		//进行BFMatch暴力匹配
		BruteForceMatcher<L2<float>>matcher;    //实例化暴力匹配器
		vector<DMatch>matches;   //定义匹配结果变量
		matcher.match(description1, description2, matches);  //实现描述符之间的匹配
		//cout << matches.size() << endl;
		if (matches.size() == 0)
		{
			matchflags[i] = 1;
			continue;
		}

		sort(matches.begin(), matches.end(), comp_dist);
		//for (int j = 0; j < matches.size(); j++)
		//	cout << "dist : " << j << " " << matches[j].distance << endl;
		if (matches.size() == 1)
		{
			if (matches[0].distance < 100)
				matchflags[i] = 1;
			else
				matchflags[i] = 0;
		}
		else
		{
			if (matches[1].distance < 100)
				matchflags[i] = 1;
			else
				matchflags[i] = 0;
		}
	}

	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		//cout << matchflags[i] << " ";
		count += matchflags[i];
	}

	cout << count;

	return 0;
}


int main(int argc, char *argv[])
{
	return vs_ppt_sift4(argc, argv);
	//return vs_ppt_1(argc, argv);
	//return vs_ppt_0(argc, argv);
	//return vs_ppt_2(argc, argv);
}
