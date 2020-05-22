/*******************************************************************************
** Copyright (C), 	2016- , Zach Yeo.										   *
** File name:		Image_Noise_Estimation.cpp                                 *
** Description:                                                                *
** Author:			Zhang Shize <zszv587@gmail.com>                            *
**-----------------------------------------------------------------------------*
** History:                                                                    *
** v1.0				2016-10-11					                               *
********************************************************************************/

#include "Image_Noise_Estimation.h"

#if 0
/************************************************************
  Function:		main
  Description:	main function
  Output:		void
  Return:		void
  Calls:		Module_Input_Image
				Module_Sigma
  Others:		
************************************************************/
int main ( ){

	Mat img;//original image

	///*module to input the img*/
	//Module_Input_Image ( img );

	/*calculate the sigma's estimation and output some results to path "./image_name/"*/
	Module_Sigma ( img );

	
	return 0;

}
#endif

/************************************************************
  Function:		Module_Input_Image
  Description:	input an image to Mat & img and create a fol-
				der, file choosed by user input
  Input:		img	:	Mat to save the image
  Output:		void
  Return:		void
  Calls:		Error_Coding
  Others:		
************************************************************/
void Module_Input_Image ( Mat & img ){

	/*hello world*/
	cout << "Please Input the File Name of the Image You Want to Check WITHOUT the Extension:\n";
	cin >> name;

	/*read in the image*/
	img = imread ( name + FORMAT_JPEG, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR );

	/*abortion*/
	if ( img.data == NULL ){
	
		//ERR_CODE
	
	}
	
	/*create the folder*/
	_mkdir ( ( ( string ) FOLDER_HEAD + name ).c_str ( ) );

}

/************************************************************
  Function:		Module_Display_Image
  Description:	display an image 
  Input:		Windoe_Name	:	name showed in window title
				img	:	image to show out
  Output:		void
  Return:		void
  Calls:		Error_Coding		 
  Others:		
************************************************************/
void Module_Display_Image ( const string Window_Name, const Mat & img ){

	/*show the image*/
	if ( img.data != NULL ){

		imshow ( Window_Name, img );  

	}
	/*abortion*/
	else {
	
		//ERR_CODE
	
	}

	/*hello? until a 'q'*/
	while ( ( ( char ) waitKey ( ) ) != 'q' ){

	}

}

/************************************************************
  Function:		Module_Sobel
  Description:	sobel convolution of an image, in x and y di-
				rection, and sumed together after abs
  Input:		img	:	img to do sobel convolution
				img_sobel	:	img to save the result of so-
				bel convolution
  Output:		void
  Return:		void
  Calls:				 
  Others:		
************************************************************/ 
void Module_Sobel ( const Mat & img, Mat & img_sobel ){

	Mat img_grad_x;//gradient in x direction
	Mat img_grad_y;//gradient in y direction
	Mat img_grad_x_abs;//abs gradient in x direction
	Mat img_grad_y_abs;//abs gradient in y direction

	/*initialize the size*/
	img_grad_x = img.clone ( );
	img_grad_y = img.clone ( );
	img_grad_x_abs = img.clone ( );
	img_grad_y_abs = img.clone ( );
	
	/*calculate x direction*/
	Sobel ( img, img_grad_x, NATURE_DEPTH, 1, 0, SOBEL_KERNEL_SIZE, 1, 1, BORDER_DEFAULT );
	convertScaleAbs ( img_grad_x, img_grad_x_abs );
	Module_Save_Image ( img_grad_x_abs, STR ( img_grad_x_abs ) );

	/*calculate y direction*/
	Sobel ( img, img_grad_y, NATURE_DEPTH, 0, 1, SOBEL_KERNEL_SIZE, 1, 1, BORDER_DEFAULT );
	convertScaleAbs ( img_grad_y, img_grad_y_abs );
	Module_Save_Image ( img_grad_y_abs, STR ( img_grad_y_abs ) );

	/*merge x and y*/
	addWeighted ( img_grad_x_abs, 0.5, img_grad_y_abs, 0.5, 0, img_sobel );
	Module_Save_Image ( img_sobel, STR ( img_sobel ) );

}

