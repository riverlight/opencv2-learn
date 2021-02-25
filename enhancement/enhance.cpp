#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#include "hdr.h"


void enhance_equalize(Mat &img, Mat &enh_img)
{
	Mat imageRGB[3];

	split(img, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		//if (i == 0)
		//	continue;
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, enh_img);
}

void enhance_laplas(Mat &img, Mat &enh_img)
{
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	filter2D(img, enh_img, CV_8UC3, kernel);
}

void enhance_log(Mat &img, Mat &enh_img)
{
	Mat imageLog(img.size(), CV_32FC3);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			imageLog.at<Vec3f>(i, j)[0] = log(1 + img.at<Vec3b>(i, j)[0]);
			imageLog.at<Vec3f>(i, j)[1] = log(1 + img.at<Vec3b>(i, j)[1]);
			imageLog.at<Vec3f>(i, j)[2] = log(1 + img.at<Vec3b>(i, j)[2]);
		}
	}
	//��һ����0~255  
	normalize(imageLog, imageLog, 0, 255, NORM_MINMAX);
	//ת����8bitͼ����ʾ  
	convertScaleAbs(imageLog, enh_img);
}

void enhance_gamma(Mat &img, Mat &enh_img)
{
	Mat imageGamma(img.size(), CV_32FC3);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			imageGamma.at<Vec3f>(i, j)[0] = (img.at<Vec3b>(i, j)[0])*(img.at<Vec3b>(i, j)[0])*(img.at<Vec3b>(i, j)[0]);
			imageGamma.at<Vec3f>(i, j)[1] = (img.at<Vec3b>(i, j)[1])*(img.at<Vec3b>(i, j)[1])*(img.at<Vec3b>(i, j)[1]);
			imageGamma.at<Vec3f>(i, j)[2] = (img.at<Vec3b>(i, j)[2])*(img.at<Vec3b>(i, j)[2])*(img.at<Vec3b>(i, j)[2]);
		}
	}
	//��һ����0~255  
	normalize(imageGamma, imageGamma, 0, 255);
	//ת����8bitͼ����ʾ  
	convertScaleAbs(imageGamma, enh_img);
	enh_img.convertTo(enh_img, CV_8UC3);
}

void test_img()
{
	Mat image = imread("1.jpg", 1);
	if (image.empty())
	{
		std::cout << "��ͼƬʧ��,����" << std::endl;
		return;
	}
	imshow("ԭͼ��", image);

	Mat enh;
	enhance_equalize(image, enh);

	imshow("ֱ��ͼ���⻯ͼ����ǿЧ��", enh);
	waitKey();
}

void test_img1()
{
	Mat image = imread("00000.png", 1);
	if (image.empty())
	{
		std::cout << "��ͼƬʧ��,����" << std::endl;
		return;
	}
	//imshow("ԭͼ��", image);

	Mat enh;
	//ALTMRetinex(image, enh, true);
	normalize(image, enh, 0, 255, NORM_MINMAX);

	//imshow("Retinex : ", enh);
	//waitKey();
	imwrite("v1-ret.png", enh);
}

void test_video()
{
	VideoCapture cap;
	cap.open("48-ir.avi");

	Size s_bg = Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputV;
	int k = VideoWriter::fourcc('I', '4', '2', '0');

	//int k = cv.
	if (!outputV.open("48-ir-ret.avi", k, cap.get(CAP_PROP_FPS), s_bg))
		return;

	Mat img_in, img_out;
	int count = 0;
	while (1)
	{
		cap >> img_in;
		if (img_in.empty())
			break;

		img_out = img_in.clone();
		hdr2(img_in, img_out);
		//enhance_equalize(img_in, img_out);//, 0, 255, NORM_MINMAX);
		outputV.write(img_out);
		cout << count << endl;
		if (count > 150)
			break;
		count++;
	}
	outputV.release();
}

int main(int argc, char *argv[])
{
	test_video();
	//test_img1();
	return 0;
}
