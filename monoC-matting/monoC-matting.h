#ifndef MONOC_MATTING_H
#define MONOC_MATTING_H

#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#ifndef L_IN
#define L_IN
#define L_OUT
#define L_INOUT
#endif /* L_IN */

/*****************************
	*
	* flagChrama£º 
	*	0£ºblue
	*	1£ºgreen
	*	2£ºred
*****************************/
int monoC_matting(L_INOUT Mat &imgObj, L_IN Mat imgBG, int flagChroma, int thres=20);
int dbgMatting(L_INOUT Mat &imgObj, L_IN Mat imgBG, int flagChroma, int thres);

int videoMonoCMatting_image(char *infile, char *bgfile, char *outfile, int color, int thres, Rect r);
int videoMonoCMatting_video2(char *infile, char *bgfile, char *outfile, int color, int thres, Rect r);

#endif /* MONOC_MATTING_H */
