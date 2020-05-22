#include <iostream>
//#include <Windows.h>
#include "tools.h"
#include "monoC-matting.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Hi, this is mono-chroma matting program!" << endl;

	if (argc != 6 && argc != 7)
	{
		cout << "monoC-mat obj-file bg-file out-file rect color [threshold]" << endl;
		return -1;
	}

	int color, thres = 20;
	color = atoi(argv[5]);
	if (argc == 7)
		thres = atoi(argv[6]);

	char bgfile[100];
#ifdef WIN32
	strcpy_s(bgfile, argv[2]);
#else
	strcpy(bgfile, argv[2]);
#endif
	for (int i = 0; i < strlen(bgfile); i++)
		bgfile[i] = tolower(bgfile[i]);

	Rect r;
	string strRect;
	strRect.assign(argv[4]);

	vector<string> v_r;
	SplitString(strRect, v_r, " ");
	if (v_r.size() != 4)
	{
		cout << "param rect err : " << strRect << endl;
		return -1;
	}
	r.x = atoi(v_r[0].c_str());
	r.y = atoi(v_r[1].c_str());
	r.width = atoi(v_r[2].c_str());
	r.height = atoi(v_r[3].c_str());

	int ret;
	if (strstr(bgfile, ".jpg") != NULL || strstr(bgfile, ".png") != NULL || strstr(bgfile, ".bmp") != NULL)
	{
		ret = videoMonoCMatting_image(argv[1], argv[2], argv[3], color, thres, r);
	}
	else
	{
		ret = videoMonoCMatting_video2(argv[1], argv[2], argv[3], color, thres, r);
	}
	cout << "result : " << ret << endl;

	return ret;
}
