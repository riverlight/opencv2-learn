#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;

#include "hdr.h"
#include "ace.h"
#include "hir.h"

const float YCbCrYRF = 0.299F;              // RGB转YCbCr的系数(浮点类型）
const float YCbCrYGF = 0.587F;
const float YCbCrYBF = 0.114F;
const float YCbCrCbRF = -0.168736F;
const float YCbCrCbGF = -0.331264F;
const float YCbCrCbBF = 0.500000F;
const float YCbCrCrRF = 0.500000F;
const float YCbCrCrGF = -0.418688F;
const float YCbCrCrBF = -0.081312F;

const float RGBRYF = 1.00000F;            // YCbCr转RGB的系数(浮点类型）
const float RGBRCbF = 0.0000F;
const float RGBRCrF = 1.40200F;
const float RGBGYF = 1.00000F;
const float RGBGCbF = -0.34414F;
const float RGBGCrF = -0.71414F;
const float RGBBYF = 1.00000F;
const float RGBBCbF = 1.77200F;
const float RGBBCrF = 0.00000F;

const int Shift = 20;
const int HalfShiftValue = 1 << (Shift - 1);

const int YCbCrYRI = (int)(YCbCrYRF * (1 << Shift) + 0.5);         // RGB转YCbCr的系数(整数类型）
const int YCbCrYGI = (int)(YCbCrYGF * (1 << Shift) + 0.5);
const int YCbCrYBI = (int)(YCbCrYBF * (1 << Shift) + 0.5);
const int YCbCrCbRI = (int)(YCbCrCbRF * (1 << Shift) + 0.5);
const int YCbCrCbGI = (int)(YCbCrCbGF * (1 << Shift) + 0.5);
const int YCbCrCbBI = (int)(YCbCrCbBF * (1 << Shift) + 0.5);
const int YCbCrCrRI = (int)(YCbCrCrRF * (1 << Shift) + 0.5);
const int YCbCrCrGI = (int)(YCbCrCrGF * (1 << Shift) + 0.5);
const int YCbCrCrBI = (int)(YCbCrCrBF * (1 << Shift) + 0.5);

const int RGBRYI = (int)(RGBRYF * (1 << Shift) + 0.5);              // YCbCr转RGB的系数(整数类型）
const int RGBRCbI = (int)(RGBRCbF * (1 << Shift) + 0.5);
const int RGBRCrI = (int)(RGBRCrF * (1 << Shift) + 0.5);
const int RGBGYI = (int)(RGBGYF * (1 << Shift) + 0.5);
const int RGBGCbI = (int)(RGBGCbF * (1 << Shift) + 0.5);
const int RGBGCrI = (int)(RGBGCrF * (1 << Shift) + 0.5);
const int RGBBYI = (int)(RGBBYF * (1 << Shift) + 0.5);
const int RGBBCbI = (int)(RGBBCbF * (1 << Shift) + 0.5);
const int RGBBCrI = (int)(RGBBCrF * (1 << Shift) + 0.5);

static int calc_v_by_base(int v, double src, double dst)
{
	double r0 = dst / src;
	double r1 = (255.0 - dst) / (255.0 - src);
	double vv = (v > src) ? (dst + (v - src) * r1) : (v * r0);
	return int(vv+0.5);
}

Mat RGB2YCbCr(Mat src) {
	int row = src.rows;
	int col = src.cols;
	Mat dst(row, col, CV_8UC3);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int Blue = src.at<Vec3b>(i, j)[0];
			int Green = src.at<Vec3b>(i, j)[1];
			int Red = src.at<Vec3b>(i, j)[2];
			dst.at<Vec3b>(i, j)[0] = (int)((YCbCrYRI * Red + YCbCrYGI * Green + YCbCrYBI * Blue + HalfShiftValue) >> Shift);
			dst.at<Vec3b>(i, j)[1] = (int)(128 + ((YCbCrCbRI * Red + YCbCrCbGI * Green + YCbCrCbBI * Blue + HalfShiftValue) >> Shift));
			dst.at<Vec3b>(i, j)[2] = (int)(128 + ((YCbCrCrRI * Red + YCbCrCrGI * Green + YCbCrCrBI * Blue + HalfShiftValue) >> Shift));
		}
	}
	return dst;
}

