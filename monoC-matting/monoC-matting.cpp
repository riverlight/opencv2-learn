#include "monoC-matting.h"


#define monoC(x, y, z) \
		{	\
		int i, j; \
		uchar *p, *pBG; \
		for (i = 0; i < imgObj.rows; i++) \
				{ \
			p = imgObj.ptr<uchar>(i); \
			pBG = imgBG.ptr<uchar>(i); \
			for (j = 0; j < imgObj.cols; j++) \
						{ \
				int b, g, r; \
				b = p[3 * j + 0]; \
				g = p[3 * j + 1]; \
				r = p[3 * j + 2]; \
				if ((x - y < thres) || (x - z < thres)) \
								{ \
					pBG[3 * j + 0] = 1.0*p[3 * j + 0] + 0.0*pBG[3 * j + 0]; \
					pBG[3 * j + 1] = 1.0*p[3 * j + 1] + 0.0*pBG[3 * j + 1]; \
					pBG[3 * j + 2] = 1.0*p[3 * j + 2] + 0.0*pBG[3 * j + 2]; \
								} \
						} \
				} \
		} \

int dbgMatting(L_INOUT Mat &imgObj, L_IN Mat imgBG, int flagChroma, int thres)
{
	int i, j; 
	uchar *p, *pBG; 

	cout << imgObj.type() << endl;
	cout << imgBG.type() << endl;
	cout << imgObj.size() << endl;;

	for (i = 0; i < imgObj.rows; i++) 
	{ 
		p = imgObj.ptr<uchar>(i); 
		pBG = imgBG.ptr<uchar>(i); 
		for (j = 0; j < imgObj.cols; j++) 
		{ 
			int b, g, r; 
			b = p[3 * j + 0]; 
			g = p[3 * j + 1]; 
			r = p[3 * j + 2]; 
			if (g>0 && (g - r > thres) && (g - b > thres)) 
			{ 
				p[3 * j + 0] = pBG[3 * j + 0]; 
				p[3 * j + 1] = pBG[3 * j + 1]; 
				p[3 * j + 2] = pBG[3 * j + 2]; 
			} 
		} 
	} 

	return 1;
}

int monoC_matting_B(L_INOUT Mat &imgObj, L_IN Mat imgBG, int thres)
{
	monoC(b, g, r);
	return 1;
}

int monoC_matting_G(L_INOUT Mat &imgObj, L_IN Mat imgBG, int thres)
{
	monoC(g, b, r);
	return 1;
}

int monoC_matting_R(L_INOUT Mat &imgObj, L_IN Mat imgBG, int thres)
{
	monoC(r, g, b);
	return 1;
}