/************************************************************
  Function:		Get_Jpeg_Params
  Description:	get jpeg parameters used for saving images in
				opencv, best quality
  Input:		params	:	vector to save jpeg params
  Output:		void
  Return:		void
  Calls:				 
  Others:		
************************************************************/ 
void Get_Jpeg_Params ( vector< int > & params ){

	/*params used to store image in JPEG format in opencv function*/
	params.push_back ( IMWRITE_JPEG_QUALITY );  
	params.push_back ( 100 );

}

/************************************************************
  Function:		Module_Laplacian
  Description:	Laplacian convolution of an image, 3*3 kernel
  Input:		img	:	Mat to save the result image
  Output:		void
  Return:		void
  Calls:		Error_Coding
				Convolution_Anchored_ij
  Others:		
************************************************************/ 
void Module_Laplacian ( const Mat & img ){

	/*abortion*/
	if ( img.rows < LAPLACIAN_KERNEL_SIZE || img.cols < LAPLACIAN_KERNEL_SIZE ){
	
		//ERR_CODE
	
	}

	Mat yuv;
	Mat y;
	vector< Mat > channels;

	cvtColor ( img, yuv, COLOR_BGR2YUV );
	
	split ( yuv, channels );

	/*only handle the y channel*/
	y = channels.at ( Y_CHANNEL );

	/*convolution caculation, here I delete two cols and two rows*/
	for ( int i = 1; i < y.rows - 1; i ++ ){
	
		for ( int j = 1; j < y.cols - 1; j ++ ){
		
			convolution_result [ i ][ j ] = Convolution_Anchored_ij ( y, i, j );
		
		}
	
	}

}

/************************************************************
  Function:		Convolution_Anchored_ij
  Description:	compute Laplacian at ( i, j ) pos of an image
  Input:		img	:	Mat to Laplacian convolution
				archor_i	:	ith row
				archor_j	:	jth col
  Output:		void
  Return:		value of the convolution at ( i, j ) pos
  Calls:			 
  Others:		
************************************************************/ 
int Convolution_Anchored_ij ( const Mat & img, const int anchor_i, const int anchor_j ){

	/*to make the calculation fast*/
	static const int offset_i [ CONVOLUTION_PIXELS ] = { -1, -1, -1,  0,  0,  0,  1,  1,  1 };
	static const int offset_j [ CONVOLUTION_PIXELS ] = { -1,  0,  1, -1,  0,  1, -1,  0,  1 };

	int res = 0;

	/*sum up nine pixel, each pixel three channel convolution results*/
	for ( int k = 0; k < CONVOLUTION_PIXELS; k ++ ){
	
		res += ( ( float ) ( img.at< uchar > ( anchor_i + offset_i [ k ], anchor_j + offset_j [ k ] ) ) ) * Laplacian_kernel [ k ];
	
	}

	return res;

}

/************************************************************
  Function:		Module_Save_Image
  Description:	save an image with given name in jpeg format
  Input:		img	:	image to save
				img_name	:	name of saved image
  Output:		void
  Return:		void
  Calls:		Get_Jpeg_Params 
  Others:		
************************************************************/ 
void Module_Save_Image ( Mat & img, string img_name ){

	/*get JPEG params*/
	vector< int > jpeg_params;
	Get_Jpeg_Params ( jpeg_params );

	/*save image*/
	imwrite ( ( ( ( string ) FOLDER_HEAD + name ) + BACK_SLASH ) + ( img_name + FORMAT_JPEG ), img, jpeg_params );

}

