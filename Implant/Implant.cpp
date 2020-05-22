#include <opencv2/imgproc/imgproc_c.h>

#include "Implant.h"

CImplant::CImplant()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		_poiLast[i].x = 0;
		_poiLast[i].y = 0;
	}
}

CImplant::~CImplant()
{

}

int CImplant::Do(Mat &bgImg, Mat &objImg)
{
	Mat bgImg_c;
	CreateGreenMask(bgImg, bgImg_c);

	float angle = 0, area;
	Point poi[4];
	GetVertexes(bgImg_c, poi, angle, area);
	AdjustVertexes(bgImg_c, poi);
	ImplObj(bgImg, objImg, bgImg_c, _poiBase, angle);

#if 0
	for (int i = 0; i < 4; i++)
	{
		circle(bgImg, poi[i], 3, Scalar(0, 0, 255), -1);
		circle(bgImg, _poiBase[i], 3, Scalar(255, 0, 0), -1);
	}
#endif

	_nFrameCount++;
	Mat img1 = bgImg.clone();
	for (int i = 0; i < 4; i++)
	{
		_poiLast[i].x = poi[i].x;
		_poiLast[i].y = poi[i].y;
		circle(img1, poi[i], 3, Scalar(0, 0, 255), -1);
		circle(img1, _poiBase[i], 3, Scalar(255, 0, 0), -1);
	}
	_fLastAngle = angle;
	_fLastArea = area;
	
	for (int i = 0; i < 4; i++ )
		SaveVertexImage(bgImg_c, _imgBase[i], _poiBase, i);

	//resize(img1, img1, Size(img1.cols / 2, img1.rows / 2));
	//imshow("88", img1);
	//waitKey();


	return 0;
}

void CImplant::AdjustVertexes(Mat &greenMask, const Point poi[4])
{
	int i;
	if (_nFrameCount == 0)
	{
		for (i = 0; i < 4; i++)
		{
			_poiBase[i].x = poi[i].x;
			_poiBase[i].y = poi[i].y;
		}
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		int sumCur = CountMatNotZeroNumber(greenMask, poi);

		if (IsBigger(poi[i], _poiBase[i], i))
		{
			_poiBase[i].x = poi[i].x;
			_poiBase[i].y = poi[i].y;
		}
		else
		{
			Mat gm_tmp = greenMask.clone();
			Point poiTmp[4];
			for (int ii = 0; ii < 4; ii++)
			{
				if (ii == i)
				{
					poiTmp[ii].x = _poiBase[ii].x;
					poiTmp[ii].y = _poiBase[ii].y;
				}
				else
				{
					poiTmp[ii].x = poi[ii].x;
					poiTmp[ii].y = poi[ii].y;
				}
			}
			int sumBase;
			sumBase = CountMatNotZeroNumber(greenMask, poiTmp);
			
			//cout << _nFrameCount << " " << CountMatNotZeroNumber(greenMask) << " " << sumBase << " " << sumCur << endl;

			int areaBase, areaCur;
			areaBase = CountVertexArea(poiTmp);
			areaCur = CountVertexArea(poi);
			//cout << double(sumCur - sumBase) / double(areaBase - areaCur + 1) << " " << areaBase << " " << areaCur << endl;
			if (sumCur <= sumBase)
			{
				_poiBase[i].x = poi[i].x;
				_poiBase[i].y = poi[i].y;
			}
			else
			{
				float fLeonIndex; // Leon 自创的指数 -:)
				fLeonIndex = double(sumCur - sumBase) / double(areaBase - areaCur + 1);
				if (fLeonIndex < 0.5)
				{
					_poiBase[i].x = poi[i].x;
					_poiBase[i].y = poi[i].y;
				}
			}
		}
	}
}

