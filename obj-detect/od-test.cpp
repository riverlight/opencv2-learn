#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <time.h>  
#include <vector>

#include "obj-detect.h"

using namespace std;
using namespace cv;

CascadeClassifier cat_cascade;
//String cat_cascade_name = "haarcascade_frontalcatface1.xml";
String cat_cascade_name = "haarcascade_frontalcatface_extended1.xml";

void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	cat_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | 
		CV_HAAR_SCALE_IMAGE, Size(100, 100));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 2, 8, 0);
		cout << " face " << endl;
	}
	//-- Show what you got
	imshow("11", frame);
	waitKey();
	imwrite("1.jpg", frame);
}

int main(int argc, char *argv[])
{
	Mat img;
	img = imread("c1.jpg");

	char *p = getenv("PATH1");
	if (p == NULL)
		cout << "11" << endl;
	else
		cout << p << endl;

#if 1
	Rect catface;
	if (Find_catface(img, catface)!=0)
	{
		Point center(catface.x + catface.width / 2, catface.y + catface.height / 2);
		ellipse(img, center, Size(catface.width / 2, catface.height / 2), 0, 0, 360, Scalar(255, 0, 255), 2, 8, 0);
		cout << catface << endl;
	}
	imshow("11", img);
	waitKey();
	imwrite("1.jpg", img);
#else
	if (!cat_cascade.load(cat_cascade_name))
	{
		printf("--(!)Error loading\n");
		return -1;
	}

	detectAndDisplay(img);
#endif

	return 0;
}
