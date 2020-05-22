#include <math.h>

#include "util.h"
#include "input.h"
#include "vs_ppt.h"

using namespace cv;
using namespace std;

static float calc_mean_diff(Mat m0, Mat m1);


int readImage(Mat &img, char *szName)
{
	img = imread(szName, IMREAD_GRAYSCALE);
	if (img.empty())
	{
		cout << "err : input file is empty" << endl;
		return -2;
	}
	Size s;
	int base_wh = 320;
	if (img.cols > img.rows)
	{
		s.width = base_wh;
		s.height = img.rows * float(base_wh) / img.cols;
	}
	else
	{
		s.height = base_wh;
		s.width = img.cols * float(base_wh) / img.rows;
	}
	resize(img, img, s);
	return 0;
}

static bool comp_dist(const DMatch &m0, const DMatch &m1)
{
	return m0.distance < m1.distance;
}

int getMatchPoints(Mat img[2], Point2f points[2][4])
{
	Mat description[2];
	vector<KeyPoint> keyPoints[2];
	Rect r(0.25 * img[0].cols, 0.25 * img[0].rows, img[0].cols / 2, img[0].rows / 2);
	calc_mat_description2(img[0](r), keyPoints[0], description[0]);
	calc_mat_description2(img[1], keyPoints[1], description[1]);
	if ((keyPoints[0]).size() < 4 || (keyPoints[1]).size() < 4)
	{
		for (int i = 0; i < 2; i++)
		{
			points[i][0].x = r.x;
			points[i][0].y = r.y;
			points[i][1].x = r.x + r.width;
			points[i][1].y = r.y;
			points[i][2].x = r.x + r.width;
			points[i][2].y = r.y + r.height;
			points[i][3].x = r.x;
			points[i][3].y = r.y + r.height;
		}
		return 0;
	}

	//进行BFMatch暴力匹配
	BruteForceMatcher<L2<float>>matcher;    //实例化暴力匹配器
	vector<DMatch>matches;   //定义匹配结果变量
	matcher.match(description[0], description[1], matches);  //实现描述符之间的匹配
	if (matches.size() < 4)
	{
		for (int i = 0; i < 2; i++)
		{
			points[i][0].x = r.x;
			points[i][0].y = r.y;
			points[i][1].x = r.x + r.width;
			points[i][1].y = r.y;
			points[i][2].x = r.x + r.width;
			points[i][2].y = r.y + r.height;
			points[i][3].x = r.x;
			points[i][3].y = r.y + r.height;
		}
		return 0;
	}

	int len = matches.size();
	sort(matches.begin(), matches.end(), comp_dist);

	for (int i = 0; i < 4; i++)
	{
		int qid = matches[i].queryIdx;
		int tid = matches[i].trainIdx;
		points[0][i].x = keyPoints[0][qid].pt.x + img[0].cols / 4;
		points[0][i].y = keyPoints[0][qid].pt.y + img[0].rows / 4;
		points[1][i].x = keyPoints[1][tid].pt.x;
		points[1][i].y = keyPoints[1][tid].pt.y;
	}
	return 0;
}

float find_min(Point2f poi[4], int bX)
{
	float fMin = 10000;
	if (bX != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (poi[i].x < fMin)
				fMin = poi[i].x;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (poi[i].y < fMin)
				fMin = poi[i].y;
		}
	}
	return fMin;
}

float find_max(Point2f poi[4], int bX)
{
	float fMax = -1;
	if (bX != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (poi[i].x > fMax)
				fMax = poi[i].x;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (poi[i].y > fMax)
				fMax = poi[i].y;
		}
	}
	return fMax;
}

