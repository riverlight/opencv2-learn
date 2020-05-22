#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;

void int2string(string &str, int n)
{
	stringstream stream;
	stream << n;
	stream >> str;
}

void int2mat(int num, int width, int height)
{
	string text;
	int2string(text, num);

	
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	double font_scale = 1;	int thickness = 2;
	int baseline;

	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
	cout << text_size.width << " " << text_size.height << endl;

	cv::Mat image = cv::Mat::zeros(cv::Size(text_size.width+2, text_size.height+2), CV_8UC4);

	//将文本框居中绘制	
	cv::Point origin;
	origin.x = image.cols / 2 - text_size.width / 2;
	origin.y = image.rows / 2 + text_size.height / 2;
	cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(128, 128, 128, 255), thickness, 8, 0);

	//显示绘制解果	
	cv::imshow("image", image);
	waitKey(0);

	Mat dst;
	resize(image, dst, Size(width, height));
	imwrite("d:\\a.png", dst);
}

void test1()
{
	cv::Mat image = cv::Mat::zeros(cv::Size(640, 480), CV_8UC4);
	//设置绘制文本的相关参数	
	std::string text = "Hello World!";
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	double font_scale = 2;	int thickness = 2;
	int baseline;

	//获取文本框的长宽	
	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

	//将文本框居中绘制	
	cv::Point origin;
	origin.x = image.cols / 2 - text_size.width / 2;
	origin.y = image.rows / 2 + text_size.height / 2;
	cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(128, 128, 128, 255), thickness, 8, 0);

	//显示绘制解果	
	cv::imshow("image", image);
	waitKey(0);
	imwrite("d:\\a.png", image);
}

void test3()
{
	Mat a = imread("d:/workroom/testroom/vmaker-45078-iva-7.jpg", -1);
	if (a.empty())
		cout << "err" << endl;
	Mat b;
	imshow("11", a);
	waitKey();
}

int main(int argc, char *argv[])
{
	cout << "123" << endl;

	//int2mat(101, 100, 16);
	test3();

	return 0;
}
