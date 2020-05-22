#include "version.h"
#include "IQA.h"


CIqa g_iqa;

int main(int argc, char *argv[])
{
	if (argc != 2 && argc!=3)
	{
		cout << "err: no argv[1]";
		return -1;
	}
	string str(argv[1]);
	Mat a;
	a = imread(str);
	if (a.empty())
	{
		cout << "err: input file not exist";
		return -2;
	}

	int bHaveBI = 1;
	if (argc == 3)
		bHaveBI = atoi(argv[2]);
	//cout << str << ",";

	float fContentFactor = g_iqa.calcContentFactor(a);
#if TOUTIAOTUIJIAN
	float fContrastFactor = 1.0;
#else
	float fContrastFactor = g_iqa.calcContrastFactor(a);
#endif
	//cout << fContentFactor << endl;

	Mat img;
	if (a.cols >= 200 && a.rows >= 200)
	{
		img = a(Rect(48, 48, a.cols-96, a.rows-96));
	}
	else
		img = a;

	float mos = g_iqa.calcMos(img, bHaveBI) * fContentFactor * fContrastFactor;
	//cout << mos << endl;
	if (mos < 0.2)
		mos /= 2;
	else if (mos>0.7)
		mos = 1 - (1 - mos) / 3;
	else
		mos = 0.1 + (mos - 0.2) * 8 / 5;
	if (mos > 1.0)
		mos = 0.983562;

	cout << mos;
	//cout << "\n";

	
	//g_iqa.printGradIndex(a);
	//g_iqa.printBlockingIndex(a);
	//g_iqa.printHSVBlurIndex(a);
	//g_iqa.printIqa(a);
	//g_iqa.printNRSS(a);
	//Mat b;
	//blur(a, b, Size(3, 3));
	//imwrite(argv[2], b);

	return 0;
}
