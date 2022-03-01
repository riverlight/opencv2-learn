#include "hir.h"

#define HABS(x) ( (x)>0 ? (x):-(x))


void GetSCMask(Mat& src, Mat& mask, int thresMin, int trhesD)
{
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int B = src.at<Vec3b>(i, j)[0];
			int G = src.at<Vec3b>(i, j)[1];
			int R = src.at<Vec3b>(i, j)[2];
			int M = (B + G + R) / 3;
			if ((B > thresMin) && (G > thresMin) && (R > thresMin) && (HABS(M-B) < trhesD) && (HABS(M - G) < trhesD) && (HABS(M - R) < trhesD))
				mask.at<uchar>(i, j) = 255;
			else
				mask.at<uchar>(i, j) = 0;
		}
	}
	//imshow("m0", mask);
	cv::Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));//3*3全1结构元素
	cv::morphologyEx(mask, mask, cv::MORPH_TOPHAT, element);
	//imshow("m1", mask);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(
		mask,               // 输入二值图
		contours,             // 存储轮廓的向量
		hierarchy,            // 轮廓层次信息
		RETR_TREE,            // 检索所有轮廓并重建嵌套轮廓的完整层次结构
		CHAIN_APPROX_NONE);   // 每个轮廓的全部像素
	
	// erase contours
	int min_size = 2;
	double min_ratio_wh = 3;
	double max_rotated_size = 7 + min_ratio_wh;
	std::vector<std::vector<cv::Point>>::iterator itc = contours.begin();
	while (itc != contours.end())
	{
		//验证轮廓大小
		double area = contourArea(*itc);
		RotatedRect roRect = minAreaRect(*itc);
		double roWHMin, roWHMax;
		
		double length = arcLength(*itc, true);
		roWHMin = roRect.size.width > roRect.size.height ? roRect.size.height : roRect.size.width;
		roWHMax = roRect.size.width < roRect.size.height ? roRect.size.height : roRect.size.width;
		roWHMin += 1;
		roWHMax += min_ratio_wh;
		double ratio_wh = roWHMax / roWHMin;
		//cout << roRect.size.width << " *** " << roRect.size.height << " &&&& " << ratio_wh << endl;

#if 1
		if (length < min_size )
		{
			itc = contours.erase(itc);
		}
		else 
#endif
		if (ratio_wh < min_ratio_wh || roWHMin > max_rotated_size)
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
		}
	}

//	cout << "contours num : " << contours.size() << endl;
	//exit(0);
	Mat masknew = Mat::zeros(mask.size(), mask.type());
	drawContours(masknew, contours, -1, Scalar(255), -1);
	
	mask = masknew;
}


void Scratch_IR(Mat& src, Mat &dst)
{
	Mat mask = Mat(src.size(), CV_8UC1);
	GetSCMask(src, mask, 160, 32);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 5));
	dilate(mask, mask, element);
	inpaint(src, mask, dst, 3, INPAINT_TELEA); // INPAINT_NS
#if 0
	imshow("mask", mask);
	imshow("1", dst);
	imshow("src", src);
	waitKey(0);
#endif
}

static void Show_colorhist(Mat& src)
{
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0, 256 };
	const float* ranges[] = { range };
	MatND redHist, greenHist, blueHist;

	int channels_r[] = { 0 };
	//进行直方图的计算（蓝色分量部分）
	calcHist(&src, 1, channels_r, Mat(), //不使用掩膜
		blueHist, 1, hist_size, ranges,
		true, false);

	//进行直方图的计算（绿色分量部分）
	int channels_g[] = { 1 };
	calcHist(&src, 1, channels_g, Mat(), // do not use mask
		greenHist, 1, hist_size, ranges,
		true, // the histogram is uniform
		false);

	//进行直方图的计算（红色分量部分）
	int channels_b[] = { 2 };
	calcHist(&src, 1, channels_b, Mat(), // do not use mask
		redHist, 1, hist_size, ranges,
		true, // the histogram is uniform
		false);

	//-----------------------绘制出三色直方图------------------------
	//参数准备
	double maxValue_red, maxValue_green, maxValue_blue;
	minMaxLoc(redHist, 0, &maxValue_red, 0, 0);
	minMaxLoc(greenHist, 0, &maxValue_green, 0, 0);
	minMaxLoc(blueHist, 0, &maxValue_blue, 0, 0);
	int scale = 1;
	int histHeight = 256;
	Mat histImage = Mat::zeros(histHeight, bins * 3, CV_8UC3);

	//正式开始绘制
	for (int i = 0; i < bins; i++)
	{
		//参数准备
		float binValue_red = redHist.at<float>(i);
		float binValue_green = greenHist.at<float>(i);
		float binValue_blue = blueHist.at<float>(i);
		int intensity_red = cvRound(binValue_red * histHeight / maxValue_red);  //要绘制的高度
		int intensity_green = cvRound(binValue_green * histHeight / maxValue_green);  //要绘制的高度
		int intensity_blue = cvRound(binValue_blue * histHeight / maxValue_blue);  //要绘制的高度

		//绘制红色分量的直方图
		line(histImage, Point(i, histHeight - 1), Point(i, histHeight - intensity_red), CV_RGB(255, 0, 0));
		//绘制绿色分量的直方图
		line(histImage, Point(i + bins, histHeight - 1), Point(i + bins, histHeight - intensity_green), CV_RGB(0, 255, 0));
		//绘制蓝色分量的直方图
		line(histImage, Point(i + bins * 2, histHeight - 1), Point(i + bins * 2, histHeight - intensity_blue), CV_RGB(0, 0, 255));
	}
	imshow("图像的RGB直方图", histImage);
	waitKey(0);
}

