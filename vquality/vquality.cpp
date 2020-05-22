#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>


using namespace cv;
using namespace std;

double psnr(Mat &I1, Mat &I2){
	Mat s1;
	absdiff(I1, I2, s1);
	s1.convertTo(s1, CV_32F);//转换为32位的float类型，8位不能计算平方  
	s1 = s1.mul(s1);
	Scalar s = sum(s1);  //计算每个通道的和  
	double sse = s.val[0] + s.val[1] + s.val[2];
	if (sse <= 1e-10) // for small values return zero  
		return 0;
	else
	{
		double mse = sse / (double)(I1.channels() * I1.total()); //  sse/(w*h*3)  
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}

double ssim(Mat &i1, Mat & i2){
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I1_2 = I1.mul(I1);
	Mat I2_2 = I2.mul(I2);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigam2_2, sigam12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigam2_2, Size(11, 11), 1.5);
	sigam2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigam12, Size(11, 11), 1.5);
	sigam12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigam12 + C2;
	t3 = t1.mul(t2);

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigam2_2 + C2;
	t1 = t1.mul(t2);

	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);

	double ssim = (mssim.val[0] + mssim.val[1] + mssim.val[2]) / 3;
	return ssim;
}

int vq_compare(char *file0, char *file1, int maxframenum)
{
	VideoCapture cap0, cap1;
	cap0.open(file0);
	cap1.open(file1);
	if (!cap0.isOpened() || !cap1.isOpened())
	{
		cout << "open file err" << endl;
		return -1;
	}

	double sum_psnr = 0;
	double sum_ssim = 0;
	int count = 0;
	Mat s0, s1;
	while (1)
	{
		cap0 >> s0;
		cap1 >> s1;
		cout << " time stamp : " << cap0.get(CV_CAP_PROP_POS_MSEC) << endl;
		if (s0.empty() || s1.empty())
			break;

		if (s0.cols != s1.cols || s0.rows != s1.rows)
			resize(s0, s0, s1.size());

		double f_psnr = 0;
		f_psnr = psnr(s0, s1);
		sum_psnr += f_psnr;
		//cout << f_psnr << endl;

		double f_ssim = 0;
		f_ssim = ssim(s0, s1);
		sum_ssim += f_ssim;
		//cout << f_ssim << endl;
		if ((maxframenum > 0) && (count == maxframenum))
			break;
		count++;
	}
	if (count == 0)
		cout << "0 0" << endl;
	else
		cout << sum_psnr / count << " " << sum_ssim / count << endl;

	return 0;
}

int main(int argc, char *argv[])
{
#if 0
	Mat m0, m1;
	m0 = imread("d:\\123.jpg");
	//imwrite("d:\\kk.jpg", m0);
	//return 0;
	m1 = imread("d:\\kk.jpg");

	Rect r = { 0, 750, 1280, 100 };
	m0(r) = 0;
	m1(r) = 0;

	if (m0.cols != m1.cols || m0.rows != m1.rows)
		resize(m0, m0, m1.size());

	cout << psnr(m0, m1) << endl;
	cout << ssim(m0, m1) << endl;
	return 0;
#else
	if (argc != 3 && argc!=4)
	{
		cout << "vquality v-file0 v-file1 [maxframenum]" << endl;
		return -1;
	}

	int maxfn = -1;
	if (argc == 4)
		maxfn = atoi(argv[3]);

	int ret;
	ret = vq_compare(argv[1], argv[2], maxfn);

	return ret;
#endif
}
