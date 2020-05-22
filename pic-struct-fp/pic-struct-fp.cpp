#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

void Mat_2_structfp(Mat &m, int structfp[48*48])
{
	Mat m_48;
	if (m.cols != 48 || m.rows != 48)
		resize(m, m_48, Size(48, 48));
	else
		m_48 = m.clone();
	cvtColor(m_48, m_48, CV_BGR2GRAY);

	Mat imgThres;
	threshold(m_48, imgThres, 255, 1, THRESH_OTSU);

	for (int i = 0; i < 48; i++)
		for (int j = 0; j < 48; j++)
			structfp[i * 48 + j] = imgThres.at<uchar>(i, j);
	//imshow("88", imgThres);
	//waitKey();
}

int calc_structfp_score(int fp0[48 * 48], int fp1[48 * 48])
{
	int score = 0;
	for (int i = 0; i < 48 * 48; i++)
	{
		if (fp0[i] == fp1[i])
			score++;
	}
	return score;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "error input" << endl;
		return -1;
	}
		
	Mat m0 = imread(argv[1]);
	Mat m1 = imread(argv[2]);

	int structfp0[48 * 48], structfp1[48 * 48];
	Mat_2_structfp(m0, structfp0);
	Mat_2_structfp(m1, structfp1);

	int score;
	score = calc_structfp_score(structfp0, structfp1);
	cout << "score : " << float(score)/(48*48) << endl;

	return 0;
}

