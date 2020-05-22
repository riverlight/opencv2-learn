﻿#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <direct.h>
#include <io.h>

#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

void getconvhv(Mat &kerconvhv, const Mat filtker, const int heigth, const int width)
{
	Mat kerconvh = Mat::zeros(35, 49, CV_32F);
	Mat kerconvv = Mat::zeros(35, 49, CV_32F);
	int rowcount = 0;
	for (int i = 0; i < 7; i++){
		for (int j = 0; j < 5; j++){
			for (int k = 0; k < 3; k++){
				kerconvh.at<float>(rowcount, i*width + j + k) = filtker.at<float>(0, k);
			}
			rowcount = rowcount + 1;
		}
	}
	int colcount = 0;
	for (int i = 0; i < 5; i++){
		for (int j = 0; j < 7; j++){
			for (int k = 0; k < 3; k++){
				kerconvv.at<float>(colcount, (i + k)*width + j) = filtker.at<float>(0, k);
			}
			colcount = colcount + 1;
		}
	}
	kerconvhv = kerconvh.t() * kerconvh + kerconvv.t() * kerconvv;
}

void getgaminv(const float para_1, const float para_2, const float locat, float &result)
{
	result = 81.8208f;
}

void im2col(const cv::Mat inmat, int patchheigth, int patchwidth, cv::Mat &outmat)
{
	for (int i = 0; i < inmat.cols - patchwidth - 1; i++){
		for (int j = 0; j < inmat.rows - patchheigth - 1; j++) {
			for (int m = 0; m < patchwidth; m++){
				for (int n = 0; n < patchheigth; n++){
					//if (i==715 && j==156)
					//	cout << i << " " << j << " " << m << " " << n << endl;
					outmat.at<float>(m * patchheigth + n, i * (inmat.rows - patchheigth + 1) + j) = inmat.at<float>(j + n, i + m);
				}
			}
		}
	}
}