int calc_target_rect(Point2f srcPoi[4], Point2f dstPoi[4], Rect &srcR, Rect &dstR, int w, int h)
{
	Point2f baseSrc, baseDst;
	float w_src, w_dst, h_src, h_dst;
	baseSrc.x = find_min(srcPoi, 1);
	baseSrc.y = find_min(srcPoi, 0);
	w_src = find_max(srcPoi, 1) - baseSrc.x;
	h_src = find_max(srcPoi, 0) - baseSrc.y;
	baseDst.x = find_min(dstPoi, 1);
	baseDst.y = find_min(dstPoi, 0);
	w_dst = find_max(dstPoi, 1) - baseDst.x;
	h_dst = find_max(dstPoi, 0) - baseDst.y;

	float w_scaler = 1.0;
	if (w_src > 0.1)
		w_scaler = w_dst / w_src;
	dstR.width = srcR.width*w_scaler;
	dstR.x = baseDst.x - (baseSrc.x - srcR.x)*w_scaler + 0.5;

	float h_scaler = 1.0;
	if (h_src > 0.1)
		h_scaler = h_dst / h_src;
	dstR.height = srcR.height*h_scaler;
	dstR.y = baseDst.y - (baseSrc.y - srcR.y)*w_scaler + 0.5;

	return 0;
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

void mat2vfp(Mat s0, int *vfp)
{
	//cvtColor(s0, s0, COLOR_RGB2GRAY);
	//equalizeHist(s0, s0);
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

int calc_psd(Mat m0, Mat m1)
{
	
	int *vfp0 = new int[64];
	int *vfp1 = new int[64];
	mat2vfp(m0, vfp0);
	mat2vfp(m1, vfp1);
	return calcScore(vfp0, vfp1);
}

int vs_ppt_sift4(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "err: vs_ppt pic-0 pic-1" << endl;
		return -1;
	}

	// 准备数据
	Mat srcImg[2];
	for (int i = 0; i < 2; i++)
	{
		if (readImage(srcImg[i], argv[1 + i]) != 0)
		{
			cout << "err : open pic failed" << endl;
			return -2;
		}
	}
	Rect r(0.25 * srcImg[0].cols, 0.25 * srcImg[0].rows, srcImg[0].cols / 2, srcImg[0].rows / 2);

	// 计算 sift 和匹配点
	Point2f matchPoints[2][4];
	getMatchPoints(srcImg, matchPoints);
	for (int i = 0; i < 4; i++)
	{
		//circle(srcImg[0], matchPoints[0][i], 5, Scalar(255, 0, 0), -1);
		//circle(srcImg[1], matchPoints[1][i], 5, Scalar(255, 0, 255), -1);
		//cout << matchPoints[0][i] << matchPoints[1][i] << endl;
	}
	//imshow("11", srcImg[0]);
	//imshow("111", srcImg[1]);
	//waitKey();
	//exit(0);

	// 计算缩放率
	Rect dstR;
	calc_target_rect(matchPoints[0], matchPoints[1], r, dstR, srcImg[0].cols, srcImg[0].rows);
	//cout << r << dstR << endl;
	//exit(0);

	// 计算sad 区域
	if (dstR.x < 0 || dstR.y<0 || (dstR.x+dstR.width)>srcImg[0].cols || (dstR.y+dstR.height)>srcImg[0].rows)
	{
		cout << 0;
		return 0;
	}

	// 缩放和 sad
	Mat tmpImg = srcImg[1](dstR), dstImg;
	resize(tmpImg, dstImg, Size(r.width, r.height));
	float scaler_factor = float(tmpImg.cols) / r.width;
	if (scaler_factor < 1.0)
		scaler_factor = 1.0 / scaler_factor;
	//cout << "scaler : " << scaler_factor << endl;

	// 评价是否是 ppt
	float s_fp_score = calc_struct_fp(srcImg[0](r), dstImg);
	//cout << " s_fp_score : " << s_fp_score << endl;

	int score = calc_psd(srcImg[0](r), dstImg);
	
	float mean_diff = calc_mean_diff(srcImg[0](r), dstImg);

#if 0
	cout << "score : " << score << endl;
	cout << mean_diff << endl;
	imshow("11", srcImg[0](r));
	imshow("111", dstImg);
	waitKey();
	
#endif

	if (mean_diff<16.0*scaler_factor*scaler_factor && score >= 58.0 / sqrt(scaler_factor) && s_fp_score>0.90 / sqrt(scaler_factor))
		cout << 1;
	else
		cout << 0;
	
	//exit(0);

	
	

	return 0;
}

static float calc_diff_ratio(Mat &m0, Mat &m1)
{
	return 1.0;
}

#define ABS(x) ((x)>0 ? (x) : -(x))
float calc_mean_diff(Mat m0, Mat m1)
{
	int notzero_count = 0;
	int count = 0;
	int sum_diff = 0;
	for (int i = 0; i < m0.rows; i++)
	{
		for (int j = 0; j < m0.cols; j++)
		{
			int diff = m0.at<uchar>(i, j) - m1.at<uchar>(i, j);
			if ((diff>20) || (diff<-20))
				count++;

			if (ABS(diff) > 8)
			{
				sum_diff += ABS(diff);
				
			}
			notzero_count++;
		}
	}
	//cout << count << " " << notzero_count << endl;
	//cout << double(sum_diff) / notzero_count << endl;
	return (double(sum_diff) / (notzero_count));
	//return float(count) / (notzero_count+1);
}

float ppt_possibility(CInputVideo *pVideo, float fTS)
{
	int nTS = fTS * 1000;
	Mat img0, img1, imgT;
	if (nTS - 60 < 0)
		return 0;

	float fps = pVideo->Get_FrameRate();
	//cout << fps << endl;
	int interval = 300;

	if (pVideo->Get_CurrentMat(imgT, 0, 3600 * 1000, nTS - interval) != 0)
		return 0;
	img0 = imgT.clone();
	//resize(img0, img0, Size(img0.cols / 2, img0.rows / 2));
	cvtColor(img0, img0, CV_BGR2GRAY);
	Canny(img0, img0, 50, 100, 3);
	
	if (pVideo->Get_CurrentMat(imgT, 0, 3600 * 1000, nTS + interval) != 0)
		return 0;
	img1 = imgT.clone();
	//resize(img1, img1, Size(img1.cols / 2, img1.rows / 2));
	cvtColor(img1, img1, CV_BGR2GRAY);
	Canny(img1, img1, 50, 100, 3);

	imshow("11", img0);
	imshow("21", img1);
	waitKey();

	
	//imwrite("d:\\0.jpg", img0);
	//imwrite("d:\\1.jpg", img1);
	//exit(0);

	Mat roi0 = img0(Rect(img0.cols / 8, img0.rows / 8, img0.cols*0.75, img0.rows*0.3));
	float min_ratio = 1;
	if (img0.cols < 80 || img0.rows < 80)
	{
		Mat roi1 = img1(Rect(img1.cols / 8, img1.rows / 8, img1.cols * 0.75, img1.rows * 0.3));
		float ratio = calc_diff_ratio(roi0, roi1);
		min_ratio = ratio;
	}
	else
	{
		for (int i = -8; i<8; i++)
		{
			for (int j = -8; j < 8; j++)
			{
				Mat roi1 = img1(Rect(img1.cols / 8 + j, img1.rows / 8 + i, img1.cols * 0.75, img1.rows * 0.3));
				float ratio = calc_diff_ratio(roi0, roi1);
				if (ratio < min_ratio)
				{
					min_ratio = ratio;
					//cout << i << " " << j << " " << min_ratio << endl;
				}
					
			}
		}
	}

	if (min_ratio > 0.1)
	{
		//imwrite("d:\\0.jpg", img0);
		//imwrite("d:\\1.jpg", img1);
		//exit(0);
	}

	cout << "ratio : " << min_ratio << endl;

	if (min_ratio < 0.04)
		return 1;
	else if (min_ratio < 0.1)
		return 0;
	else if (min_ratio < 0.2)
		return -0.5;
	else
		return -1;
}

int vs_ppt_2(int argc, char *argv[])
{
	if (argc <= 2)
	{
		cout << "err: no argv" << endl;
		return -1;
	}
	CInputVideo *pVideo;
	pVideo = new CInputVideo();
	if (pVideo->Open(argv[1]) != 0)
	{
		return -2;
	}

	vector<float> vSceneTS;
	for (int i = 2; i < argc; i++)
	{
		//cout << argv[i] << endl;
		vSceneTS.push_back(atof(argv[i]));
	}
	
	sort(vSceneTS.begin(), vSceneTS.end());
	float scene_ppt_count = 0;
	for (int i = 0; i < vSceneTS.size(); i++)
	{
		float fPossibility = ppt_possibility(pVideo, vSceneTS[i]);
		scene_ppt_count += fPossibility;
		//cout << vSceneTS[i] << endl;
	}
	if (scene_ppt_count < 0)
		scene_ppt_count = 0;
	//cout << scene_ppt_count << endl;
	cout << float(scene_ppt_count)/(argc-2) << endl;

	return 0;
}
