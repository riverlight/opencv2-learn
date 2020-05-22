#include <math.h>
#include "disp-png.h"
#include "throwpng.h"

static void irotate(Mat &s, float angle);

typedef struct PngAlphaInfo_s
{
	int bHavePng;
	int nStart, nEnd;
	int nMiddle;

	// 考虑脸部特效的黑圈
	int nRStart, nREnd;
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
		pai->nRStart = -1;
		pai->nREnd = -1;
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
		//cout << i << " " << pai->bHavePng << " " << pai->nStart << " " << pai->nEnd << " " << pai->nMiddle << endl;
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
		else
		{
			pai_bg[i].bHavePng = 0;
			pai_bg[i].nStart = -1;
			pai_bg[i].nEnd = -1;
			pai_bg[i].nREnd = -1;
			pai_bg[i].nRStart = -1;
		}
		//cout << i << " " << pai_bg[i].bHavePng << " " << pai_bg[i].nStart << " " << pai_bg[i].nEnd << " " << pai_bg[i].nMiddle << endl;
	}

	delete pai_png;
	return pai_bg;
}

void t_resize(Mat &s_img, Mat &d_img, int h)
{
	int i;
	float lens = float(s_img.rows);
	float lend = float(d_img.rows);

	if (lens == lend)
	{
		s_img.copyTo(d_img);
		return;
	}

	//cout << lens << " lens " << lend << endl;
	for (i = 0; i < d_img.rows; i++)
	{
		float t;
		t = i + (lens - lend) * (float(i) / float(lend))* (float(i) / float(lend));
		int it = int(t);
		//cout << i << " " << it << endl;
		d_img.ptr(i)[0] = s_img.ptr(it)[0];
		d_img.ptr(i)[1] = s_img.ptr(it)[1];
		d_img.ptr(i)[2] = s_img.ptr(it)[2];
	}
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
			if (pai[i].nRStart==-1)
				r_ts.height = pai[i].nMiddle;
			else
				r_ts.height = pai[i].nRStart;

			r_td.x = i;
			r_td.y = 0;
			r_td.width = 1;
			r_td.height = pai[i].nStart;
			Mat roi_s, roi_d;
			roi_s = Mat(imgBG, r_ts);
			roi_d = Mat(imgBG, r_td);
			Mat t = Mat(pai[i].nStart, 1, roi_s.type());
			//t_resize(roi_s, t, Size(1, pai[i].nStart));
			t_resize(roi_s, t, pai[i].nStart);
			t.copyTo(roi_d);
			//cout << i << " " << pai[i].nStart << " " << pai[i].nMiddle << endl;
		}
		if (pai[i].nEnd < imgBG.rows - 1)
		{
			Rect r_bs, r_bd;
			r_bs.x = i;
			r_bs.width = 1;

			if (pai[i].nREnd == -1)
			{
				r_bs.y = pai[i].nMiddle + 1;
				r_bs.height = imgBG.rows - 1 - pai[i].nMiddle;
			}
			else
			{
				r_bs.y = pai[i].nREnd + 1;
				r_bs.height = imgBG.rows - 1 - pai[i].nREnd;
			}
			
			r_bd.x = i;
			r_bd.y = pai[i].nEnd + 1;
			r_bd.width = 1;
			r_bd.height = imgBG.rows - 1 - pai[i].nEnd;
			Mat roi_s, roi_d;
			roi_s = Mat(imgBG, r_bs);
			roi_d = Mat(imgBG, r_bd);
			Mat t = Mat(imgBG.rows - 1 - pai[i].nEnd, 1, roi_s.type());
			//t_resize(roi_s, t, Size(1, imgBG.rows - 1 - pai[i].nEnd));
			t_resize(roi_s, t, imgBG.rows - 1 - pai[i].nEnd);
			t.copyTo(roi_d);
		}
	}
}

