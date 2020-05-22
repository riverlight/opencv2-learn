#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

#include "obj-detect.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "err : [od-app infile mode]" << endl;
		return -1;
	}

	Mat in_img;
	in_img = imread(argv[1]);
	if (in_img.empty())
	{
		cout << "err : [input is empty] " << endl;
		return -2;
	}

	int ret;
	Rect r;
	int mode = atoi(argv[2]);
	switch (mode)
	{
	case 1:
		ret = Find_catface(in_img, r);
		break;
	default:
		cout << "err : [mode not support] " << endl;
		return -3;
	}

	cout << "ok" << endl;
	cout << ret << endl;
	if (ret != 0)
	{
		cout << r.x << ", " << r.y << ", " << r.width << ", " << r.height << endl;
	}
	
	return 0;
}