static int calc_v_by_base(int v, double src, double dst)
{
//	cout << "src " << src << "  dst : " << dst << endl;
	double r0 = dst / src;
	double r1 = (256.0 - dst) / (256.0 - src);
	double vv = (v > src) ? (dst + (v - src) * r1) : (v * r0);
//	cout << v << " ++ " << vv << " ++ " << r0 << " ++ " << r1 << endl;
	return int(vv);
}

void adjust_mat_bybase(Mat& img, double src, double dst)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int v = img.at<uchar>(i, j);
			img.at<uchar>(i, j) = calc_v_by_base(v, src, dst);
		}
	}
}

void AdjustColor(Mat& src, Mat& dst)
{
#if 1
	vector<Mat> vRGBMat;
	cv::split(src, vRGBMat);
	Mat t = vRGBMat[0];
	vRGBMat[0] = vRGBMat[2];
	vRGBMat[2] = t;
	cv::merge(vRGBMat, dst);
#else
	//Show_colorhist(src);
	vector<Mat> vRGBMat;
	split(src, vRGBMat);
	double minColor = 256.0;
	int minIndex = -1;
	for (int i = 0; i < 3; i++) {
		cout << mean(vRGBMat[i])[0] << endl;
		if (mean(vRGBMat[i])[0] < minColor) {
			minIndex = i;
			minColor = mean(vRGBMat[i])[0];
		}
	}
	
	double dstColor = 256.0;
	for (int i = 0; i < 3; i++) {
		if (i == minIndex)
			continue;
		if (mean(vRGBMat[i])[0] < dstColor) {
			dstColor = mean(vRGBMat[i])[0];
		}
	}
	cout << minIndex << " : " << minColor << " **** " << dstColor << endl;
	adjust_mat_bybase(vRGBMat[minIndex], minColor, dstColor);
	merge(vRGBMat, dst);
	imshow("s", src);
	imshow("d", dst);
	waitKey();
	for (int i = 0; i < 3; i++) {
		cout << mean(vRGBMat[i])[0] << endl;
		
	}
#endif
}

void AdjustColor_2(Mat& src, Mat& dst, int idx0, int idx1, double basePoint)
{
	if (idx0 > 2 || idx1 > 2) {
		exit(-1);
	}

	dst = src.clone();

	for (int i = 0; i < dst.rows; i++)
	{
		for (int j = 0; j < dst.cols; j++)
		{
			uchar M = src.at<Vec3b>(i, j)[idx0];
			uchar N = src.at<Vec3b>(i, j)[idx1];
			double S = (M + N);
			double r = (double(j) +0.5) / double(dst.cols);
			//if (r > basePoint)
			//	r = basePoint + (r - 0.5) * (1.0 - basePoint) / (1.0 - 0.5);//  (dst + (v - src) * r1)
			//else
			//	r = r * basePoint / 0.5;
			double v0 = (1-r)*N + r*M;
			double v1 = (1-r)*M + r*N;
			v0 = v0 * S / (v0 + v1 + 1.0);
			v1 = v1 * S / (v0 + v1 + 1.0);
			v0 = v0 > 0 ? (v0<255.0 ? v0 : 255.0): 0;
			v1 = v1 > 0 ? (v1 < 255.0 ? v1 : 255.0) : 0;
			dst.at<Vec3b>(i, j)[idx0] = v0;
			dst.at<Vec3b>(i, j)[idx1] = v1;
		}
	}

}

