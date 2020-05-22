#include <stdio.h>
#include <fstream>
#include <iostream> 
#include <string> 

#include "tools.h"
#include "obj-detect.h"
#include "RectInfo.h"

using namespace std;

int CRectInfo::CheckInputRM(vector<string> &vrm)
{
	if (vrm.size() != 5 && vrm.size()!=6)
		return -1;

	return 0;
}

void CRectInfo::kill_cr(char *s)
{
	int i;
	for (i = 0; i < strlen(s); i++)
		if (s[i] == 13)
			s[i] = 0;
}

CRectInfo::CRectInfo(vector<string> &vrm)
{
	_x = atoi(vrm[0].c_str());
	_y = atoi(vrm[1].c_str());
	_w = atoi(vrm[2].c_str());
	_h = atoi(vrm[3].c_str());
	_r.x = _x;
	_r.y = _y;
	_r.width = _w;
	_r.height = _h;
	_center.x = _x + _w / 2;
	_center.y = _y + _h / 2;

	ifstream in;
	in.open(vrm[4].c_str(), ios::in);
	if (!in.is_open())
		return;

	if (vrm.size() == 6)
		_nAngle = atoi(vrm[5].c_str());
	else
		_nAngle = 0;

	_bHaveThrow = 0;
	string line;
	while (getline(in, line))
	{
		vector<string> vline;
		SplitString(line, vline, " ");
		//cout << line << endl;

		const char *charResult;
		InfoUnit iu;
		memset(&iu, 0, sizeof(iu));
		if (vline.size() == 2)
		{
			iu.mode = atoi(vline[0].c_str());
			switch (iu.mode)
			{
			case 1: // blink
				iu.strenth = atof(vline[1].c_str());
				break;
			case 2: // zoomout
				iu.alpha = atof(vline[1].c_str());
				break;
			case 3: // png
				charResult = vline[1].c_str();
				memcpy(iu.png, charResult, strlen(charResult) + 1);
				kill_cr(iu.png);
				InsertPng(iu.png);
				break;
			case 4: // rotate
				iu.angle = atoi(vline[1].c_str());
				break;
			case 5: // throw png
				charResult = vline[1].c_str();
				memcpy(iu.png, charResult, strlen(charResult) + 1);
				kill_cr(iu.png);
				InsertPng(iu.png);

				_bHaveThrow = 1;
				break;
			default:
				cout << "not support mode : " << iu.mode << endl;
				break;
			}
		}
		else
		{
			iu.mode = -1;
		}
		_vIU.push_back(iu);
	}
}

CRectInfo::~CRectInfo()
{

}

void CRectInfo::GuessFaceRect(Mat &img)
{
	_rectFace.height = _r.height * 5; // 3 + 1.0+ 1.0
	_rectFace.width = _r.width * 3; // 1.0 + 1.0 + 1.0
	_rectFace.x = _r.x - _r.width;
	_rectFace.y = _r.y - _r.height * 3;

	if (_rectFace.x <= 0)
		_rectFace.x = 0;
	if (_rectFace.y < 0)
		_rectFace.y = 0;
	if (_rectFace.x + _rectFace.width >img.cols)
		_rectFace.width = img.cols - _rectFace.x;
	if (_rectFace.y + _rectFace.height > img.rows)
		_rectFace.height = img.rows - _rectFace.y;

	_rectThrow.x = _r.x - _rectFace.x;
	_rectThrow.y = _r.y - _rectFace.y;

	_rectThrow.width = _r.width;
	_rectThrow.height = _r.height;
}

void CRectInfo::GetFaceRect(Mat &img)
{
	GuessFaceRect(img);

	if (_bHaveThrow == 0)
		return;

	Rect rTempFace;
	if (Find_catface(img, rTempFace) != 0)
	{
		if ((_r.x > rTempFace.x) && (_r.y>rTempFace.y) && ((_r.x + _r.width)<(rTempFace.x + rTempFace.width)) \
			&& ((_r.y + _r.height) < (rTempFace.y + rTempFace.height)))
		{
			//cout << (_r.y + _r.height) << " " << (_rectFace.y + _rectFace.height) << endl;
			_rectFace.x = rTempFace.x;
			_rectFace.y = rTempFace.y;
			_rectFace.width = rTempFace.width;
			_rectFace.height = rTempFace.height;

			_rectThrow.x = _r.x - _rectFace.x;
			_rectThrow.y = _r.y - _rectFace.y;
			_rectThrow.width = _r.width;
			_rectThrow.height = _r.height;
		}
	}
	//cout << "face : " << _rectFace << endl;
	//cout << "throw : " << _rectThrow << endl;
}

void CRectInfo::InsertPng(char png[128])
{
	map<string, Mat>::iterator it_find;
	it_find = _mpPng.find(png);
	if (it_find != _mpPng.end()){
		
	}
	else{
		Mat pngimg = imread(png, -1);
		_mpPng.insert(make_pair(png, pngimg));
	}
}

Mat CRectInfo::GetPngMat(char png[128])
{
	map<string, Mat>::iterator it_find;
	it_find = _mpPng.find(png);
	return it_find->second;
}
