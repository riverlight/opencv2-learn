#ifndef RECTINFO_H
#define RECTINFO_H

#include <vector>
#include <map>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

using namespace std;

typedef struct InfoUnit_s
{
	int mode;
	float strenth;
	float alpha;
	char png[128];
	int angle;
} InfoUnit;

class CRectInfo
{
public:
	CRectInfo(vector<string> &vrm);
	virtual ~CRectInfo();

	int _x, _y, _w, _h;
	vector<InfoUnit> _vIU;
	int _nAngle;
	
	// pre-load png image
	map<string, Mat> _mpPng;

	//Mat _t_img;
	Rect _r_t;
	Point _center_t;
	
	Rect _r;
	Point _center;

	// throw, obj-detect
	int _bHaveThrow;
	Rect _rectFace, _rectThrow;

public:
	void GetFaceRect(Mat &img);
	Mat GetPngMat(char png[128]);

private:
	void GuessFaceRect(Mat &img);

	void InsertPng(char png[128]);
	

public:
	static int CheckInputRM(vector<string> &vrm);
	static void kill_cr(char *s);
};

#endif // RECTINFO_H