/************************************************************
  Function:		Module_Edge_Map
  Description:	to calculate the binary image, where black p-
				ixels have strong sobel value, and white ones
				the weak. followed by some steps to make the 
				seperation better.
  Input:		img_sobel	:	sovel convolution result
				img_edge_map	:	seperation result
  Output:		void
  Return:		void
  Calls:		Module_Color_Histogram
				Kick_Single_Point
  Others:		
************************************************************/ 
void Module_Edge_Map ( const Mat & img_sobel, Mat & img_edge_map ){

	/*initialize the size*/
	img_edge_map = Mat ( Size ( img_sobel.cols, img_sobel.rows ), CV_8U, SINGLE_CHANNEL );

	/*color histogram calculation, binary image out*/
	Module_Color_Histogram ( img_sobel, img_edge_map, COLOR_HISTOGRAM_THRESHOLD_RATIO );
	//Module_Save_Image ( img_edge_map, "edge_map_original" );

	/*kick out the single point*/
	Kick_Single_Point ( img_edge_map, KICK_SINGLE_THRESHOLD_REMAIN );
	//Module_Save_Image ( img_edge_map, "edge_map_without_single_point" );

	/*params to some morph function in opencv*/
	Mat element = getStructuringElement ( MORPH_RECT, Size( MORPH_KERNEL_SIZE, MORPH_KERNEL_SIZE ) );
	Mat tmp;
	
	/*some procedure to make the edge map better*/
	tmp = img_edge_map.clone ( );
	morphologyEx ( img_edge_map, tmp, MORPH_GRADIENT, element, CENTER_ARCHORED );
	bitwise_not ( tmp, img_edge_map );	
	tmp = img_edge_map.clone ( );
	//Module_Save_Image ( tmp, "morph_gradient" );
	
	/*erode procedure*/
	tmp = img_edge_map.clone ( );
	erode ( tmp, img_edge_map, element );
	//Module_Save_Image ( img_edge_map, "edgemap_final" );

}

/************************************************************
  Function:		Fill_Hole
  Description:	to fill white holes in a binary image
  Input:		img	:	img to fill
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Fill_Hole ( Mat & img ){

	/*first make a larger image*/
	Mat tmp;
	tmp = Mat ( img.rows + 2, img.cols + 2, CV_8UC1, Scalar ( PIXEL_WHITE ) );

	/*then copy the original image to the center of the larger one, with white edge*/
	img.copyTo ( tmp ( Range ( 1, img.rows + 1 ), Range ( 1, img.cols + 1 ) ) );

	/*floodfill any points in the white edge, here the ( 0, 0 )*/
	floodFill ( tmp, Point ( 0, 0 ), Scalar ( PIXEL_BLACK ) );

	/*cut the image out from the larger and floodfilled one*/
	Mat cut;
	tmp ( Range ( 1, img.rows + 1 ), Range ( 1, img.cols + 1 ) ).copyTo ( cut );

	/*bitwise 'not' and 'and', then, no inside hole*/
	img = img & ( ~ cut );

}

/************************************************************
  Function:		Erosion
  Description:	to replace some single black pixels with whi-
				te ones
  Input:		img	:	img to erode
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Erosion ( Mat & img ){

	/*temp Mat, because the kicking procedure is one by one, the former kicked one can effect the later ones*/
	Mat tmp;

	tmp = img.clone ( );

	/*if a point is edge and single point, then no longer edge*/
	for ( int i = 0; i < img.rows; i ++ ){
	
		for ( int j = 0; j < img.cols; j ++ ){

			if ( ( ( int ) tmp.at< uchar > ( i, j ) == PIXEL_BLACK ) && Is_Single_Point ( tmp, i, j ) ){
			
				img.at< uchar > ( i, j ) = PIXEL_WHITE;
			
			}

		}

	}

}

/************************************************************
  Function:		Kick_Single_Point
  Description:	kick out single black pixels
  Input:		img	:	img to kick
				thres	:	threshold when kicking
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Kick_Single_Point ( Mat & img, double thres ){

	Mat tmp;
	long long int remain;
	long long int remain_threshold;
	int exit_flag;

	remain = img.rows * img.cols;
	remain_threshold = thres * img.rows * img.cols;
	exit_flag = 0;

	/*kick until remain points is less enough, or no point is kicked in a iteration*/
	while ( remain > remain_threshold ){

		tmp = img.clone ( );
		remain = 0;
		exit_flag = 0;

		/*if a point is edge and single point, then no longer edge*/
		for ( int i = 0; i < img.rows; i ++ ){
	
			for ( int j = 0; j < img.cols; j ++ ){
		
				if ( ( int ) tmp.at< uchar > ( i, j ) == PIXEL_BLACK ){
			
					if ( Is_Single_Point ( tmp, i, j ) ){

						img.at< uchar > ( i, j ) = PIXEL_WHITE;
						exit_flag = 1;

					}
					else {
					
						remain ++;
					
					}

				}
		
			}
	
		}

		/*kick no point in a iteration, then exit*/
		if ( exit_flag == 0 ){
		
			break;
		
		}

	}

}

