#include <iostream>
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>

#include "obj-detect.h"

using namespace std;
using namespace cv;

static CascadeClassifier _cat_cascade;
static String _cat_cascade_name = "./model/haarcascade_frontalcatface_extended1.xml";
static int _bInit = 0;

int Find_catface(Mat &img, Rect &rectCatFace)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	if (_bInit == 0)
	{
		String cn, themis_root;
		char *p = getenv("THEMIS_ROOT");
		if (p == NULL)
		{
			cout << "no themis root" << endl;
			return 0;
		}
		themis_root.assign(p);
		cn = themis_root + _cat_cascade_name;
		if (!_cat_cascade.load(cn))
		{
			cout << "--(!)Error loading, name : " << cn << endl;
			return 0;
		}
		_bInit = 1;
	}

	cvtColor(img, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	_cat_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 |
		CV_HAAR_SCALE_IMAGE, Size(100, 100));

	if (faces.size() == 0)
		return 0;

	cout << "face number : " << faces.size() << endl;
	// find max rect
	int max_area = 0;
	for (int i = 0; i < faces.size(); i++)
	{
		if (faces[i].width*faces[i].height > max_area)
		{
			rectCatFace.x = faces[i].x;
			rectCatFace.y = faces[i].y;
			rectCatFace.width = faces[i].width;
			rectCatFace.height = faces[i].height;
			max_area = faces[i].width * faces[i].height;
		}
	}

#if 0
	// for debug & display
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(img, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 2, 8, 0);
		cout << " face " << endl;
	}
#endif

	return 1;
}