#if 1
int monoC_matting(L_INOUT Mat& imgObj, L_IN Mat imgBG, int flagChroma, int thres)
{
	Mat imgAlpha = Mat::zeros(imgObj.size(), CV_8UC1);

	// get img alpha
	{
		int i, j;
		uchar* p, * pAlpha;
		for (i = 0; i < imgObj.rows; i++)
		{
			p = imgObj.ptr<uchar>(i);
			pAlpha = imgAlpha.ptr<uchar>(i);
			for (j = 0; j < imgObj.cols; j++)
			{
				int x, y, z;
				x = p[3 * j + ((0 + flagChroma) % 3)];
				y = p[3 * j + ((1 + flagChroma) % 3)];
				z = p[3 * j + ((2 + flagChroma) % 3)];
				if ((x - y < thres) || (x - z < thres))
					pAlpha[j] = 1;
			}
		}
	}

	Mat imgCoeff = Mat::zeros(imgAlpha.size(), CV_32FC1);
	// get coeff
	{
		int i, j;
		uchar* pAlpha, *pA_0, *pA_2;
		float* pCoeff;
		for (i = 1; i < imgAlpha.rows-1; i++)
		{
			pAlpha = imgAlpha.ptr<uchar>(i);
			pA_0 = imgAlpha.ptr<uchar>(i - 1);
			pA_2 = imgAlpha.ptr<uchar>(i + 1);
			pCoeff = imgCoeff.ptr<float>(i);

			pCoeff = imgCoeff.ptr<float>(i);
			for (j = 1; j < imgAlpha.cols-1; j++)
			{
				int a00, a01, a02, a10, a11, a12, a20, a21, a22;
				a00 = pA_0[j - 1];
				a01 = pA_0[j];
				a02 = pA_0[j + 1];
				a10 = pAlpha[j - 1];
				a11 = pAlpha[j];
				a12 = pAlpha[j + 1];
				a20 = pA_2[j - 1];
				a21 = pA_2[j];
				a22 = pA_2[j + 1];
				pCoeff[j] = float(a00+a01+a02+a10+a11*8+a12+a20+a21+a22) / 16;
				//pCoeff[j] = 0.0;
			}
		}
	}

	//imwrite("test.jpg", imgAlpha);
	//exit(0);

	// fusion
	{
		int i, j;
		uchar* p, * pBG;
		float* pCoeff;
		for (i = 0; i < imgObj.rows; i++)
		{
			p = imgObj.ptr<uchar>(i);
			pBG = imgBG.ptr<uchar>(i);
			pCoeff = imgCoeff.ptr<float>(i);
			for (j = 0; j < imgObj.cols; j++)
			{
				int b, g, r;
				b = p[3 * j + 0];
				g = p[3 * j + 1];
				r = p[3 * j + 2];
				pBG[3 * j + 0] = pCoeff[j] * p[3 * j + 0] + (1 - pCoeff[j]) * pBG[3 * j + 0];
				pBG[3 * j + 1] = pCoeff[j] * p[3 * j + 1] + (1 - pCoeff[j]) * pBG[3 * j + 1];
				pBG[3 * j + 2] = pCoeff[j] * p[3 * j + 2] + (1 - pCoeff[j]) * pBG[3 * j + 2];
			}
		}
	}

	return 1;
}

#else
int monoC_matting(L_INOUT Mat &imgObj, L_IN Mat imgBG, int flagChroma, int thres)
{
	switch (flagChroma)
	{
	case 0:
		return monoC_matting_B(imgObj, imgBG, thres);
	case 1:
		return monoC_matting_G(imgObj, imgBG, thres);
	case 2:
		return monoC_matting_R(imgObj, imgBG, thres);
	default:
		return 0;
	}
}
#endif

int videoMonoCMatting_image(char *infile, char *bgfile, char *outfile, int color, int thres, Rect r)
{
	VideoCapture inputV(infile);
	if (!inputV.isOpened())
	{
		cout << " open file failed!\n" << endl;
		return -1;
	}

	Mat imgBG;
	imgBG = imread(bgfile);

	Size s_bg = imgBG.size();
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open(outfile, k, inputV.get(CV_CAP_PROP_FPS), s_bg))
		return -1;

	Size s = Size(r.width, r.height);
	Mat frame;
	Mat imgDisp;
	while (1)
	{
		inputV >> frame;
		if (frame.empty())
			break;

		Mat frame2 = Mat(s, frame.type(), 0);
		resize(frame, frame2, s);

		Mat imgBG2 = imgBG.clone();
		monoC_matting(frame2, imgBG2(r), color, thres);
		outputV.write(imgBG2);
		//waitKey();
		//Sleep(1000);
	}
	outputV.release();
	return 0;
}

int videoMonoCMatting_video2(char *infile, char *bgfile, char *outfile, int color, int thres, Rect r)
{
	VideoCapture inputV(infile);
	if (!inputV.isOpened())
	{
		cout << " open file failed!\n" << endl;
		return -1;
	}

	VideoCapture inputBG(bgfile);
	if (!inputBG.isOpened())
	{
		cout << " open bg file failed!\n" << endl;
		return -1;
	}

	Size s_bg = Size(inputBG.get(CV_CAP_PROP_FRAME_WIDTH), inputBG.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open(outfile, k, inputBG.get(CV_CAP_PROP_FPS), s_bg))
		return -1;

	Size s = Size(r.width, r.height);
	Mat frame, frameBG;
	while (1)
	{
		inputV >> frame;
		if (frame.empty())
			break;
		inputBG >> frameBG;
		if (frameBG.empty())
			break;

		Mat frame2 = Mat(s, frameBG.type(), 0);
		resize(frame, frame2, s);

		monoC_matting(frame2, frameBG(r), color, thres);
		outputV.write(frameBG);
		//waitKey();
		//Sleep(1000);
	}
	outputV.release();

	return 0;
}