/************************************************************
  Function:		Is_Single_Point
  Description:	judge whether a pixel is a single point or n-
				ot
  Input:		img	:	img to judge
				anchor_i	:	ith row
				anchor_j	:	jth col
  Output:		void
  Return:		bool value
					true when pixel ( i, j ) is single 
					false when pixel ( i, j ) isn't single
  Calls:		
  Others:		
************************************************************/ 
bool Is_Single_Point ( const Mat & img, const int anchor_i, const int anchor_j ){
	
	/*accu stores amount of edge points around some consider point*/
	int accu = 0;
	static const int kernel_pixels = SINGLE_POINT_KERNEL_SIZE * SINGLE_POINT_KERNEL_SIZE;

	/*around neighbourhood of pixel ( i, j )*/
	for ( int i = ( anchor_i - SINGLE_POINT_KERNEL_SIZE / 2 >= 0 ? anchor_i - SINGLE_POINT_KERNEL_SIZE / 2 : 0 ); i < ( anchor_i + SINGLE_POINT_KERNEL_SIZE / 2 < img.rows ? anchor_i + SINGLE_POINT_KERNEL_SIZE / 2 : img.rows ); i ++ ){
	
		for ( int j = ( anchor_j - SINGLE_POINT_KERNEL_SIZE / 2 >= 0 ? anchor_j - SINGLE_POINT_KERNEL_SIZE / 2 : 0 ); j < ( anchor_j + SINGLE_POINT_KERNEL_SIZE / 2 < img.cols ? anchor_j + SINGLE_POINT_KERNEL_SIZE / 2 : img.cols ); j ++ ){
		
			/*if neighbouthood pixel is a edge pixel*/
			if ( ( int ) img.at< uchar > ( i, j ) == PIXEL_BLACK ){
			
				accu ++;

				/*neibourhood edge pixels enough*/
				if ( accu > kernel_pixels * SINGLE_POINT_THRESHOLD_RATIO ){
				
					return false;
				
				}
			
			}
		
		}
	
	}

	return true;

}

/************************************************************
  Function:		Module_Color_Histogram
  Description:	calculate a threshold of an image according 
				to the color histogram, then get a binary one 
				with this threshold
  Input:		img	:	sobel result of an image
				img_edge_map	:	to save the binary result
				thres_ratio	:	thres in color histogram
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Color_Histogram ( const Mat & img, Mat & img_edge_map, double thres_ratio ){

	/*done in y, new version*/
	Mat yuv;
	Mat y;
	vector< Mat > channels;

	/*histogram*/
	int Y_bin [ CV_8U_MAX + 1 ] = { ALL_ZERO };

	/*threshold*/
	int hist_threshold;
	hist_threshold = thres_ratio * ( double ) ( img.rows * img.cols );

	/*binary threshold, get from the histogram*/
	int anchor_Y = CV_8U_MAX;

	/*get the y channel*/
	cvtColor ( img, yuv, COLOR_BGR2YUV );
	split ( img, channels );
	y = channels.at ( Y_CHANNEL );

	/*calculate the color histogram in y channel*/
	Calcu_Color_Histogram ( y, Y_bin );

	/*calculate the binary threshold in y channel*/
	Calcu_Anchor ( Y_bin, anchor_Y, hist_threshold );

	/*binary procedure*/
	cv :: threshold ( y, img_edge_map, anchor_Y, PIXEL_WHITE, THRESH_BINARY_INV );

}

/************************************************************
  Function:		Calcu_Color_Histogram
  Description:	calculate color histogram of an image
  Input:		img	:	image to calculate
				hist	:	array to save the histogram
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Calcu_Color_Histogram ( const Mat & img, int * hist ){
	
	//cout << img.rows << img.cols << img.depth ( ) << img.data << img.channels ( );

	for ( int i = 0; i < img.rows; i ++ ){
	
		for ( int j = 0; j < img.cols; j ++ ){
			
			/*add one to the particular bin*/
			hist[ ( int ) ( img.at< uchar > ( i, j ) ) ] ++;
		
		}
	
	}

}

/************************************************************
  Function:		Calcu_Anchor
  Description:	calculate threshold according to histogram
  Input:		hist	:	histogram array
				anchor	:	save the threshold
				thres	:	threshold to get anchor
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Calcu_Anchor ( const int * hist, int & anchor, const int thres ){

	 int accu = 0;
	 
	 for ( int i = 0; i <= CV_8U_MAX; i ++ ){
	 
		 accu += hist[ i ];

		 /*anchor is binary threshold, only in particular percentage in the histogram*/
		 if ( accu >= thres ){
		
			anchor = i;
			break;

		 }
	 
	 }

}

