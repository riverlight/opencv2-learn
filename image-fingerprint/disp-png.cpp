#include "disp-png.h"


static void irotate(Mat &s, float angle)
{
	Mat rot_mat(2, 3, CV_32FC1);
	double scale = 1.0;
	Point center = Point(s.cols / 2, s.rows / 2);
	rot_mat = getRotationMatrix2D(center, angle, scale);

	Mat c = s.clone();
	warpAffine(c, s, rot_mat, s.size());
}


void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
	cv::Mat &output, cv::Point2i location)
{
	background.copyTo(output);


	// start at the row indicated by location, or at row 0 if location.y is negative.
	for (int y = std::max(location.y, 0); y < background.rows; ++y)
	{
		int fY = y - location.y; // because of the translation

		// we are done of we have processed all rows of the foreground image.
		if (fY >= foreground.rows)
			break;

		// start at the column indicated by location,

		// or at column 0 if location.x is negative.
		for (int x = std::max(location.x, 0); x < background.cols; ++x)
		{
			int fX = x - location.x; // because of the translation.

			// we are done with this row if the column is outside of the foreground image.
			if (fX >= foreground.cols)
				break;

			// determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
			double opacity =
				((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

				/ 255.;


			// and now combine the background and foreground pixel, using the opacity,

			// but only if opacity > 0.
			for (int c = 0; opacity > 0 && c < output.channels(); ++c)
			{
				unsigned char foregroundPx =
					foreground.data[fY * foreground.step + fX * foreground.channels() + c];
				unsigned char backgroundPx =
					background.data[y * background.step + x * background.channels() + c];
				output.data[y*output.step + output.channels()*x + c] =
					backgroundPx * (1. - opacity) + foregroundPx * opacity;
			}
		}
	}
}


void DispPng(Mat& imgBG, Rect r, Mat& imgPng, int angle)
{
	Mat imgPngR, imgPngR_g;
	resize(imgPng, imgPngR, Size(r.width, r.height));

	if (angle!=0)
		irotate(imgPngR, angle);

	Point p;
	p.x = r.x;
	p.y = r.y;

	Mat imgC = imgBG.clone();
	overlayImage(imgC, imgPngR, imgBG, p);
}


void copyTo_edge(Mat &s, Mat &d, Mat &mask)
{
	Mat dd = d.clone();

	int i, j;
	uchar *ps, *pd, *pdd, *pm, *pm_n1, *pm_p1;
	for (i = 1; i < d.rows-1; i++)
	{
		ps = s.ptr<uchar>(i);
		pd = d.ptr<uchar>(i);
		pdd = dd.ptr<uchar>(i);
		pm_n1 = mask.ptr<uchar>(i - 1);
		pm = mask.ptr<uchar>(i);
		pm_p1 = mask.ptr<uchar>(i + 1);

		for (j = 1; j < d.cols-1; j++)
		{
			int count = 0;
			count = pm_n1[j - 1] + pm_n1[j] + pm_n1[j + 1] +
				+ pm[j - 1] + pm[j] + pm[j + 1] + 
				pm_p1[j - 1] + pm_p1[j] + pm_p1[j + 1];
			cout << i << " " << j << " " << count << endl;

			//if (pm[j] != 0)
			{
				//if (count != 9)
				//	count = count / 2;
				pd[3 * j + 0] = (ps[3 * j + 0] * count) / 9 + (pdd[3 * j + 0]*(9-count))/9;
				pd[3 * j + 1] = (ps[3 * j + 1] * count) / 9 + (pdd[3 * j + 1] * (9 - count)) / 9;
				pd[3 * j + 2] = (ps[3 * j + 2] * count) / 9 + (pdd[3 * j + 2] * (9 - count)) / 9;
			}
		}
	}
}

void DispPng2(Mat& imgBG, Rect r, Mat& imgPng)
{
	Mat imgPngR, imgPngR_g;
	resize(imgPng, imgPngR, Size(r.width, r.height));
	
	cvtColor(imgPngR, imgPngR_g, COLOR_RGB2GRAY);
	threshold(imgPngR_g, imgPngR_g, 253, 1, THRESH_BINARY_INV);

	Mat t = Mat(imgBG, r);
	//imgPngR.copyTo(t, imgPngR_g);
	copyTo_edge(imgPngR, t, imgPngR_g);
}