void Throw2(Mat &imgBG, PngAlphaInfo *pai)
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
			Mat t = Mat(pai[i].nStart, 1, roi_s.type());
			//t_resize(roi_s, t, Size(1, pai[i].nStart));
			t_resize(roi_s, t, pai[i].nStart);
			t.copyTo(roi_d);
			//cout << i << " " << pai[i].nStart << " " << pai[i].nMiddle << endl;
		}
		if (pai[i].nEnd < imgBG.rows - 1)
		{
			Rect r_bs, r_bd;
			r_bs.x = i;
			r_bs.width = 1;

			r_bs.y = pai[i].nMiddle + 1;
			r_bs.height = imgBG.rows - 1 - pai[i].nMiddle;

			r_bd.x = i;
			r_bd.y = pai[i].nEnd + 1;
			r_bd.width = 1;
			r_bd.height = imgBG.rows - 1 - pai[i].nEnd;
			Mat roi_s, roi_d;
			roi_s = Mat(imgBG, r_bs);
			roi_d = Mat(imgBG, r_bd);
			Mat t = Mat(imgBG.rows - 1 - pai[i].nEnd, 1, roi_s.type());
			//t_resize(roi_s, t, Size(1, imgBG.rows - 1 - pai[i].nEnd));
			t_resize(roi_s, t, imgBG.rows - 1 - pai[i].nEnd);
			t.copyTo(roi_d);
		}
	}
}

#define L_abs(x) ((x)>0 ? (x) : -(x))

void stat(PngAlphaInfo *pai, Mat bgimg)
{
	int i, j;
	int num = 0;
	int max_ver = 0, center_i, center_middle;
	for (i = 0; i < bgimg.cols; i++)
	{
		if (pai[i].bHavePng != 0)
		{
			if (pai[i].nEnd - pai[i].nStart > max_ver)
			{
				max_ver = pai[i].nEnd - pai[i].nStart;
				center_i = i;
				center_middle = pai[i].nMiddle;
			}
			num++;
		}
	}
	//cout << bgimg.cols << " " << bgimg.rows << endl;
	//cout << "num : " << num << "  max_ver : " << max_ver << endl;
	//cout << "center : " << center_i << " " << center_middle << endl;

	float r_s = float(num>max_ver ? max_ver : num ) / 3; // 圈应该小点似乎更好
	float r_s_square = r_s*r_s;

	Mat bg_c = bgimg.clone();
	for (j = 0; j < bgimg.cols; j++)
	{
		pai[j].nRStart = -1;
		pai[j].nREnd = -1;
		for (i = 0; i < bgimg.rows; i++)
		{
			//float r = sqrt((i - center_middle)*(i - center_middle) + (j - center_i)*(j - center_i));
			float r_square = ((i - center_middle)*(i - center_middle) + (j - center_i)*(j - center_i));
			if (r_square < r_s_square)
			{
				bgimg.ptr(i)[3 * j + 0] = 0;
				bgimg.ptr(i)[3 * j + 1] = 0;
				bgimg.ptr(i)[3 * j + 2] = 0;

				if (pai[j].nRStart == -1)
					pai[j].nRStart = i;
				pai[j].nREnd = i;

				continue;
			}

			//float coeff = (r_s / r)*(r_s / r);
			float coeff = (r_s_square / r_square);
			int dy = int(float(coeff*L_abs(i-center_middle)) + 0.5);
			int dx = int(float(coeff*L_abs(j - center_i)) + 0.5);

			if (i>center_middle)
				dy = -dy;
			if (j > center_i)
				dx = -dx;

			//if (dy>1 || dx > 1)
			//	cout << dy << " " << dx << endl;

			bgimg.ptr(i)[3 * j + 0] = bg_c.ptr(i + dy)[3 * (j + dx) + 0];
			bgimg.ptr(i)[3 * j + 1] = bg_c.ptr(i + dy)[3 * (j + dx) + 1];
			bgimg.ptr(i)[3 * j + 2] = bg_c.ptr(i + dy)[3 * (j + dx) + 2];
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
	Throw2(imgBG, pai);
	stat(pai, imgBG);

	Point p;
	p.x = r.x;
	p.y = r.y;

	//Mat imgC = imgBG.clone();
	overlayImage2(imgPngR, imgBG, p);
	
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
