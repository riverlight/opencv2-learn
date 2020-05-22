#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define VERSION "v 0.1.0504"


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


void DispPng(Mat& imgBG, Rect r, Mat& imgPng)
{
	Mat imgPngR, imgPngR_g;
	resize(imgPng, imgPngR, Size(r.width, r.height));

	Point p;
	p.x = r.x;
	p.y = r.y;

	Mat imgC = imgBG.clone();
	overlayImage(imgC, imgPngR, imgBG, p);
}

int main(int argc, char *argv[])
{
	cout << VERSION << " " << __TIME__ << endl;
	if (argc != 8)
	{
		cout << "bqb-addlogo input-pic input-logo output-pic x y w h" << endl;
		return -1;
	}

	Mat imgPng, imgPng_g;
	Mat imgBG;

	imgBG = imread(argv[1]);
	imgPng = imread(argv[2], -1);

	Rect r;
	r.x = atoi(argv[4]);
	r.y = atoi(argv[5]);
	r.width = atoi(argv[6]);
	r.height = atoi(argv[7]);

	DispPng(imgBG, r, imgPng);
	imwrite(argv[3], imgBG);

	return 0;
}