/************************************************************
  Function:		Module_Noise_Estimation
  Description:	calculate the noise estimation
  Input:		img_edge_map	:	binary image
				Gaussian_sigma	:	estimation value
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Noise_Estimation ( const Mat & img_edge_map, double & Gaussian_sigma ){

	/*variance is all the squared error in no edge pixels' neighbourhood*/
	long long int variance = 0;
	int pixels = 0;
	
	for ( int i = 1; i < img_edge_map.rows - 1; i ++ ){
	
		for ( int j = 1; j < img_edge_map.cols - 1; j ++ ){
			
			if ( ( int ) img_edge_map.at< uchar > ( i, j ) == PIXEL_WHITE ){

				/*add the no edge pixel's squared error*/
				variance += abs ( convolution_result [ i ][ j ] );
				pixels ++;

			}

		}
	
	}	

	/*calculate the sigma estimation value*/
	Gaussian_sigma = ( ( double ) sqrt ( M_PI_2 ) * variance ) / ( 6 * pixels );

}

/************************************************************
  Function:		Module_Sigma
  Description:	head function that handling estimation of no-
				ise
  Input:		img	:	img to calculate
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Sigma ( Mat & img ){

	Mat img_sobel;//the Mat to store the image after sobel 
	Mat img_edge_map;//the Mat to store all the location of the edges, black pixel in edge position
	double Gaussian_sigma;
	vector< string > files;
	int size;
	string tmp;

	//getFiles ( FOLDER_HEAD, files );
	files.push_back("D:\\workroom\\testroom\\42536-006.jpg");
	size = files.size ( );
	cout << "size : " << size << endl;
	for ( int i = 0; i < size; i ++ ){

		tmp = files[ i ];

		if ( ( tmp.at ( tmp.length ( ) - 1 ) == 'g' && tmp.at ( tmp.length ( ) - 2 ) == 'p' && tmp.at ( tmp.length ( ) - 3 ) == 'j' ) || ( tmp.at ( tmp.length ( ) - 1 ) == 'p' && tmp.at ( tmp.length ( ) - 2 ) == 'm' && tmp.at ( tmp.length ( ) - 3 ) == 'b' ) ){

			//tmp.erase ( 0, 3 );

			img = imread ( tmp, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH );
			
			if ( img.data == NULL ){
			
				//err_code
				cout << "empty file error" << endl;
				system ( "pause" );
				exit ( 1 );
			
			}

			name = tmp;
			name.erase ( name.length ( ) - 4, 4 );

			///*histogram eaqulization*/
			//Module_Equalize_Histogram ( img );

			/*calculate the Laplacian of an image*/
			Module_Laplacian ( img );

			/*calculate the sobel of an image*/
			Module_Sobel ( img, img_sobel );

			/*calculate the edge_map of an image*/
			Module_Edge_Map ( img_sobel, img_edge_map );

			/*estimate the sigma of noises' Gaussian distribution*/
			Module_Noise_Estimation ( img_edge_map, Gaussian_sigma );

			/*output the sigma to some txt file*/
			Module_Output_sigma ( Gaussian_sigma );

			/*specially handle the wall if meanningful*/
			//Module_Wall_Sigma ( img_sobel );

		}

	}

}

/************************************************************
  Function:		Module_Sigma
  Description:	head function that handling estimation of no-
				ise specially in wall zone
  Input:		img_sobel	:	sobel result of image
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Wall_Sigma ( const Mat img_sobel ){

	Mat img_edge_map;
	double Gaussian_sigma;

	/*calculate the edge_map of the wall*/
	Module_Wall_Edge_Map ( img_sobel, img_edge_map );

	/*estimate the sigma of noises' Gaussian distribution*/
	//Module_Noise_Estimation ( img_edge_map, Gaussian_sigma );

	/*output the sigma to some txt file*/
	//Module_Output_sigma ( Gaussian_sigma );

}

/************************************************************
  Function:		Error_Coding
  Description:	handling the error code
  Input:		err_code	:	error code
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Error_Coding ( const int & err_code ){

	/*handle the error code, not finished yet*/
	system ( "pause" );
	exit ( 1 ); 

}

