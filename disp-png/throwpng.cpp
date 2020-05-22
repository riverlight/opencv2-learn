#include "disp-png.h"
#include "throwpng.h"

static void irotate(Mat &s, float angle);

typedef struct PngAlphaInfo_s
{
	int bHavePng;
	int nStart, nEnd;
	int nMiddle;
} PngAlphaInfo;

PngAlphaInfo *createPngAlphaInfoBG(Mat &imgPngR, Mat &imgBG, Rect r)
{
	PngAlphaInfo *pai_png, *pai_bg;
	pai_png = new PngAlphaInfo[imgPngR.cols];
	pai_bg = new PngAlphaInfo[imgBG.cols];
	memset(pai_png, 0, sizeof(PngAlphaInfo)*imgPngR.cols);
	memset(pai_bg, 0, sizeof(PngAlphaInfo)*imgBG.cols);

	int i, j;
	for (i = 0; i < imgPngR.cols; i++)
	{
		PngAlphaInfo *pai = pai_png + i;
		pai->bHavePng = 0;
		pai->nStart = -1;
		pai->nEnd = -1;
		for (j = 0; j < imgPngR.rows; j++)
		{
			if (imgPngR.ptr(j)[i]>250)
			{
				pai->bHavePng = 1;
				if (pai->nStart == -1)
					pai->nStart = j;
				pai->nEnd = j;
			}
			pai->nMiddle = (pai->nStart + pai->nEnd) / 2;
		}
		cout << i << " " << pai->bHavePng << " " << pai->nStart << " " << pai->nEnd << " " << pai->nMiddle << endl;
	}

	for (i = 0; i < imgBG.cols; i++)
	{
		if (i >= r.x && i < (r.x + r.width))
		{
			pai_bg[i].bHavePng = pai_png[i-r.x].bHavePng;
			pai_bg[i].nStart = pai_png[i - r.x].nStart + r.y;
			pai_bg[i].nEnd = pai_png[i - r.x].nEnd + r.y;
			pai_bg[i].nMiddle = pai_png[i - r.x].nMiddle + r.y;
		}
		cout << i << " " << pai_bg[i].bHavePng << " " << pai_bg[i].nStart << " " << pai_bg[i].nEnd << " " << pai_bg[i].nMiddle << endl;
	}

	delete pai_png;
	return pai_bg;
}

void Throw(Mat &imgBG, PngAlphaInfo *pai)
{
	int i;
	for (i = 0; i < imgBG.cols; i++)
	{
		if (pai[i].bHavePng == 0)
			continue;

		if (pai[i].nStart > 0)
		{
			Rect r_ts, r_td;
			r_ts.x = i;
			r_ts.y = 0;
			r_ts.width = 1;
			r_ts.height = pai[i].nMiddle;
			r_td.x = i;
			r_td.y = 0;
			r_td.width = 1;
			r_td.height = pai[i].nStart;
			Mat roi_s, roi_d;
			roi_s = Mat(imgBG, r_ts);
			roi_d = Mat(imgBG, r_td);
			Mat t;
			resize(roi_s, t, Size(1, pai[i].nStart));
			t.copyTo(roi_d);
			cout << i << " " << pai[i].nStart << " " << pai[i].nMiddle << endl;
		}
		if (pai[i].nEnd < imgBG.rows - 1)
		{
			Rect r_bs, r_bd;
			r_bs.x = i;
			r_bs.y = pai[i].nMiddle + 1;
			r_bs.width = 1;
			r_bs.height = imgBG.rows - 1 - pai[i].nMiddle;
			r_bd.x = i;
			r_bd.y = pai[i].nEnd + 1;
			r_bd.width = 1;
			r_bd.height = imgBG.rows - 1 - pai[i].nEnd;
			Mat roi_s, roi_d;
			roi_s = Mat(imgBG, r_bs);
			roi_d = Mat(imgBG, r_bd);
			Mat t;
			resize(roi_s, t, Size(1, imgBG.rows - 1 - pai[i].nEnd));
			t.copyTo(roi_d);
		}
	}
}

void ThrowPng(Mat& imgBG, Rect r, Mat& imgPng, int angle)
{
	Mat imgPngR, imgPngR_g;
	resize(imgPng, imgPngR, Size(r.width, r.height));

	if (angle != 0)
		irotate(imgPngR, angle);

	std::vector<Mat> channels;
	split(imgPngR, channels);
	//channels[3] /= 128;
	
	PngAlphaInfo *pai = createPngAlphaInfoBG(channels[3], imgBG, r);
	Throw(imgBG, pai);

	Point p;
	p.x = r.x;
	p.y = r.y;

	Mat imgC = imgBG.clone();
	overlayImage(imgC, imgPngR, imgBG, p);
	
	delete pai;
}


void irotate(Mat &s, float angle)
{
	Mat rot_mat(2, 3, CV_32FC1);
	double scale = 1.0;
	Point center = Point(s.cols / 2, s.rows / 2);
	rot_mat = getRotationMatrix2D(center, angle, scale);

	Mat c = s.clone();
	warpAffine(c, s, rot_mat, s.size());
}