Mat YCbCr2RGB(Mat src) {
	int row = src.rows;
	int col = src.cols;
	Mat dst(row, col, CV_8UC3);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int Y = src.at<Vec3b>(i, j)[0];
			int Cb = src.at<Vec3b>(i, j)[1] - 128;
			int Cr = src.at<Vec3b>(i, j)[2] - 128;
			int Red = Y + ((RGBRCrI * Cr + HalfShiftValue) >> Shift);
			int Green = Y + ((RGBGCbI * Cb + RGBGCrI * Cr + HalfShiftValue) >> Shift);
			int Blue = Y + ((RGBBCbI * Cb + HalfShiftValue) >> Shift);
			if (Red > 255) Red = 255; else if (Red < 0) Red = 0;
			if (Green > 255) Green = 255; else if (Green < 0) Green = 0;    // 编译后应该比三目运算符的效率高
			if (Blue > 255) Blue = 255; else if (Blue < 0) Blue = 0;
			dst.at<Vec3b>(i, j)[0] = Blue;
			dst.at<Vec3b>(i, j)[1] = Green;
			dst.at<Vec3b>(i, j)[2] = Red;
		}
	}
	return dst;
}

template<typename T>
inline T sign(T const& input) {
	return input >= 0 ? 1 : -1;
}

Mat AutomaticWhiteBalanceMethod(Mat &src) {
	int row = src.rows;
	int col = src.cols;
	if (src.channels() == 4) {
		cvtColor(src, src, COLOR_BGRA2BGR);
	}
	Mat input = RGB2YCbCr(src);
	Mat mark(row, col, CV_8UC1);
	int sum = 0;
	for (int i = 0; i < row; i += 100) {
		for (int j = 0; j < col; j += 100) {
			if (i + 100 < row && j + 100 < col) {
				Rect rect(j, i, 100, 100);
				Mat temp = input(rect);
				Scalar global_mean = mean(temp);
				double dr = 0, db = 0;
				for (int x = 0; x < 100; x++) {
					uchar* ptr = temp.ptr<uchar>(x) + 1;
					for (int y = 0; y < 100; y++) {
						dr += pow(abs(*ptr - global_mean[1]), 2);
						ptr++;
						db += pow(abs(*ptr - global_mean[2]), 2);
						ptr++;
						ptr++;
					}
				}
				dr /= 10000;
				db /= 10000;
				double cr_left_criteria = 1.5 * global_mean[1] + dr * sign(global_mean[1]);
				double cr_right_criteria = 1.5 * dr;
				double cb_left_criteria = global_mean[2] + db * sign(global_mean[2]);
				double cb_right_criteria = 1.5 * db;
				for (int x = 0; x < 100; x++) {
					uchar* ptr = temp.ptr<uchar>(x) + 1;
					for (int y = 0; y < 100; y++) {
						uchar cr = *ptr;
						ptr++;
						uchar cb = *ptr;
						ptr++;
						ptr++;
						if ((cr - cb_left_criteria) < cb_right_criteria && (cb - cr_left_criteria) < cr_right_criteria) {
							sum++;
							mark.at<uchar>(i + x, j + y) = 1;
						}
						else {
							mark.at<uchar>(i + x, j + y) = 0;
						}
					}
				}
			}
		}
	}

	int Threshold = 0;
	int Ymax = 0;
	int Light[256] = { 0 };
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (mark.at<uchar>(i, j) == 1) {
				Light[(int)(input.at<Vec3b>(i, j)[0])]++;
			}
			Ymax = max(Ymax, (int)(input.at<Vec3b>(i, j)[0]));
		}
	}
	printf("maxY: %d\n", Ymax);
	int sum2 = 0;
	for (int i = 255; i >= 0; i--) {
		sum2 += Light[i];
		if (sum2 >= sum * 0.1) {
			Threshold = i;
			break;
		}
	}
	printf("Threshold: %d\n", Threshold);
	printf("Sum: %d Sum2: %d\n", sum, sum2);
	double Blue = 0;
	double Green = 0;
	double Red = 0;
	int cnt2 = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (mark.at<uchar>(i, j) == 1 && (int)(input.at<Vec3b>(i, j)[0]) >= Threshold) {
				Blue += 1.0 * src.at<Vec3b>(i, j)[0];
				Green += 1.0 * src.at<Vec3b>(i, j)[1];
				Red += 1.0 * src.at<Vec3b>(i, j)[2];
				cnt2++;
			}
		}
	}
	Blue /= cnt2;
	Green /= cnt2;
	Red /= cnt2;
	printf("%.5f %.5f %.5f\n", Blue, Green, Red);

	// leon fix
	double minRGB;
	minRGB = Blue > Green ? Green : Blue;
	minRGB = minRGB > Red ? Red : minRGB;
	double ratio = pow(Ymax / minRGB, 0.5);
	ratio = ratio > 1.5 ? 1.5 : ratio;
	double maxY = ratio * minRGB;

	double dstR = (Red + maxY) / 2.0;
	double dstG = (Green + maxY) / 2.0;
	double dstB = (Blue + maxY) / 2.0;
	Mat dst(row, col, CV_8UC3);
	
	printf("maxY : %.5f\n", maxY);
	printf("%.5f %.5f %.5f\n", dstB, dstG, dstR);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
