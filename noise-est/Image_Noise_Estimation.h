/*******************************************************************************
** Copyright (C), 	2016- , Zach Yeo.                                          *
** File name:		Image_Noise_Estimation.h                                   *
** Description:                                                                *
** Author:			Zhang Shize <zszv587@gmail.com>                            *
**-----------------------------------------------------------------------------*
** History:                                                                    *
** v1.0				2016-10-11					                               *
********************************************************************************/

#define _CRT_SECURE_NO_WARNINGS//special for visual studio

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <direct.h>
#include <io.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*ordinary define*/
#define NATURE_DEPTH -1//use the default depth of img
#define SOBEL_KERNEL_SIZE 3//3 * 3 kernel
#define LAPLACIAN_KERNEL_SIZE 3//3 * 3 kernel
#define STR(S) #S//get the name of varible
#define FORMAT_JPEG ".jpg"//extension name
#define FORMAT_TXT ".txt"//extension name
#define CENTER_ARCHORED ( Point ( -1, -1 ) )//center archored when convolution
#define SINGLE_CHANNEL 1
#define TRIPLE_CHANNEL 3
#define CV_8U_MAX 255//max pixel value in format CV_8U from opencv
#define RED_CHANNEL 2//red channel offset
#define GREEN_CHANNEL 1//green channel offset
#define BLUE_CHANNEL 0//blue channel offset
#define Y_CHANNEL 0//y channel offset in yuv
#define ALL_ZERO 0//used to initialize matrix
#define COLOR_HISTOGRAM_THRESHOLD_RATIO 0.85//threshold in color histogram, smaller gradient strength than this will be considered not edge
#define M_PI_2 1.57079632679489661923/* PI * 1 / 2 */
#define M_2_PI 6.2831853071795864769252866/* PI * 2 */
#define PIXEL_BLACK 0//pixel value
#define PIXEL_WHITE 255//pixel value
#define CONVOLUTION_PIXELS 9//how many pixels in a convoltion kernel
#define SINGLE_POINT_KERNEL_SIZE 3//5 * 5 kernel
#define SINGLE_POINT_THRESHOLD_RATIO 0.3//with less neighbourhood points, more posibility to be single point, here is the judge threshold
#define KICK_SINGLE_THRESHOLD_REMAIN 0.05//keep this many points when kicking single points out
#define WALL_KICK_SINGLE_THRESHOLD_REMAIN 0.070//keep this many points when kicking single points out, specially for wall zone
#define QUANTIFICATION_THRESHOLD 0.2//keep this many points when the final step in edge_map calculation
#define MORPH_KERNEL_SIZE 5//kernel size in morph params
#define WALL_MORPH_KERNEL_SIZE 15//kernel size in morph params specially for wall zone
#define FOLDER_HEAD "D:\\workroom\\testroom\\"
#define BACK_SLASH "\\"

/*error code define*/
#define ERR_CODE_IMAGE_SIZE_SMALL 00001//even smaller than the convolution kernel
#define ERR_CODE_IMAGE_OPEN_FAIL  00002//fail to open the iamge

string name;//image name without extension, also used to create the result folder

/*store the result of Laplacian convolution, range from ( 1, 1 ) to ( img.rows, img.cols )*/
int convolution_result [ 5000 ][ 5000 ] = { };

/*kernels*/
const float Laplacian_kernel[9] = 
{					
	 1, -2,  1 ,	
	-2,  4, -2 ,	
     1, -2,  1 
};

const float sobel_x[3][3] = 
{
	{ -1, -2, -1 },
	{  0,  0,  0 },
	{  1,  2,  1 }
};

const float sobel_y[3][3] = 
{
	{ -1,  0,  1 },
	{ -2,  0,  2 },
	{ -1,  0,  1 }
};

/*function declaration*/
void Module_Input_Image ( Mat & img );//module to input the img
void Module_Display_Image ( const string Window_Name, const Mat & img );//module to show the image
void Module_Sobel ( const Mat & img, Mat & img_sobel );//module to calculate sobel
void Get_Jpeg_Params ( vector< int > & params );//get the params used to store image
void Module_Laplacian ( const Mat & img );//module to calculate the Laplacian
void Module_Save_Image ( Mat & img, string name );//module to save the image
void Module_Edge_Map ( const Mat & img, Mat & img_edge_map );//module to calculate the edge_map
void Module_Color_Histogram ( const Mat & img, Mat & img_edge_map, double thres_ratio );//module to calculate the color histogram
void Calcu_Color_Histogram ( const Mat & img, int * hist );//sub-function of Module_Color_Histogram
void Calcu_Anchor ( const int * hist, int & anchor, const int thres );//calculate a self-adaption threshold
void Module_Noise_Estimation ( const Mat & img_edge_map, double & Gaussian_sigma );//module to calculate the sigma of noise
int Convolution_Anchored_ij ( const Mat & img, const int anchor_i, const int anchor_j );//calculate the convolution at position ( i, j )
void Module_Sigma ( Mat & img );//module to calculate the sigma of noise, a head module
void Error_Coding ( const int & err_code );//module to handle error code
void Kick_Single_Point ( Mat & img, double thres );//kick out single point
bool Is_Single_Point ( const Mat & img, const int anchor_i, const int anchor_j );//judge a single point 
void Erosion ( Mat & img );//erode an image
void Fill_Hole ( Mat & img );//fill all the hole inside the image
void Module_Output_sigma ( double & gaussian_sigma );//output the sigma to a txt file
void Module_Equalize_Histogram ( Mat & img );//a image color histogram equalization procedure on each channel
void Module_Wall_Sigma ( const Mat img_sobel );//calculat the sigma on the wall zone
void Module_Wall_Edge_Map ( const Mat & img, Mat & img_edge_map );//module to calculate the edge_map on wall zone
void getFiles ( const string path, vector< string > & files );//get all file name in the root folder iteratively