int main(int argc, char* argv[])
{
	vector<float> noiselevellist;
	for (int i = 0; i <= 45; i++){
		noiselevellist.push_back((float)i + 5.0f);
	}
	Mat oriimage = imread("D:\\workroom\\testroom\\42536-006.jpg");
	ofstream outfile("D:\\workroom\\testroom\\wood.txt", ios::trunc);
	outfile.close();
	//FileStorage noiselevelfile("noiseleve.yaml", FileStorage::WRITE);
	//for (int noiselevellistcount = 0; noiselevellistcount < noiselevellist.size(); noiselevellistcount++)
	int noiselevellistcount = 30;
	{
		// save the estimate noise level
		vector<float> noisesigmalist;
		int kerconvhvrank = 0;
		float noisesigma = 0;
		float threholdtau = 0;
		float initthreholdtau = 10000;
		vector<string> imagechanles;
		imagechanles.push_back("blue");
		imagechanles.push_back("green");
		imagechanles.push_back("red");
		// image filter acquire
		Mat noise;
		Mat image;
		Mat filtimage;
		Mat sigularvalue;
		Mat filtker = (Mat_<float>(1, 3) << -0.5, 0, 0.5);
		Mat kerconvhv(cv::Size(49, 49), CV_32F);
		oriimage.convertTo(image, CV_32F, 1, 0);
		getconvhv(kerconvhv, filtker, 7, 7);
		SVD::compute(kerconvhv, sigularvalue);
		for (int i = 0; i < sigularvalue.rows; i++){
			if (sigularvalue.at<float>(i, 0) >= 1e-3){ kerconvhvrank++; }
		}
		getgaminv(1.0f, float(kerconvhvrank) / 2.0f, (2.0f*trace(kerconvhv)(0)) / float(kerconvhvrank), initthreholdtau);
		noise.create(image.size(), CV_32FC3);
		randn(noise, 0, noiselevellist.at(noiselevellistcount));
		Mat stdnoise;
		Mat meannoise;
		meanStdDev(noise, meannoise, stdnoise);
		image = image + noise;
		// image preproccessing  
		filter2D(image, filtimage, image.depth(), filtker, Point(-1, -1), 0, BORDER_REPLICATE);
		Mat imageh = filtimage(Range(1, image.rows - 1), Range::all());
		Mat imagehh = imageh.mul(imageh);
		filter2D(image, filtimage, image.depth(), filtker.t(), Point(-1, -1), 0, BORDER_REPLICATE);
		Mat imagev = filtimage(Range::all(), Range(1, image.cols - 1));
		Mat imagevv = imagev.mul(imagev);
		vector<Mat> imagechanlesh;
		split(imagehh, imagechanlesh);
		vector<Mat> imagechanlesv;
		split(imagevv, imagechanlesv);
		vector<Mat> imagechanleshv;
		split(image, imagechanleshv);
		for (int i = 0; i < 3; i++){
			Mat imagecolh = Mat::zeros(35, ((image.rows - 6) * (image.cols - 6)), CV_32F);
			Mat imagecolv = Mat::zeros(35, ((image.rows - 6) * (image.cols - 6)), CV_32F);
			Mat imagecolhv = Mat::zeros(49, ((image.rows - 6) * (image.cols - 6)), CV_32F);
			Mat sigleimageh = imagechanlesh.at(i);
			Mat sigleimagev = imagechanlesv.at(i);
			Mat sigleimagehv = imagechanleshv.at(i);
			im2col(sigleimageh, 7, 5, imagecolh);
			im2col(sigleimagev, 5, 7, imagecolv);
			im2col(sigleimagehv, 7, 7, imagecolhv);
			Mat imagecolhpv;
			vconcat(imagecolh, imagecolv, imagecolhpv);
			Mat imagecoltrace;
			reduce(imagecolhpv, imagecoltrace, 0, CV_REDUCE_SUM);
			Mat imagecovmat = (imagecolhv * imagecolhv.t()) / (imagecolhv.cols - 1);
			Mat imagecoveigenvalue;
			Mat imagecoveigenvector;
			eigen(imagecovmat, imagecoveigenvalue, imagecoveigenvector);
			noisesigma = imagecoveigenvalue.at<float>(imagecoveigenvalue.rows - 1, 0);
			vector<Mat> imagetrace;
			imagetrace.push_back(imagecoltrace);
			vector<Mat> imagecovmats;
			imagecovmats.push_back(imagecolhv);
			for (int itertime = 0; itertime < 2; itertime++){
				threholdtau = initthreholdtau * noisesigma;
				vector<Mat> rightcolmat;
				vector<float> rightelement;
				Mat insideimagecovmat(imagecovmats.at(itertime));
				Mat insideimagecoltrace(imagetrace.at(itertime));
				for (int i = 0; i < insideimagecoltrace.cols; i++){
					if (insideimagecoltrace.at<float>(0, i) < threholdtau){
						rightelement.push_back(insideimagecoltrace.at<float>(0, i));
						rightcolmat.push_back(insideimagecovmat(Range::all(), Range(i, i + 1)));
					}
				}
				Mat tempvec(rightelement);
				imagetrace.push_back(tempvec.t());
				// vector mat for mat 
				Mat tempmat = Mat::zeros(49, rightcolmat.size(), CV_32F);
				for (int i = 0; i < tempmat.rows; i++){
					for (int j = 0; j < tempmat.cols; j++){
						tempmat.at<float>(i, j) = rightcolmat.at(j).at<float>(i, 0);
					}
				}
				imagecovmats.push_back(tempmat);
				// noise level estimation
				Mat insideimagecovmatx = (tempmat * tempmat.t()) / (tempmat.cols - 1);
				Mat insideimagecoveigenvalue;
				Mat insideimagecoveigenvector;
				eigen(insideimagecovmatx, insideimagecoveigenvalue, insideimagecoveigenvector);
				noisesigma = insideimagecoveigenvalue.at<float>(insideimagecoveigenvalue.rows - 1, 0);
				if (itertime == 1){
					noisesigmalist.push_back(sqrt(noisesigma));
				}
			}
		}
		Mat noiseestimation(1, noisesigmalist.size(), CV_32F);
		for (int i = 0; i < noisesigmalist.size(); i++){
			noiseestimation.at<float>(0, i) = noisesigmalist.at(i);
		}
		outfile.open("D:\\workroom\\testroom\\wood.txt", ios::app | ios::binary);
		outfile << "std : " << stdnoise.t() << endl;
		outfile << "est : " << noiseestimation << endl;
		outfile.close();
		
		cout << noiselevellistcount << " " << noiseestimation << endl;
		cout << stdnoise.t() << endl;
		//noiselevelfile << "std" << stdnoise;
		//noiselevelfile << "est" << noiseestimation;
	}
	//noiselevelfile.release();
	return 0;
}
