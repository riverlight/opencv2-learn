#include "rotate.h"


void rotate(Mat &s, float angle)
{
	Mat rot_mat(2, 3, CV_32FC1);
	double scale = 0.4;
	Point center = Point(s.cols / 2, s.rows / 2);
	rot_mat = getRotationMatrix2D(center, angle, scale);

	Mat c = s.clone();
	warpAffine(c, s, rot_mat, s.size());
}

void pointAffine(Point &s, Point &d, Mat img, Mat &rotImg)
{
	float m[6];
	m[0] = rotImg.ptr<double>(0, 0)[0];
	m[1] = rotImg.ptr<double>(0, 1)[0];
	m[2] = rotImg.ptr<double>(0, 2)[0];
	m[3] = rotImg.ptr<double>(1, 0)[0];
	m[4] = rotImg.ptr<double>(1, 1)[0];
	m[5] = rotImg.ptr<double>(1, 2)[0];

	d.x = int((m[1] * s.y + m[2]) + m[0] * s.x);
	d.y = int((m[4] * s.y + m[5]) + m[3] * s.x);
}

#define i_clip(x, min, max) ( (x) < (min) ? (min) : ( (x)>(max) ? (max) : (x) ) )

void rectAffine(Rect &rs, Rect &rd, Mat img, Mat &rotImg)
{
	Point poi[4], poi_d[4];
	poi[0].x = rs.x;
	poi[0].y = rs.y;
	poi[1].x = rs.x + rs.width;
	poi[1].y = rs.y;
	poi[2].x = rs.x + rs.width;
	poi[2].y = rs.y + rs.height;
	poi[3].x = rs.x;
	poi[3].y = rs.y + rs.height;

	int x_min = 10000, x_max = -10000, y_min = 10000, y_max = -10000;
	int i;
	for (i = 0; i < 4; i++)
	{
		pointAffine(poi[i], poi_d[i], img, rotImg);
		//cout << poi_d[i] << endl;
		if (poi_d[i].x < x_min)
			x_min = poi_d[i].x;
		if (poi_d[i].x > x_max)
			x_max = poi_d[i].x;
		if (poi_d[i].y < y_min)
			y_min = poi_d[i].y;
		if (poi_d[i].y > y_max)
			y_max = poi_d[i].y;
	}

	x_min = i_clip(x_min, 0, img.cols);
	x_max = i_clip(x_max, 0, img.cols);
	y_min = i_clip(y_min, 0, img.rows);
	y_max = i_clip(y_max, 0, img.rows);
	rd.x = x_min;
	rd.width = x_max - x_min;
	rd.y = y_min;
	rd.height = y_max - y_min;

	//cout << rs << endl;
	//cout << rd << endl;
#if 1
	if (rd.width > rs.width)
	{
		int diff = rd.width - rs.width;
		rd.width = rs.width;
		rd.x += diff / 2;
	}
	if (rd.height > rs.height)
	{
		int diff = rd.height - rs.height;
		rd.height = rs.height;
		rd.y += diff / 2;
	}
	//cout << rd << endl;
	//exit(-1);
#endif
	//cout << rd;
}

void rectAffine(Rect &rs, Rect &rd, Mat img, double angle)
{
	Mat rot_mat(2, 3, CV_32FC1);
	Point center = Point(img.cols / 2, img.rows / 2);
	rot_mat = getRotationMatrix2D(center, angle, 1.0);
	rectAffine(rs, rd, img, rot_mat);
}

void i_rotate(Mat &s, Mat &d, double angle, double scale)
{
	Mat rot_mat(2, 3, CV_32FC1);
	Point center = Point(d.cols / 2, d.rows / 2);

	//¼ÆËãÐý×ª¾ØÕó
	rot_mat = getRotationMatrix2D(center, angle, scale);
	//cout << rot_mat << endl;

	warpAffine(s, d, rot_mat, s.size());
}

void inv_rotate(Mat &s, Mat &t, double angle, double scale)
{
	Mat d;
	Mat rot_mat(2, 3, CV_32FC1);
	Point center = Point(t.cols / 2, t.rows / 2);

	rot_mat = getRotationMatrix2D(center, -angle, scale);
	warpAffine(t, d, rot_mat, t.size());

	Mat m_g;
	cvtColor(d, m_g, CV_BGR2GRAY);

	Mat kernel;
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(2, 2));
	erode(m_g, m_g, kernel);

	d.copyTo(s, m_g);

#if 0
	Rect sr, dr;
	sr.x = 100;
	sr.y = 100;
	sr.width = 200;
	sr.height = 150;
	rectAffine(sr, dr, s_img, rot_mat);
#endif
}

