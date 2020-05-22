#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


class Histogram1D
{
private:
	int histSize[1]; // 项的数量
	float hranges[2]; // 统计像素的最大值和最小值
	const float* ranges[1];
	int channels[1]; // 仅计算一个通道

public:
	Histogram1D()
	{
		// 准备1D直方图的参数
		histSize[0] = 64;
		hranges[0] = 0.0f;
		hranges[1] = 256.0f;
		ranges[0] = hranges;
		channels[0] = 0;
	}

	MatND getHistogram(const Mat &image)
	{
		MatND hist;
		// 计算直方图
		calcHist(&image,// 要计算图像的
			1,                // 只计算一幅图像的直方图
			channels,        // 通道数量
			Mat(),            // 不使用掩码
			hist,            // 存放直方图
			1,                // 1D直方图
			histSize,        // 统计的灰度的个数
			ranges);        // 灰度值的范围
		return hist;
	}

	Mat getHistogramImage(const Mat &image)
	{
		MatND hist = getHistogram(image);

		// 最大值，最小值
		double maxVal = 0.0f;
		double minVal = 0.0f;

		minMaxLoc(hist, &minVal, &maxVal);

		//显示直方图的图像
		Mat histImg(histSize[0], histSize[0], CV_8U, Scalar(255));

		// 设置最高点为nbins的90%
		int hpt = static_cast<int>(0.9 * histSize[0]);
		//每个条目绘制一条垂直线
		for (int h = 0; h < histSize[0]; h++)
		{
			float binVal = hist.at<float>(h);
			int intensity = static_cast<int>(binVal * hpt / maxVal);
			// 两点之间绘制一条直线
			line(histImg, Point(h, histSize[0]), Point(h, histSize[0] - intensity), Scalar::all(0));
		}
		return histImg;
	}

	void mat2GrayHist(Mat &image, int grayHist[64])
	{
		Mat image_g;
		cvtColor(image, image_g, CV_BGR2GRAY);
		equalizeHist(image_g, image_g);
		int sum = 0;
		Mat hist = getHistogram(image_g);
		for (int g = 0; g < 64; g++)
		{
			grayHist[g] = hist.at<float>(g);
			sum += grayHist[g];
		}
		//cout << "sum : " << sum << endl;
	}

};



class CalcHistogram
{
private:
	int histSize[3];         //直方图项的数量
	float hranges[2];        //h通道像素的最小和最大值
	float sranges[2];
	float vranges[2];
	const float *ranges[3];  //各通道的范围
	int channels[3];         //三个通道
	int dims;

public:
	CalcHistogram(int hbins = 4, int sbins = 4, int vbins = 4)
	{
		histSize[0] = hbins;
		histSize[1] = sbins;
		histSize[2] = vbins;
		hranges[0] = 0; hranges[1] = 180;
		sranges[0] = 0; sranges[1] = 256;
		vranges[0] = 0; vranges[1] = 256;
		ranges[0] = hranges;
		ranges[1] = sranges;
		ranges[2] = vranges;
		channels[0] = 0;
		channels[1] = 1;
		channels[2] = 2;
		dims = 3;
	}

	Mat getHistogram(const Mat &image);
	void getHistogramImage(const Mat &image);
	void mat2HSVHist(Mat &image, float hsvHist[64]);
};

Mat CalcHistogram::getHistogram(const Mat &image)
{
	Mat hist;
	calcHist(&image,
		1,
		channels,
		Mat(),
		hist,
		dims,
		histSize,
		ranges,
		true,      //直方图每一维的histSize是均匀的
		false
		);

	return hist;
}

void CalcHistogram::mat2HSVHist(Mat &image, float hsvHist[64])
{
	cvtColor(image, image, CV_BGR2HSV);
	Mat hist = getHistogram(image);
	int sum = 0;
	for (int v = 0; v < 4; v++)
	{
		for (int s = 0; s < 4; s++)
		{
			for (int h = 0; h < 4; h++)
			{
				hsvHist[v * 16 + s * 4 + h] = hist.at<float>(h, s, v);
				sum += hsvHist[v * 16 + s * 4 + h];
				cout << hsvHist[v * 16 + s * 4 + h] << " ";
			}
		}
	}
	cout << "\n" << sum << endl;
}