/************************************************************
  Function:		Module_Output_sigma
  Description:	output sigma to file
  Input:		Gaussian_sigma	:	noise estimation
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Output_sigma ( double & Gaussian_sigma ){

	/*create and open a txt file in particular path*/
	ofstream res_file;
	res_file.open ( ( string ) FOLDER_HEAD+ ( "res" + ( string ) FORMAT_TXT ), ios :: out | ios :: app );

	/*output the sigma*/
	res_file << name << "\t" << Gaussian_sigma << endl;
	cout << name << "\t" << Gaussian_sigma << endl;

	/*close the file*/
	res_file.close ( );

}

/************************************************************
  Function:		Module_Equalize_Histogram
  Description:	equalize the image, only in y channel
  Input:		img	:	img to equalize
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Equalize_Histogram ( Mat & img ){
	
	/*the yuv way*/

	Mat yuv;
	vector< Mat > channels;

	cvtColor ( img, yuv, COLOR_BGR2YUV );
	split ( yuv, channels );

	Mat y;
	y = channels.at ( Y_CHANNEL );

	equalizeHist ( y, y );

	channels.at ( Y_CHANNEL ) = y;

	cv :: merge ( channels, img );

	cvtColor ( img, img, COLOR_YUV2BGR );

	//Module_Save_Image ( img, "equalization");
	
}

/************************************************************
  Function:		Module_Wall_Edge_Map
  Description:	to calculate the binary image, where black p-
				ixels have strong sobel value, and white ones
				the weak. followed by some steps to make the 
				seperation better. only in wall zone
  Input:		img_sobel	:	sobel result of image
				img_edge_map	:	output binary image
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/ 
void Module_Wall_Edge_Map ( const Mat & img_sobel, Mat & img_edge_map ){

	/*initialize the size*/
	img_edge_map = Mat ( Size ( img_sobel.cols, img_sobel.rows ), CV_8U, SINGLE_CHANNEL );

	/*color histogram calculation, binary image out*/
	Module_Color_Histogram ( img_sobel, img_edge_map, COLOR_HISTOGRAM_THRESHOLD_RATIO );

	/*kick out the single point*/
	Kick_Single_Point ( img_edge_map, WALL_KICK_SINGLE_THRESHOLD_REMAIN );

	/*params to some morph function in opencv*/
	Mat element = getStructuringElement ( MORPH_RECT, Size( WALL_MORPH_KERNEL_SIZE, WALL_MORPH_KERNEL_SIZE ) );
	Mat tmp;
	
	/*some procedure to make the edge map better*/
	tmp = img_edge_map.clone ( );
	morphologyEx ( img_edge_map, tmp, MORPH_GRADIENT, element, CENTER_ARCHORED );
	bitwise_not ( tmp, img_edge_map );	
	tmp = img_edge_map.clone ( );
	
	/*erode procedure*/
	tmp = img_edge_map.clone ( );
	erode ( tmp, img_edge_map, element );

	/*fill the holes inside the image*/
	tmp = img_edge_map.clone ( );
	Fill_Hole ( tmp );
	//Module_Save_Image ( tmp, "edgemap_fill_hole");
	
}

/************************************************************
  Function:		getfiles
  Description:	get all files' name in path, iteratively
  Input:		path	:	path
				files	:	string vector that store names
  Output:		void
  Return:		void
  Calls:		
  Others:		
************************************************************/
void getFiles ( const string path, vector< string > & files ){
 
    long hFile = 0;
    struct _finddata_t fileinfo;
    string p;

    if ( ( hFile = _findfirst ( p.assign ( path ).append ( "\\*" ).c_str ( ), & fileinfo ) ) != -1 ){
        
		do{  

            if ( ( fileinfo.attrib & _A_SUBDIR ) ){  
                
				if ( strcmp ( fileinfo.name, "." ) != 0 && strcmp ( fileinfo.name, ".." ) != 0 ){

                    getFiles ( p.assign ( path ).append ( "\\" ).append ( fileinfo.name ), files ); 

				}

            }  
            else{  
                
				files.push_back ( p.assign ( path ).append ( "\\" ).append ( fileinfo.name ) );

            }  
        }while ( _findnext ( hFile, & fileinfo ) == 0 );

        _findclose(hFile);

    }

}  