#if 1
			int B = calc_v_by_base(src.at<Vec3b>(i, j)[0], Blue, dstB);
			int G = calc_v_by_base(src.at<Vec3b>(i, j)[1], Green, dstG);
			int R = calc_v_by_base(src.at<Vec3b>(i, j)[2], Red, dstR);
#else
			int B = (int)(maxY * src.at<Vec3b>(i, j)[0] / Blue);
			int G = (int)(maxY * src.at<Vec3b>(i, j)[1] / Green);
			int R = (int)(maxY * src.at<Vec3b>(i, j)[2] / Red);
#endif
			if (B > 255) B = 255; else if (B < 0) B = 0;
			if (G > 255) G = 255; else if (G < 0) G = 0;
			if (R > 255) R = 255; else if (R < 0) R = 0;
			dst.at<Vec3b>(i, j)[0] = B;
			dst.at<Vec3b>(i, j)[1] = G;
			dst.at<Vec3b>(i, j)[2] = R;
		}
	}
	
	return dst;
}


void grayworld()
{
	Mat g_srcImage, dstImage;
	vector<Mat> g_vChannels;
	g_srcImage = imread("jstv.png");

	//分离通道	
	split(g_srcImage, g_vChannels);
	Mat imageBlueChannel = g_vChannels.at(0);
	Mat imageGreenChannel = g_vChannels.at(1);
	Mat imageRedChannel = g_vChannels.at(2);
	double imageBlueChannelAvg = 0;
	double imageGreenChannelAvg = 0;
	double imageRedChannelAvg = 0;

	//求各通道的平均值	
	imageBlueChannelAvg = mean(imageBlueChannel)[0];
	imageGreenChannelAvg = mean(imageGreenChannel)[0];
	imageRedChannelAvg = mean(imageRedChannel)[0];

	//求出各通道的增益	
	double K = (imageRedChannelAvg + imageGreenChannelAvg + imageRedChannelAvg) / 3;
	double Kb = K / imageBlueChannelAvg;	double Kg = K / imageGreenChannelAvg;
	double Kr = K / imageRedChannelAvg;

	//更新灰世界处理后各通道BGR值	
	addWeighted(imageBlueChannel, Kb, 0, 0, 0, imageBlueChannel);
	addWeighted(imageGreenChannel, Kg, 0, 0, 0, imageGreenChannel);
	addWeighted(imageRedChannel, Kr, 0, 0, 0, imageRedChannel);

	//图像各通道合并	
	merge(g_vChannels, dstImage);

	//显示原图和处理后图像	
	imshow("source-image", g_srcImage);
	imshow("destination-image", dstImage);

	//输出图片
	imwrite("source-image.jpg", g_srcImage);
	imwrite("destination-image.jpg", dstImage);

	waitKey(0);
	return ;
}