void CalcHistogram::getHistogramImage(const Mat &image)
{
	Mat hist = getHistogram(image);
	int scale = 4;
	int hbins = histSize[0];
	int sbins = histSize[1];
	int vbins = histSize[2];
	float *hist_sta = new float[sbins];
	float *hist_val = new float[vbins];
	float *hist_hue = new float[hbins];
	memset(hist_val, 0, vbins * sizeof(float));
	memset(hist_sta, 0, sbins * sizeof(float));
	memset(hist_hue, 0, hbins * sizeof(float));

	for (int s = 0; s < sbins; s++)
	{
		for (int v = 0; v < vbins; v++)
		{
			for (int h = 0; h<hbins; h++)
			{
				float binVal = hist.at<float>(h, s, v);
				cout << binVal << " ";
				hist_hue[h] += binVal;
				hist_val[v] += binVal;
				hist_sta[s] += binVal;
			}
		}
	}
#if 0
	for (int s = 0; s < sbins; s++)
		cout << hist_sta[s] << " ";
	for (int v = 0; v < vbins; v++)
		cout << hist_val[v] << " ";
	for (int h = 0; h < hbins; h++)
		cout << hist_hue[h] << " ";
#endif

	double max_sta = 0, max_val = 0, max_hue = 0;
	for (int i = 0; i<sbins; ++i)
	{
		if (hist_sta[i]>max_sta)
			max_sta = hist_sta[i];
	}
	for (int i = 0; i<vbins; ++i)
	{
		if (hist_val[i]>max_val)
			max_val = hist_val[i];
	}
	for (int i = 0; i<hbins; ++i)
	{
		if (hist_hue[i]>max_hue)
			max_hue = hist_hue[i];
	}

	Mat sta_img = Mat::zeros(240, sbins*scale + 20, CV_8UC3);
	Mat val_img = Mat::zeros(240, vbins*scale + 20, CV_8UC3);
	Mat hue_img = Mat::zeros(240, hbins*scale + 20, CV_8UC3);

	for (int i = 0; i<sbins; ++i)
	{
		int intensity = cvRound(hist_sta[i] * (sta_img.rows - 10) / max_sta);
		rectangle(sta_img, Point(i*scale + 10, sta_img.rows - intensity), Point((i + 1)*scale - 1 + 10, sta_img.rows - 1), Scalar(0, 255, 0), 1);
	}
	for (int i = 0; i<vbins; ++i)
	{
		int intensity = cvRound(hist_val[i] * (val_img.rows - 10) / max_val);
		rectangle(val_img, Point(i*scale + 10, val_img.rows - intensity), Point((i + 1)*scale - 1 + 10, val_img.rows - 1), Scalar(0, 0, 255), 1);
	}
	for (int i = 0; i<hbins; ++i)
	{
		int intensity = cvRound(hist_hue[i] * (hue_img.rows - 10) / max_hue);
		rectangle(hue_img, Point(i*scale + 10, hue_img.rows - intensity), Point((i + 1)*scale - 1 + 10, hue_img.rows - 1), Scalar(255, 0, 0), 1);
	}

	imshow("Shist", sta_img);
	imshow("Vhist", val_img);
	imshow("Hhist", hue_img);

	delete[] hist_sta;
	delete[] hist_val;
	delete[] hist_hue;
}

float calcCosSimi(int h0[], int h1[], int size=64)
{
	int ss0 = 0, ss1 = 0;
	int hh = 0;
	for (int i = 0; i < size; i++)
	{
		ss0 += h0[i] * h0[i];
		ss1 += h1[i] * h1[i];
		hh += h0[i] * h1[i];
	}
	float s0 = sqrt(ss0);
	float s1 = sqrt(ss1);
	cout << float(hh) / (s0*s1) << endl;
	return float(hh) / (s0*s1);
}

#if 1
float calcCosSimi(float h0[64], float h1[64])
{
	int ss0 = 0, ss1 = 0;
	int hh = 0;
	for (int i = 0; i < 64; i++)
	{
		ss0 += h0[i] * h0[i];
		ss1 += h1[i] * h1[i];
		hh += h0[i] * h1[i];
	}
	float s0 = sqrt(ss0);
	float s1 = sqrt(ss1);
	cout << float(hh) / (s0*s1) << endl;
	return float(hh) / (s0*s1);
}

void Mat_2_structfp(Mat &m, int structfp[48 * 48])
{
	Mat m_48;
	if (m.cols != 48 || m.rows != 48)
		resize(m, m_48, Size(48, 48));
	else
		m_48 = m.clone();
	cvtColor(m_48, m_48, CV_BGR2GRAY);

	Mat imgThres;
	threshold(m_48, imgThres, 255, 1, THRESH_OTSU);

	for (int i = 0; i < 48; i++)
		for (int j = 0; j < 48; j++)
			structfp[i * 48 + j] = imgThres.at<uchar>(i, j);
	//imshow("88", imgThres);
	//waitKey();
}

void func1()
{
	float hsvHist0[64], hsvHist1[64];
	Mat src0 = imread("zy-1.png");
	//Mat src0 = imread("346a-0-37-81.jpg");
	Mat src1 = imread("zy-2.png");
	CalcHistogram h;
	h.mat2HSVHist(src0, hsvHist0);
	h.mat2HSVHist(src1, hsvHist1);
	calcCosSimi(hsvHist0, hsvHist1);
}

void func2()
{
	Mat image0 = imread("dup_214_1-7.jpg");
	Mat image1 = imread("dup_214_0-0.jpg");
	
	int ghist0[64], ghist1[64];
	Histogram1D hist1d;
	hist1d.mat2GrayHist(image0, ghist0);
	hist1d.mat2GrayHist(image1, ghist1);
	cout << calcCosSimi(ghist0, ghist1) << endl;

	Mat m0, m1;
	cvtColor(image0, m0, CV_BGR2GRAY);
	cvtColor(image1, m1, CV_BGR2GRAY);
	equalizeHist(m0, m0);
	equalizeHist(m1, m1);
	imshow("0", m0);
	imshow("1", m1);
	waitKey();
}

float calcHMDist(int hs0[], int hs1[], int size = 48 * 48)
{
	float hs_sum = 0;
	for (int i = 0; i < size; i++)
	{
		if (hs0[i] == hs1[i])
			hs_sum += 1;
	}
	
	return hs_sum / size;
}

void func3()
{
	Mat image0 = imread("dup_214_1-7.jpg");
	Mat image1 = imread("dup_214_0-0.jpg");

	int sfp0[48 * 48], sfp1[48 * 48];
	Mat_2_structfp(image0, sfp0);
	Mat_2_structfp(image1, sfp1);
	cout << calcHMDist(sfp0, sfp1, 48 * 48) << endl;
}


void func4()
{
	Mat src0 = imread("0-7.jpg");
	cvtColor(src0, src0, CV_BGR2HSV);
	CalcHistogram h;
	h.getHistogramImage(src0);

	waitKey();
}

int main()
{
	func4();

	return 0;
}

#endif
