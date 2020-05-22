#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "math.h"  
#include "magic.h"

void sketch(Mat srcImg, Mat &resultImg)
{
	//转成灰度图
	Mat gray;
	cvtColor(srcImg, gray, CV_BGR2GRAY);

	//中值滤波:去除噪声，让边缘锐化
	const int MEDIAN_BLUR_FILTER_SIZE = 7;
	medianBlur(gray, gray, MEDIAN_BLUR_FILTER_SIZE);

	//拉普拉斯边缘滤波
	Mat edges;
	const int LAPLACIAN_FILTER_SIZE = 5;
	Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE);

	//二值化
	Mat mask;
	const int EDGES_THRESHOLD = 80;
	threshold(edges, mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);
	mask.convertTo(resultImg, CV_8UC3);
	//mask.copyTo(resultImg);
	//imshow("Cartoonifier", resultImg);
	//waitKey(3);
}

void cartoon(Mat srcImg, Mat &resultImg)
{
	Size sizeSrc = srcImg.size();

	//降低分辨率，提高运行效率
	Size smallSize;
	smallSize.width = sizeSrc.width / 6;
	smallSize.height = sizeSrc.height / 6;
	Mat smallImg = Mat(smallSize, CV_8UC3);
	resize(srcImg, smallImg, smallSize, 0, 0, INTER_LINEAR);


	//双边滤波
	Mat temp = Mat(smallSize, CV_8UC3);
	int repetitions = 3; //执行双边滤波次数
	for (int i = 0; i < repetitions; ++i)
	{
		int ksize = 15;  //滤波核大小 9
		double sigmaColor = 50;  //9
		double sigmaSpace = 50;  //7
		bilateralFilter(smallImg, temp, ksize, sigmaColor, sigmaSpace);
		bilateralFilter(temp, smallImg, ksize, sigmaColor, sigmaSpace);
	}


	//还原图像分辨率
	Mat bigImg;
	resize(smallImg, bigImg, sizeSrc, 0, 0, INTER_LINEAR);
	resultImg.setTo(0);

	//保持边缘
	Mat gray;
	cvtColor(srcImg, gray, CV_BGR2GRAY);
	const int MEDIAN_BLUR_FILTER_SIZE = 7;
	medianBlur(gray, gray, MEDIAN_BLUR_FILTER_SIZE);
	Mat edges;
	const int LAPLACIAN_FILTER_SIZE = 5;
	Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE);
	Mat mask;
	const int EDGES_THRESHOLD = 80;
	threshold(edges, mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);
	bigImg.copyTo(resultImg, mask);
	/*Mat mask2 = -1*mask+255;
	srcImg.copyTo(resultImg, mask2);*/
	//bigImg.copyTo(resultImg);
}

//哈哈镜 凸透镜效果
void MaxFrame(Mat  srcImg, Mat &resultImg)
{
	srcImg.copyTo(resultImg);
	IplImage temp = IplImage(resultImg);
	IplImage *frame;
	frame = &temp;
	uchar* old_data = (uchar*)frame->imageData;
	uchar* new_data = new uchar[frame->widthStep * frame->height];

	//中心点
	int center_X = frame->width / 2;
	int center_Y = frame->height / 2;

	//半径
	int radius = 400;//400 for all
	/*if (frame->width > frame->height)
	radius = frame->width;
	else
	radius = frame->height;*/
	int newX = 0;
	int newY = 0;

	int real_radius = (int)(radius / 2.0);
	for (int i = 0; i < frame->width; i++)
	{
		for (int j = 0; j < frame->height; j++)
		{
			int tX = i - center_X;
			int tY = j - center_Y;

			int distance = (int)(tX * tX + tY * tY);
			if (distance < radius * radius)
			{
				newX = (int)((float)(tX) / 2.0);
				newY = (int)((float)(tY) / 2.0);

				newX = (int)(newX * (sqrt((double)distance) / real_radius));
				newX = (int)(newX * (sqrt((double)distance) / real_radius));

				newX = newX + center_X;
				newY = newY + center_Y;

				new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * newY + newX * 3];
				new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * newY + newX * 3 + 1];
				new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * newY + newX * 3 + 2];
			}
			else
			{
				new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * j + i * 3];
				new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * j + i * 3 + 1];
				new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * j + i * 3 + 2];
			}
		}
	}
	memcpy(old_data, new_data, sizeof(uchar) * frame->widthStep * frame->height);
	delete new_data;
}

//哈哈镜 凹透镜效果
void MinFrame(Mat  srcImg, Mat &resultImg)
{
	srcImg.copyTo(resultImg);
	IplImage temp = IplImage(resultImg);
	IplImage *frame;
	frame = &temp;
	uchar* old_data = (uchar*)frame->imageData;
	uchar* new_data = new uchar[frame->widthStep * frame->height];

	//中心点
	int center_X = frame->width / 2;
	int center_Y = frame->height / 2;

	int radius = 100;//半径
	double theta = 0;
	int newX = 0;
	int newY = 0;

	for (int i = 0; i < frame->width; i++)
	{
		for (int j = 0; j < frame->height; j++)
		{
			int tX = i - center_X;
			int tY = j - center_Y;

			theta = atan2((double)tY, (double)tX);
			radius = (int)sqrt((double)(tX * tX) + (double)(tY * tY));
			int newR = (int)(sqrt((double)radius) * 12);
			newX = center_X + (int)(newR * cos(theta));
			newY = center_Y + (int)(newR * sin(theta));

			if (!(newX > 0 && newX < frame->width))
			{
				newX = 0;
			}
			if (!(newY > 0 && newY < frame->height))
			{
				newY = 0;
			}

			new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * newY + newX * 3];
			new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * newY + newX * 3 + 1];
			new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * newY + newX * 3 + 2];
		}
	}
	memcpy(old_data, new_data, sizeof(uchar) * frame->widthStep * frame->height);
	delete new_data;
}

#define pi 3.1415926  
void waterCircle(Mat &img)
{
	Mat img2 = img.clone();

	int width = img.cols;
	int height = img.rows;
	float A, B;
	A = 7;
	B = 2.5;

	Point Center(width / 2, height / 2);

	float r0, r1, new_x, new_y;
	float p, q, x1, y1, x0, y0;
	float theta;

	for (int y = 0; y<height; y++)
	{
		for (int x = 0; x<width; x++)
		{
			y0 = Center.y - y;
			x0 = x - Center.x;
			theta = atan(y0 / (x0 + 0.00001));
			if (x0<0) theta = theta + pi;
			r0 = sqrt(x0*x0 + y0*y0);
			r1 = r0 + A*width*0.01*sin(B*0.1*r0);

			new_x = r1*cos(theta);
			new_y = r1*sin(theta);

			new_x = Center.x + new_x;
			new_y = Center.y - new_y;

			if (new_x<0)         new_x = 0;
			if (new_x >= width - 1)  new_x = width - 2;
			if (new_y<0)         new_y = 0;
			if (new_y >= height - 1) new_y = height - 2;

			x1 = (int)new_x;
			y1 = (int)new_y;

			p = new_x - x1;
			q = new_y - y1;

			for (int k = 0; k<3; k++)
			{
				img.at<Vec3b>(y, x)[k] = (1 - p)*(1 - q)*img2.at<Vec3b>(y1, x1)[k] +
					(p)*(1 - q)*img2.at<Vec3b>(y1, x1 + 1)[k] +
					(1 - p)*(q)*img2.at<Vec3b>(y1 + 1, x1)[k] +
					(p)*(q)*img2.at<Vec3b>(y1 + 1, x1 + 1)[k];
			}

		}
	}
}