// cast 计算出的偏差值，小于1.5表示比较正常，大于1.5表示存在色偏。
// da   红/绿色偏估计值，da大于0，表示偏红；da小于0表示偏绿。
// db   黄/蓝色偏估计值，db大于0，表示偏黄；db小于0表示偏蓝。
void detect_color(cv::Mat input_img, float& cast, float& da, float& db)
{
	cv::Mat lab_img;
	cvtColor(input_img, lab_img, COLOR_BGR2Lab);

	float a = 0, b = 0;
	int HistA[256] = { 0 }, HistB[256] = { 0 };

	for (int i = 0; i < lab_img.rows; i++)
	{
		for (int j = 0; j < lab_img.cols; j++)
		{
			a += lab_img.at<cv::Vec3b>(i, j)[1];
			b += lab_img.at<cv::Vec3b>(i, j)[2];

			HistA[lab_img.at<cv::Vec3b>(i, j)[1]]++;
			HistB[lab_img.at<cv::Vec3b>(i, j)[2]]++;
		}
	}

	da = a / float(lab_img.total()) - 128;
	db = b / float(lab_img.total()) - 128;

	float Ma = 0, Mb = 0;

	for (int i = 0; i < 256; i++)
	{
		Ma += abs(i - 128 - da) * HistA[i]; // 计算范围-128～127
		Mb += abs(i - 128 - db) * HistB[i];
	}

	Ma /= float(lab_img.total());
	Mb /= float(lab_img.total());

	cast = sqrt(da * da + db * db) / sqrt(Ma * Ma + Mb * Mb);
	cout << "cast : " << cast << endl;
	cout << "da : " << da << endl;
	cout << "db : " << db << endl;
	exit(0);
}

void dbg_rgb(Mat& img)
{
	Mat imgBGR[3];
	split(img, imgBGR);
	imwrite("g.png", imgBGR[1]);
	exit(0);
}


void enhance_hsv(Mat& img, Mat& enh_img)
{
	Mat imageRGB[3];
	Mat hsv;
	cvtColor(img, hsv, COLOR_BGR2HSV);

	split(hsv, imageRGB);
	for (int i = 2; i < 3; i++)
	{
		//if (i == 0)
		//	continue;
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, enh_img);
	cvtColor(enh_img, enh_img, COLOR_HSV2BGR);
}

void enhance_equalize(Mat &img, Mat &enh_img)
{
	Mat imageRGB[3];

	split(img, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		//if (i == 0)
		//	continue;
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, enh_img);
}

void enhance_laplas(Mat &img, Mat &enh_img)
{
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	filter2D(img, enh_img, CV_8UC3, kernel);
}

void enhance_log(Mat &img, Mat &enh_img)
{
	Mat imageLog(img.size(), CV_32FC3);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			imageLog.at<Vec3f>(i, j)[0] = log(1 + img.at<Vec3b>(i, j)[0]);
			imageLog.at<Vec3f>(i, j)[1] = log(1 + img.at<Vec3b>(i, j)[1]);
			imageLog.at<Vec3f>(i, j)[2] = log(1 + img.at<Vec3b>(i, j)[2]);
		}
	}
	//归一化到0~255  
	normalize(imageLog, imageLog, 0, 255, NORM_MINMAX);
	//转换成8bit图像显示  
	convertScaleAbs(imageLog, enh_img);
}

