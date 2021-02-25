#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "process.h"


using namespace cv;
using namespace std;

typedef struct Y4mInfo_s {
	int _width, _height;
	float _fps;
	string _colorspace;
	char _interlacing; // p, t, b, m
	int _headeroffset;
} Y4mInfo;



int Get_Y4mInfo(char* szY4m, Y4mInfo* pInfo)
{
	ifstream input(szY4m, ios::in);
	if (!input) {
		cout << szY4m << " open fail...\n";
		exit(-1);
	}

	while (1) {
		string str_tmp;
		input >> str_tmp;
		if (str_tmp == "FRAME") {
			cout << "done" << endl;
			cout << input.tellg() << endl;
			break;
		}
		switch (str_tmp[0])
		{
		case 'W':
			pInfo->_width = std::stoi(str_tmp.substr(1));
			break;
		case 'H':
			pInfo->_height = std::stoi(str_tmp.substr(1));
			break;
		case 'F':
		{
			size_t pos_t = str_tmp.find(':');
			string n0 = str_tmp.substr(1, pos_t - 1);
			string n1 = str_tmp.substr(pos_t + 1);
			cout << n0 << "  * " << n1 << endl;
			pInfo->_fps = float(std::stoi(n0)) / float(std::stof(n1));
			break;
		}
		case 'I':
			pInfo->_interlacing = str_tmp[1];
			break;
		case 'C':
			pInfo->_colorspace = str_tmp.substr(1, 3);
			cout << pInfo->_colorspace << endl;
			break;
		default:
			continue;
		}
	}
	int cur_pos = input.tellg();
	pInfo->_headeroffset = cur_pos - 6;
	
	input.close();
	return 0;
}

int main(int argc, char* argv[])
{
	cout << "Hi, this is a y4m-2-avi test program!" << endl;

	if (argc != 2) {
		cout << "Usage: y4m-2-avi *.y4m" << endl;
		return -1;
	}

	Y4mInfo info;
	Get_Y4mInfo(argv[1], &info);

	int framesize = info._width * info._height * 3 / 2;
	unsigned char* yuv;
	yuv = new unsigned char[framesize];
	ifstream fy4m(argv[1], ios::in|ios::binary);
	fy4m.seekg(info._headeroffset);
	int count = 0;
	while (1) {
		char szFrameFlag[7] = { 0 };
		fy4m.read(szFrameFlag, 6);
		if (szFrameFlag[0] == 0)
			break;
		count++;
		fy4m.read((char*)yuv, framesize);
		Process(yuv, info._width, info._height);
		if (fy4m.eof())
			break;
	}

	fy4m.close();
	delete yuv;
	return 0;
}
