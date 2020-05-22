
#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>  
#include <vector>

#include "math.h"  
#include "magic.h"
#include "blink.h"
#include "zoomout.h"
#include "tools.h"
#include "disp-png.h"
#include "throwpng.h"
#include "rotate.h"
#include "RectInfo.h"
#include "obj-detect.h"
#include "version.h"

using namespace std;
using namespace cv;

using namespace std;
using namespace cv;

#define FRAMERATE 10

static const String _logo_basename = "/data/fksc-logo.png";

string inputImageName;
string outputVideoName;
float g_fTime = 0;
vector<CRectInfo *> v_pRI;

static void Add_fkscLogo(Mat img)
{
	String logoName, themis_root;
	char *p = getenv("THEMIS_ROOT");
	if (p == NULL)
	{
		cout << "no themis root" << endl;
		return;
	}
	themis_root.assign(p);
	logoName = themis_root + _logo_basename;

	Mat logo_img = imread(logoName, -1);
	if (logo_img.empty())
	{
		cout << "no logo file" << endl;
		return;
	}

	Point poi;
	poi.x = 50;
	poi.y = img.rows - 100;
	if (poi.y < 0 || img.cols < 350)
	{
		cout << "image size too small" << endl;
		return;
	}
	overlayImage(img, logo_img, poi);
}

static int MagicProcess()
{
	Mat imgIn;
	if (inputImageName.size()){
		if (inputImageName.find(".jpg") != string::npos || inputImageName.find(".png") != string::npos
			|| inputImageName.find(".bmp") != string::npos){
			imgIn = imread(inputImageName, 1);
			if (imgIn.empty()){
				cout << "Read Image fail" << endl;
				return -1;
			}
		}
	}

	int i;
	for (i = 0; i < v_pRI.size(); i++)
	{
		CRectInfo *ri = v_pRI[i];
		ri->GetFaceRect(imgIn);
	}

	Size s = Size(imgIn.cols, imgIn.rows);
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open(outputVideoName, k, FRAMERATE, s))
	{
		cout << " xvid not support " << endl;
		return -1;
	}

	int framenum = int(g_fTime * FRAMERATE);
	int count = 0;
	while (1)
	{
		if (count >= framenum)
			break;

		Mat imgFrame;
		imgFrame = imgIn.clone();

		Mat imgRoi, imgPng;

		for (i = 0; i < v_pRI.size(); i++)
		{
			CRectInfo *ri = v_pRI[i];
			if (count < ri->_vIU.size())
			{
				switch (ri->_vIU[count].mode)
				{
				case 1:
				{
					if (ri->_nAngle==0)
						blink(imgFrame, ri->_center, ri->_w, ri->_h, ri->_vIU[count].strenth);
					else
					{
						Mat t_img;
						t_img = Mat::zeros(imgFrame.rows, imgFrame.cols, imgFrame.type());
						i_rotate(imgFrame, t_img, ri->_nAngle, 1.0);
						rectAffine(ri->_r, ri->_r_t, t_img, ri->_nAngle);
#if 0
						rectangle(t_img, ri->_r_t, Scalar(0, 0, 255), 3, 8, 0);//用矩形画矩形窗 
						imwrite("t.jpg", t_img);
						rectangle(imgFrame, ri->_r, Scalar(0, 0, 255), 3, 8, 0);//用矩形画矩形窗 
						imwrite("s.jpg", imgFrame);
						exit(-1);
#endif
						ri->_center_t.x = ri->_r_t.x + ri->_r_t.width / 2;
						ri->_center_t.y = ri->_r_t.y + ri->_r_t.height / 2;

						blink(t_img, ri->_center_t, ri->_r_t.width, ri->_r_t.height, ri->_vIU[count].strenth);
						inv_rotate(imgFrame, t_img, ri->_nAngle, 1.0);
					}
					break;
				}
				case 2:
					imgRoi = Mat(imgFrame, ri->_r);
					zoomout(imgRoi, ri->_vIU[count].alpha);
					break;
				case 3:
					//imgPng = imread(ri->_vIU[count].png, -1);
					imgPng = ri->GetPngMat(ri->_vIU[count].png);
					if (imgPng.empty())
					{
						cout << "file not exist : " << ri->_vIU[count].png << endl;
						break;
					}
					DispPng(imgFrame, ri->_r, imgPng, ri->_nAngle);
					break;
				case 4:
					rotate(imgFrame, ri->_vIU[count].angle);
					break;
				case 5:
				{
					//imgPng = imread(ri->_vIU[count].png, -1);
					imgPng = ri->GetPngMat(ri->_vIU[count].png);
					if (imgPng.empty())
					{
						cout << "file not exist : " << ri->_vIU[count].png << endl;
						break;
					}

					//cout << "face rect : " << ri->_rectFace << endl;
					Mat imgFace = imgFrame(ri->_rectFace);
					ThrowPng(imgFace, ri->_rectThrow, imgPng, ri->_nAngle);
					//ThrowPng(imgFrame, ri->_r, imgPng, ri->_nAngle);
					break;
				}
				default:
					break;
				}
			}
		}

		//Add_fkscLogo(imgFrame);
		
		outputV.write(imgFrame);
		count++;
	}
	outputV.release();

	return 0;
}

int main(int argc, char *argv[])
{
	cout << VERSION << " " << __TIME__ << endl;
	if (argc < 5)
	{
		cout << "magic in-pic out-video time [list](x y w h moban.txt)" << endl;
		return 0;
	}
	inputImageName.assign(argv[1]);
	outputVideoName.assign(argv[2]);
	g_fTime = atof(argv[3]);
	
	int i;
	for (i = 4; i < argc; i++)
	{
		string sTemp;
		sTemp.assign(argv[i]);
		
		vector<string> vRM;
		SplitString(sTemp, vRM, " ");
		if (CRectInfo::CheckInputRM(vRM) != 0)
			continue;

		CRectInfo *pRI = new CRectInfo(vRM);
		v_pRI.push_back(pRI);
	}
	
	int ret;
	ret = MagicProcess();

	return ret;
}
