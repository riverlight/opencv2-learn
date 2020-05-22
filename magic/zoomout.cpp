#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "math.h"  
#include "zoomout.h"

using namespace std;
using namespace cv;

#define pi 3.1415926  

int zoomout(Mat &img, float alpha)
{
	Mat Img_out;
	Img_out = img.clone();

	int width = img.cols;
	int height = img.rows;

	float R;
	float e;
	float a, b;
	//float alpha = 0.75;
	float K = pi / 2;

	a = height / 2.0; b = width / 2.0;
	e = (float)width / (float)height;
	R = std::min(a, b);

	Point Center(width / 2, height / 2);

	float radius, r0, Dis, new_x, new_y;
	float p, q, x1, y1, x0, y0;
	float theta;

	for (int y = 0; y<height; y++)
	{
		for (int x = 0; x<width; x++)
		{
			y0 = Center.y - y;
			x0 = x - Center.x;
			Dis = x0*x0 + y0*y0;
			r0 = sqrt(Dis);

			if (Dis<R*R)
			{
				theta = atan(y0 / (x0 + 0.00001));
				if (x0<0) theta = theta + pi;

				radius = R*sin(r0 / R*K);
				radius = (radius - r0)*(alpha)+r0;
				new_x = radius*cos(theta);
				new_y = radius*sin(theta);
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
#if 1
					img.at<Vec3b>(y, x)[k] = (1 - p)*(1 - q)*Img_out.at<Vec3b>(y1, x1)[k] +
						(p)*(1 - q)*Img_out.at<Vec3b>(y1, x1 + 1)[k] +
						(1 - p)*(q)*Img_out.at<Vec3b>(y1 + 1, x1)[k] +
						(p)*(q)*Img_out.at<Vec3b>(y1 + 1, x1 + 1)[k];
#else
					Img_out.at<Vec3b>(y, x)[k] = (1 - p)*(1 - q)*img.at<Vec3b>(y1, x1)[k] +
						(p)*(1 - q)*img.at<Vec3b>(y1, x1 + 1)[k] +
						(1 - p)*(q)*img.at<Vec3b>(y1 + 1, x1)[k] +
						(p)*(q)*img.at<Vec3b>(y1 + 1, x1 + 1)[k];
#endif
				}

			}

		}
	}
	return 0;
}
