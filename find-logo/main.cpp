#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define LABS(x) ((x)>0 ? (x) : -(x))
float meanAbsDiffMatU8(Mat&s0, Mat s1, int nonZeroCount)
{
	int sum = 0;

	int i, j;
	uchar *p0, *p1;
	for (i = 0; i < s0.rows; i++)
	{
		p0 = s0.ptr<uchar>(i);
		p1 = s1.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			v1 = p1[j];
			sum += LABS(v0 - v1);
		}
	}
	return float(sum) / nonZeroCount;
}

int main(int argc, char *argv[])
{
	//cout << "Hi, this is a find-logo program!\n" << endl;
	if (argc != 3)
	{
		cout << "err" << endl;
		return -1;
	}
	char *srcName, *logoName;
	srcName = argv[1];
	logoName = argv[2];

	Mat imgLogo, imgSrc;
	imgLogo = imread(logoName);
	if (imgLogo.empty())
	{
		cout << "err" << endl;
		return 0;
	}
		
	Mat imgLogo_g;
	cvtColor(imgLogo, imgLogo_g, COLOR_BGR2GRAY);

	VideoCapture cap;
	cap.open(srcName);
	if (!cap.isOpened())
	{
		cout << "err" << endl;
		return 0;
	}
	cap >> imgSrc;
	//cout << imgSrc.type() << endl;
	cvtColor(imgSrc, imgSrc, COLOR_BGR2GRAY);
	//cout << imgSrc.type() << endl;
	
	//waitKey();
	threshold(imgLogo_g, imgLogo_g, 250, 1, THRESH_BINARY_INV);
	//cout << imgLogo_g << endl;
	//cout << "sum : " << sum(imgLogo_g);
	//imshow("logo", imgLogo_g);
	//waitKey();
	int count = sum(imgLogo_g)[0];
	Mat imgT;
	imgLogo.copyTo(imgT, imgLogo_g);
	cvtColor(imgT, imgT, COLOR_BGR2GRAY);

	Size sLogo, sSrc;
	sLogo = imgT.size();
	sSrc = imgSrc.size();

	int i, j;
	float minMean = 255 * count;
	int minx, miny;
	for (i = 0; i < sSrc.height - sLogo.height; i++)
	{
		for (j = 0; j < sSrc.width - sLogo.width; j++)
		{
			Rect r = { j, i, sLogo.width, sLogo.height };
			Mat roi(imgSrc, r), roi2;
			Mat diff;
			roi.copyTo(roi2, imgLogo_g);
			
			float m = meanAbsDiffMatU8(roi2, imgT, count);
			if (minMean > m)
			{
				minMean = m;
				minx = j;
				miny = i;
				//cout << "pos : " << i << " " << j << "  " << minMean << endl;
			}
			//break;
			//cout << "i : " << i << " j : " << j << "  " << mean(diff, imgLogo_g) << endl;
		}
		//break;
		//cout << " i : " << i << endl;
	}
	cout << minMean << " "<< minx << " " << miny << " " << imgT.cols << " " << imgT.rows << endl;

	return 0;
}