void enhance_gamma(Mat &img, Mat &enh_img)
{
	Mat imageGamma(img.size(), CV_32FC3);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			imageGamma.at<Vec3f>(i, j)[0] = (img.at<Vec3b>(i, j)[0])*(img.at<Vec3b>(i, j)[0])*(img.at<Vec3b>(i, j)[0]);
			imageGamma.at<Vec3f>(i, j)[1] = (img.at<Vec3b>(i, j)[1])*(img.at<Vec3b>(i, j)[1])*(img.at<Vec3b>(i, j)[1]);
			imageGamma.at<Vec3f>(i, j)[2] = (img.at<Vec3b>(i, j)[2])*(img.at<Vec3b>(i, j)[2])*(img.at<Vec3b>(i, j)[2]);
		}
	}
	//归一化到0~255  
	normalize(imageGamma, imageGamma, 0, 255);
	//转换成8bit图像显示  
	convertScaleAbs(imageGamma, enh_img);
	enh_img.convertTo(enh_img, CV_8UC3);
}

void test_img()
{
	Mat image = imread("1.jpg", 1);
	if (image.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return;
	}
	imshow("原图像", image);

	Mat enh;
	enhance_equalize(image, enh);

	imshow("直方图均衡化图像增强效果", enh);
	waitKey();
}

void test_img1()
{
	Mat image = imread("22164625_pJnJ.jpg", 1);
	if (image.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return;
	}
	//imshow("原图像", image);
	//dbg_rgb(image);
	float cast, da, db;
	detect_color(image, cast, da, db);

	Mat enh;
	//ALTMRetinex(image, enh, true);
	//normalize(image, enh, 0, 255, NORM_MINMAX);
	enhance_hsv(image, enh);

	//imshow("Retinex : ", enh);
	//waitKey();
	imwrite("jstv-ret.png", enh);
}

void test_video()
{
	VideoCapture cap;
	cap.open("202111221458.mp4");

	Size s_bg = Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputV;
	int k = VideoWriter::fourcc('I', '4', '2', '0');

	//int k = cv.
	if (!outputV.open("202111221458-ac.avi", k, cap.get(CAP_PROP_FPS), s_bg))
		return;

	Mat img_in, img_out;
	int count = 0;
	while (1)
	{
		cap >> img_in;
		if (img_in.empty())
			break;

		img_out = img_in.clone();
		//hdr2(img_in, img_out);
		//img_out = AutomaticWhiteBalanceMethod(img_in);
		//enhance_equalize(img_in, img_out);//, 0, 255, NORM_MINMAX);
		//AdjustColor_2(img_in, img_out, 0, 2, 0.5);
		AdjustColor(img_in, img_out);
		outputV.write(img_out);
		cout << count << endl;
		if (count > 150000)
			break;
		count++;
	}
	outputV.release();
}

void test_img2()
{
	Mat image = imread("jstv.png", 1);
	if (image.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return;
	}

	Mat dst = AutomaticWhiteBalanceMethod(image);
	//Mat dst = ACE::getACE(image, 4, 7);
	imwrite("jstv-awb.png", dst);
	exit(0);
}

void test_ir()
{
	Mat image = imread("sc.png", 1);
	if (image.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return;
	}
	Mat dst;
	Scratch_IR(image, dst);
}


void test_ac()
{
	Mat image = imread("jstv.png");
	if (image.empty())
	{
		std::cout << "打开图片失败,请检查" << std::endl;
		return;
	}
	Mat dst;
	AdjustColor(image, dst);
	imwrite("jstv_ac.png", dst);
}

int main(int argc, char *argv[])
{
	test_video();
	//test_img1();
	//grayworld();
	//test_img2();
	//test_ir();
	//test_ac();

	return 0;
}
