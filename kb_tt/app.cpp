#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>

using namespace cv;
using namespace std;

#define THRESHOLD_MAX_VALUE 255

int g_debugmode = 0;

typedef struct HitInfo_s
{
	int _nKeyDown_fn;
	double _dKeyDown_ts;

	int _nTextOut_fn;
	double _dTextOut_ts;
} HitInfo;

typedef struct WarnInfo_s
{
	int _nKeyDown_fn;
	double _dKeyDown_ts;
} WarnInfo;

void resize_crop(Mat &src, Mat &dst, int nDstWidth=256)
{
	int nDstHeight = nDstWidth * src.rows / src.cols;
	resize(src, dst, Size(nDstWidth, nDstHeight));
}

int isKeyDown(Mat imgDiff)
{
	int bKeyDown = 0;
	Rect r(0, imgDiff.rows-156, 256, 156);
	Mat img_kb = imgDiff(r).clone();
	//imshow("3", img_kb);

	int notzero_count = sum(img_kb)[0] / THRESHOLD_MAX_VALUE;
	//std::cout << "notzero_count : " << notzero_count << endl;
	if (notzero_count > 256 * 3 && notzero_count < 256 * 10)
		bKeyDown = 1;

	return bKeyDown;
}

int isTextOut(Mat imgDiff)
{
	int bTextOut = 0;
	Rect r(0, imgDiff.rows - 190, 256, 30);
	Mat img_to = imgDiff(r).clone();
	//imshow("3", img_to);

	int notzero_count = sum(img_to)[0] / THRESHOLD_MAX_VALUE;
	//std::cout << "TEXTOUT notzero_count : " << notzero_count << endl;
	if (notzero_count > 256 * 0.1 && notzero_count < 256 * 7)
		bTextOut = 1;

	return bTextOut;
}

void record_hitinfo_and_warning(vector<int> &v_kd, vector<int> &v_to, vector<double> &v_ts, string outfile)
{
	vector<HitInfo> v_hitinfo;
	vector<WarnInfo> v_warninfo;

	int b_kd_status = 0;
	int current_kd;
	for (int i = 0; i < v_kd.size(); i++)
	{
		if (i < 30 || i>v_kd.size()-30)
			continue;

		if (v_kd[i]>0 && v_kd[i - 1] == 0)
		{
			b_kd_status = 1;
			current_kd = i;
		}

		if (b_kd_status != 0)
		{
			if (v_to[i] > 0 && v_to[i - 1] == 0)
			{
				HitInfo hi;
				hi._nKeyDown_fn = current_kd;
				hi._dKeyDown_ts = v_ts[current_kd];
				hi._nTextOut_fn = i;
				hi._dTextOut_ts = v_ts[i];
				v_hitinfo.push_back(hi);

				b_kd_status = 0;
				cout << "hit" << endl;
			}
			else if (i - current_kd > 10)
			{
				WarnInfo wi;
				wi._nKeyDown_fn = current_kd;
				wi._dKeyDown_ts = v_ts[current_kd];
				v_warninfo.push_back(wi);

				b_kd_status = 0;
				cout << "warning" << current_kd << " " << v_ts[current_kd] << " " << i << endl;
			}
			else
			{

			}
		}
	}

	ofstream outFile(outfile);
	for (int i = 0; i < v_hitinfo.size(); i++)
	{
		outFile << "hitinfo," << v_hitinfo[i]._nKeyDown_fn << "," << v_hitinfo[i]._nTextOut_fn << "," << \
			v_hitinfo[i]._dKeyDown_ts << "," << v_hitinfo[i]._dTextOut_ts << "," << \
			v_hitinfo[i]._nTextOut_fn - v_hitinfo[i]._nKeyDown_fn << "\n";
	}
	for (int i = 0; i < v_warninfo.size(); i++)
	{
		outFile << "warninfo," << v_warninfo[i]._nKeyDown_fn << "," << v_warninfo[i]._dKeyDown_ts << "\n";
	}
}


int Process(char *videofile, char *outfile)
{
	VideoCapture cap;
	cap.open(videofile);
	if (!cap.isOpened())
	{
		std::cout << " cap err" << endl;
		return -1;
	}

	int count = 0;
	Mat imgCurrent;
	Mat imgLast_g;
	
	vector<int> v_kd, v_to;
	vector<double> v_ts;
	while (1)
	{
		int bKB_down = 0, bText_out = 0;
		double timestamp_ms = cap.get(CV_CAP_PROP_POS_MSEC);
		v_ts.push_back(timestamp_ms);
		cap >> imgCurrent;
		if (imgCurrent.empty())
		{
			break;
		}

		Mat imgCurrent_g;
		cvtColor(imgCurrent, imgCurrent_g, CV_BGR2GRAY);
		resize_crop(imgCurrent_g, imgCurrent_g);
		if (count != 0)
		{
			Mat imgDiff;
			subtract(imgLast_g, imgCurrent_g, imgDiff);
			abs(imgDiff);
			threshold(imgDiff, imgDiff, 16, THRESHOLD_MAX_VALUE, THRESH_BINARY);
			
			if (isKeyDown(imgDiff) != 0)
			{
				bKB_down = 1;
				//std::cout << "keydown" << endl;
			}
			if (isTextOut(imgDiff) != 0)
			{
				bText_out = 1;
				//std::cout << "text out" << endl;
			}
			//if (count > 2600)
			if (g_debugmode!=0)
			{
				std::cout << count << endl;
				imshow("1", imgDiff);
				imshow("2", imgCurrent_g);
				waitKey();
			}
			
		}
		v_kd.push_back(bKB_down);
		v_to.push_back(bText_out);
		//std::cout << count << endl;

		imgLast_g = imgCurrent_g.clone();
		count++;
	}
	record_hitinfo_and_warning(v_kd, v_to, v_ts, outfile);

	return 0;
}

int main(int argc, char *argv[])
{
	std::cout << "Hi, this is keyboard test tool program" << endl;

	if (argc != 3 && argc!=4)
	{
		std::cout << "Usage: kk_tt videofile outfile" << endl;
		return -1;
	}
	if (argc == 4)
		g_debugmode = 1;
	
	return Process(argv[1], argv[2]);
}