int CImplant::ImplObj(Mat &bgImg, Mat &obgImg, Mat &greenMask, Point poi[4], float angle)
{
	Point2f srcTri[4];
	Point2f dstTri[4];

	//设置三个点来计算仿射变换
	srcTri[0] = Point2f(0, 0);
	srcTri[1] = Point2f(0, obgImg.rows - 1);
	srcTri[2] = Point2f(obgImg.cols, obgImg.rows - 1);
	srcTri[3] = Point2f(obgImg.cols, 0);
	for (int i = 0; i < 4; i++)
	{
		dstTri[i].x = poi[i].x;
		dstTri[i].y = poi[i].y;
	}
	//计算仿射变换矩阵
	Mat warp_dstImage = Mat::zeros(bgImg.rows, bgImg.cols, bgImg.type());
	Mat warp_mat(3, 3, CV_32FC1);
	warp_mat = getPerspectiveTransform(srcTri, dstTri); 
	warpPerspective(obgImg, warp_dstImage, warp_mat, warp_dstImage.size(), 1, 0, Scalar(0, 0, 0));
	
	Copy_mask_green(bgImg, warp_dstImage, greenMask);
	
	//resize(bgImg, bgImg, Size(bgImg.cols / 2, bgImg.rows / 2));
	//imshow("88", bgImg);
	//waitKey();

	//imwrite("impl.png", bgImg);

	return 0;
}

int CImplant::GetVertexes(Mat &greenMask, Point poi[4], float &angle, float &area)
{
	vector<vector<Point> > contours;
	findContours(greenMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //找轮廓

	// 寻找最大轮廓
	double maxArea = 0;
	int maxContourIndex = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double farea = contourArea(contours[i], false);
		if (farea>maxArea)
		{
			maxArea = farea;
			maxContourIndex = i;
		}
	}
	
	int min_x, max_x, min_y, max_y;
	min_x = min_y = 100000;
	max_x = max_y = -1;
	for (int i = 0; i < contours[maxContourIndex].size(); i++)
	{
		if (contours[maxContourIndex][i].y < min_y)
		{
			min_y = contours[maxContourIndex][i].y;
			poi[0] = contours[maxContourIndex][i];
		}
		if (contours[maxContourIndex][i].x < min_x)
		{
			min_x = contours[maxContourIndex][i].x;
			poi[1] = contours[maxContourIndex][i];
		}
		if (contours[maxContourIndex][i].y > max_y)
		{
			max_y = contours[maxContourIndex][i].y;
			poi[2] = contours[maxContourIndex][i];
		}
		if (contours[maxContourIndex][i].x > max_x)
		{
			max_x = contours[maxContourIndex][i].x;
			poi[3] = contours[maxContourIndex][i];
		}
	}
	
	CvPoint2D32f rectpoint[4];
	CvBox2D rect = minAreaRect(Mat(contours[maxContourIndex]));

	cvBoxPoints(rect, rectpoint); //获取4个顶点坐标  
	//与水平线的角度  
	angle = rect.angle;
	area = maxArea;

	// 重新制作 greenmask
	vector<Point> hullMask;
	convexHull(Mat(contours[maxContourIndex]), hullMask, false); //凸包
	
	vector<vector<Point>> ctsMask(0);
	ctsMask.push_back(hullMask);
	greenMask = Mat::zeros(greenMask.size(), greenMask.type());
	drawContours( greenMask, ctsMask, 0, CV_RGB(255,255,255), CV_FILLED, 4, vector<Vec4i>(), 0, Point() );//绘制凸包

	//DbgMat(greenMask);
#if 0
	for (int i = 0; i < 4; i++)
	{
		Point p2;
		p2.x = poi[i].x;
		p2.y = poi[i].y;
		circle(greenMask, p2, 3, Scalar(0, 0, 255), -1);
		cout << poi[i].x << " " << poi[i].y << endl;
	}
#endif

	return 0;
}


void CImplant::mattingGreen(Mat &img)
{
	int thres = 10;
	int i, j;
	uchar *p;
	for (i = 0; i < img.rows; i++)
	{
		p = img.ptr<uchar>(i);
		for (j = 0; j < img.cols; j++)
		{
			int b, g, r;
			b = p[3 * j + 0];
			g = p[3 * j + 1];
			r = p[3 * j + 2];
			if ((g - b < thres) || (g - r < thres))
			{
				p[3 * j + 0] = 0;
				p[3 * j + 1] = 0;
				p[3 * j + 2] = 0;
			}
		}
	}
	return;
}

