#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

//ȫ�ֱ���
String src_windowName = "ԭͼ��";
String warp_windowName = "����任";
String warp_rotate_windowName = "������ת�任";
String rotate_windowName = "ͼ����ת";

void warpRotate(Mat &s, float angle)
{
	Mat rot_mat(2, 3, CV_32FC1);
	double scale = 0.8;
	Point center = Point(s.cols / 2, s.rows / 2);
	rot_mat = getRotationMatrix2D(center, angle, scale);

	Mat c = s.clone();
	warpAffine(c, s, rot_mat, s.size());
}


int xWarp(cv::Mat &mat, int dst)
{
	double pi = 3.1415;
	int w = mat.cols;
	int h = mat.rows;
	cv::Mat t = mat.clone();
	//cols
	for (int i = 0; i < w; i++)
	{
		//rows
		for (int j = 0; j < h; j++)
		{
			double delta;
			if (i < (w / 2))
				delta = (double)i / (double)w * dst * sin(((double)j / (double)h) * pi);
			else
				delta = (1 - (double)i / (double)w) * dst * sin(((double)j / (double)h) * pi);
			mat.at<cv::Vec3b>(j, i) = t.at<cv::Vec3b>(j, i - delta);
		}
	}
	return 0;
}

int test1()
{
	Point2f srcTri[3];
	Point2f dstTri[3];

	Mat rot_mat(2, 3, CV_32FC1);
	Mat warp_mat(2, 3, CV_32FC1);
	Mat srcImage, warp_dstImage, warp_rotate_dstImage, rotate_dstImage;
	Mat imgWarp;

	//����ͼ��
	srcImage = imread("88.png");

	//�ж��ļ��Ƿ���سɹ�
	if (srcImage.empty())
	{
		cout << "ͼ�����ʧ��!" << endl;
		return -1;
	}
	else
		cout << "ͼ����سɹ�!" << endl << endl;

	imgWarp = srcImage.clone();
	xWarp(imgWarp, 300);

	//��������任Ŀ��ͼ����ԭͼ��ߴ�������ͬ
	warp_dstImage = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());

	//�������������������任
	srcTri[0] = Point2f(0, 0);
	srcTri[1] = Point2f(srcImage.cols - 1, 0);
	srcTri[2] = Point2f(0, srcImage.rows - 1);

	dstTri[0] = Point2f(srcImage.cols*0.0, srcImage.rows*0.33);
	dstTri[1] = Point2f(srcImage.cols*0.85, srcImage.rows*0.25);
	dstTri[2] = Point2f(srcImage.cols*0.15, srcImage.rows*0.7);

	//�������任����
	warp_mat = getAffineTransform(srcTri, dstTri);

	//�Լ���ͼ�ν��з���任����
	warpAffine(srcImage, warp_dstImage, warp_mat, warp_dstImage.size(), CV_WARP_INVERSE_MAP, IPL_BORDER_TRANSPARENT);

	//����ͼ���е�˳ʱ����ת50�ȣ���������Ϊ0.6����ת����
	Point center = Point(warp_dstImage.cols / 2, warp_dstImage.rows / 2);
	double angle = 60.0;
	double scale = 0.8;

	//������ת����
	rot_mat = getRotationMatrix2D(center, angle, scale);

	//��ת��Ť��ͼ��
	warpAffine(warp_dstImage, warp_rotate_dstImage, rot_mat, warp_dstImage.size());

	//��ԭͼ����ת
	warpAffine(srcImage, rotate_dstImage, rot_mat, srcImage.size());

	//��ʾ�任���
	namedWindow(src_windowName, WINDOW_AUTOSIZE);
	imshow(src_windowName, srcImage);

	namedWindow("77", WINDOW_AUTOSIZE);
	imshow("77", imgWarp);
#if 1
	namedWindow(warp_windowName, WINDOW_AUTOSIZE);
	imshow(warp_windowName, warp_dstImage);

	namedWindow(warp_rotate_windowName, WINDOW_AUTOSIZE);
	imshow(warp_rotate_windowName, warp_rotate_dstImage);

	namedWindow(rotate_windowName, WINDOW_AUTOSIZE);
	imshow(rotate_windowName, rotate_dstImage);
#endif

	waitKey(0);

	return 0;
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
		cout << poi_d[i] << endl;
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

	cout << rd;
}


void rotate(Mat &s, Mat &d, double angle, double scale)
{
	Mat rot_mat(2, 3, CV_32FC1);
	Point center = Point(d.cols / 2, d.rows / 2);

	//������ת����
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

int main(int argc, char * argv[])
{
	Mat s_img, d_img, t_img;
	s_img = imread("cat.jpg");

	t_img = Mat::zeros(s_img.rows, s_img.cols, s_img.type());
	double angle = 45;
	double scale = 1.0;
	rotate(s_img, t_img, angle, scale);

	imwrite("ro-1.jpg", t_img);

	// ��ת����
	inv_rotate(s_img, t_img, angle, scale);
	imwrite("src-1.jpg", s_img);

	return 0;
}