void CImplant::Copy_mask_green(Mat &dst, Mat &src, Mat &mask)
{
	int thres = 10;
	int i, j;
	uchar *ps, *pd, *pm;

	for (i = 0; i < src.rows; i++)
	{
		ps = src.ptr<uchar>(i);
		pd = dst.ptr<uchar>(i);
		pm = mask.ptr<uchar>(i);
		for (j = 0; j < src.cols; j++)
		{
			int b, g, r, m;
			b = pd[3 * j + 0];
			g = pd[3 * j + 1];
			r = pd[3 * j + 2];
			m = pm[j];
#if 0
			if (m!=0)
			{
				pd[3 * j + 0] = 0;
				pd[3 * j + 1] = 0;
				pd[3 * j + 2] = 0;
			}
#else
			if ((g - b >= thres) && (g - r >= thres) && m!=0)
			{
				pd[3 * j + 0] = ps[3 * j + 0];
				pd[3 * j + 1] = ps[3 * j + 1];
				pd[3 * j + 2] = ps[3 * j + 2];
			}
#endif
		}
	}
	return;
}

void CImplant::SaveVertexImage(Mat &greenImg, Mat &vtxImg, const Point poi[4], int pos)
{
	if (poi[2].y - poi[0].y < 16)
	{
		SetImageEmpty(vtxImg);
		return;
	}
	if (poi[3].x - poi[1].x <16)
	{
		SetImageEmpty(vtxImg);
		return;
	}

	Rect r;
	switch (pos)
	{
	case 0:
		r.x = poi[pos].x - 7;
		r.y = poi[pos].y;
		r.width = 16;
		r.height = 16;
		if (r.x<0)
		{
			SetImageEmpty(vtxImg);
			return;
		}
		Mat(greenImg, r).copyTo(vtxImg);
		break;
	case 1:
		r.x = poi[pos].x;
		r.y = poi[pos].y - 7;
		r.width = 16;
		r.height = 16;
		if (r.y<0 || (r.y + 8)>greenImg.rows)
		{
			SetImageEmpty(vtxImg);
			return;
		}
		Mat(greenImg, r).copyTo(vtxImg);
		break;
	case 2:
		r.x = poi[pos].x - 7;
		r.y = poi[pos].y - 15;
		r.width = 16;
		r.height = 16;
		if (r.x<0 || r.y<0)
		{
			SetImageEmpty(vtxImg);
			return;
		}
		Mat(greenImg, r).copyTo(vtxImg);
		break;
	case 3:
		r.x = poi[pos].x - 15;
		r.y = poi[pos].y - 7;
		r.width = 16;
		r.height = 16;
		if (r.y<0 || (r.y+8)>greenImg.rows)
		{
			SetImageEmpty(vtxImg);
			return;
		}
		Mat(greenImg, r).copyTo(vtxImg);
		break;
	default :
		return;
	}
}

void CImplant::SetImageEmpty(Mat &img)
{
	if (!img.empty())
		img.release();
}

int CImplant::CountSAD(Mat &m0, Mat &m1)
{
	int nSum = 16 * 16 * 256;
	if (m0.empty() || m1.empty())
		return nSum;

	Mat d;
	absdiff(m0, m1, d);
	nSum = cv::sum(d)[0];
	return nSum;
}

int CImplant::IsBigger(Point poiCur, Point poiBase, int pos)
{
	switch (pos)
	{
	case 0:
		return (poiCur.y < poiBase.y) ? 1 : 0;
	case 1:
		return (poiCur.x < poiBase.x) ? 1 : 0;
	case 2:
		return (poiCur.y > poiBase.y) ? 1 : 0;
	case 3:
		return (poiCur.x > poiBase.x) ? 1 : 0;
	default:
		return 0;
	}
}
