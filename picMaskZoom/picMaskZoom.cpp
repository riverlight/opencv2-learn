#include "picMaskZoom.h"
using namespace std;
using namespace cv;

static void pinchHorizontal(Mat& image1, Mat& image2, Point center, int w, int h, float strength);

static void DoOneEye(Mat& imgIn, Point center, int w, int h, float strength)
{
#if 1
	if (center.y < h)
		h = center.y;
	if (center.y + h>imgIn.rows)
		h = (imgIn.rows - center.y);
#endif
	Mat tmpImage = imgIn.clone();
	pinchHorizontal(tmpImage, imgIn, center, w, h, strength);
	tmpImage = imgIn.clone();
	pinchHorizontal(tmpImage, imgIn, center, w, h, strength);
}


void Pinch(Mat& img, Mat& dst, int degree)
{
    if  (degree<1 ) degree = 1;
    if ( degree> 32) degree = 32;
    
    if ( dst.empty())
        dst.create(img.rows, img.cols, img.type());
    
    dst = cv::Scalar::all(0);
    
    int chns = img.channels();
    int height = img.rows;
    int width = img.cols;
    
    int midX = width/2;
    int midY =height/2;
    
    int i, j, k;
    int X, Y, offsetX, offsetY;
    double radian, radius;  //弧和半径
    
    for ( i=0; i<height; i++)
    {
        for ( j=0; j<width; j++)
        {
            offsetX = j-midX;
            offsetY = i- midY;
            
            radian = atan2((double)offsetY, (double)offsetX);
            
            // 半径
            radius = sqrtf((float)(offsetX*offsetX + offsetY*offsetY));
            radius = sqrtf(radius)*degree;
            
            X = (int)(radius*cos(radian))+midX;
            Y = (int)(radius*sin(radian))+midY;
            
            if ( X<0 ) X = 0;
            if ( X>= width) X = width - 1;
            if ( Y<0 ) Y = 0;
            if ( Y>=height) Y = height -1;
            
            for ( k=0; k<chns; k++)
            {
                dst.ptr(i)[chns*j+k] = img.ptr(X)[chns*Y+k];
            }
        }
    }
}

void pinchHorizontal(Mat& image1, Mat& image2, Point center, int w, int h, float strength)
{
    int rows = image2.rows;
    int cols = image2.cols;
    
    if (strength < 0.0001) {
        return;
    }
    
	int wradius = w / 2;
	int hradius = h / 2;
	for (int i = -2 * hradius; i<2 * hradius; i++) {
        for(int j=-wradius;j<=wradius;j++) {
			int h_star = sqrt(hradius*hradius - j*j);
			if ((i < h_star + hradius && i >= 0)
				|| (i > -h_star - hradius && i <= 0)) {
                int new_i = i;
                if (i >= 0 && i < strength * h_star) {
                    new_i = i / strength;
                } else if (i <= 0 && i > -strength * h_star) {
                    new_i = i / strength;
                } else if (i >= 0 && i >= strength * h_star) {
					new_i = (i - strength * h_star) * hradius / (hradius + (1 - strength) * h_star) + h_star;
                } else if (i <= 0 && i <= -strength * h_star) {
					new_i = (i + strength * h_star) * hradius / (hradius + (1 - strength) * h_star) - h_star;
                }
                
                new_i = center.y + new_i;
                
                if (new_i >= rows) {
                    new_i = rows - 1;
                } else if (new_i < 0) {
                    new_i = 0;
                }
                
                image2.ptr(i+center.y)[3*(j+center.x)+0] = image1.ptr(new_i)[3*(j+center.x)+0];
                image2.ptr(i+center.y)[3*(j+center.x)+1] = image1.ptr(new_i)[3*(j+center.x)+1];
                image2.ptr(i+center.y)[3*(j+center.x)+2] = image1.ptr(new_i)[3*(j+center.x)+2];
            }
        }
    }
    
    return;
}

void copyRegion2Pos(Mat& image1, Mat& image2, Point top_left) {
    int rows = image2.rows;
    int cols = image2.cols;
    
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            if (image2.ptr(i)[4*j+3] == 0xff) {
                image1.ptr(i+top_left.y)[3*(j+top_left.x)+0] = image2.ptr(i)[4*j+0];
                image1.ptr(i+top_left.y)[3*(j+top_left.x)+1] = image2.ptr(i)[4*j+1];
                image1.ptr(i+top_left.y)[3*(j+top_left.x)+2] = image2.ptr(i)[4*j+2];
            }
        }
    }
    
    return;
}

void getRegion(Mat& image, Rect& region) {
    int rows = image.rows;
    int cols = image.cols;
    
    int top = rows-1;
    int left = cols-1;
    int bot = 0;
    int right = 0;
    
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            if (image.ptr(i)[4*j+3] == 0xff) {
                if (i < top) {
                    top = i;
                }
                
                if (i > bot) {
                    bot = i;
                }
                
                if (j < left) {
                    left = j;
                }
                
                if (j > right) {
                    right = j;
                }
            }
        }
    }
    
    printf("top = %d,left = %d,bot = %d, right = %d\n", top, left, bot, right);
    
    region.x = left;
    region.y = top;
    region.width = right-left+1;
    region.height = bot-top+1;
    
    return;
}

void mergeWithAlpha(Mat& valid_fore_img, Mat& valid_background_img) {
    int rows = valid_fore_img.rows;
    int cols = valid_fore_img.cols;
    
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            if (valid_fore_img.ptr(i)[4*j+3] == 0xff) {
                valid_fore_img.ptr(i)[4*j+2] = valid_background_img.ptr(i)[3*j+2];
                valid_fore_img.ptr(i)[4*j+1] = valid_background_img.ptr(i)[3*j+1];
                valid_fore_img.ptr(i)[4*j+0] = valid_background_img.ptr(i)[3*j+0];
            }
        }
    }
    
    return;
}

#if 1
int main(int argc, const char** argv){
	if (argc != 7 && argc != 10) {
		printf("%s input_image output_image time-s lcenter.x lcenter.y lradius [ rcenter.x rcenter.y rradius]\n", argv[0]);
		return -1;
	}

	Mat inputImage;
	Mat ouputImage;
	Mat tmpImage;

	string inputImageName;
	string outputImageName;
	float strength = 0.3;

	inputImageName.assign(argv[1]);
	outputImageName.assign(argv[2]);
	int framenum = atoi(argv[3]) * 15;

	if( inputImageName.size() ){
		if (inputImageName.find(".jpg")!=string::npos||inputImageName.find(".png")!=string::npos
			||inputImageName.find(".bmp")!=string::npos){
			inputImage = imread( inputImageName, 1 );
			if (inputImage.empty()){
				cout << "Read Image fail" << endl;
				return -1;
			}
		}
	}

	Size s = Size(inputImage.cols, inputImage.rows);
	VideoWriter outputV;
	int k = CV_FOURCC('x', 'v', 'i', 'd');
	//int k = cv.
	if (!outputV.open(outputImageName, k, 15, s))
		return -1;

	int count = 0;
	while (1)
	{
		if (count >= framenum)
			break;

		Mat imgSrc = inputImage.clone();
		
#if 1
		strength = 0.5;
#else
		strength += 0.1;
		if (strength > 1.0)
			strength = 0.3;
#endif

		{
			Point lcenter;
			lcenter.x = atoi(argv[4]);
			lcenter.y = atoi(argv[5]);
			int lradius = atoi(argv[6]);
			DoOneEye(imgSrc, lcenter, lradius * 2, lradius * 2, strength);
		}
		if (argc == 10)
		{
			Point rcenter;
			rcenter.x = atoi(argv[7]);
			rcenter.y = atoi(argv[8]);
			int rradius = atoi(argv[9]);
			DoOneEye(imgSrc, rcenter, rradius * 2, rradius * 2, strength);
		}

		outputV.write(imgSrc);
		count++;
	}

	//imwrite(outputImageName, ouputImage);
	outputV.release();

	return 0;
}

#else
int main( int argc, const char** argv ){
    if (argc != 6 && argc!=9) {
        printf("%s input_image output_image lcenter.x lcenter.y lradius [ rcenter.x rcenter.y rradius]\n", argv[0]);
        return 0;
    }
    
    Mat inputImage;
    Mat ouputImage;
    Mat tmpImage;
    
    string inputImageName;
    string outputImageName;
    float strength = 0.3;
    
    inputImageName.assign(argv[1]);
    outputImageName.assign(argv[2]);
    
    if( inputImageName.size() ){
        if (inputImageName.find(".jpg")!=string::npos||inputImageName.find(".png")!=string::npos
            ||inputImageName.find(".bmp")!=string::npos){
            inputImage = imread( inputImageName, 1 );
            if (inputImage.empty()){
                cout << "Read Image fail" << endl;
                return -1;
            }
        }
    }
	if (argc == 6)
	{
		Point lcenter;
		lcenter.x = atoi(argv[3]);
		lcenter.y = atoi(argv[4]);
		int lradius = atoi(argv[5]);
		DoOneEye(inputImage, ouputImage, lcenter, lradius, strength);
	}
	else
	{
		Point lcenter,rcenter;
		lcenter.x = atoi(argv[3]);
		lcenter.y = atoi(argv[4]);
		int lradius = atoi(argv[5]);
		rcenter.x = atoi(argv[6]);
		rcenter.y = atoi(argv[7]);
		int rradius = atoi(argv[8]);
		DoTwoEyes(inputImage, ouputImage, lcenter, lradius, rcenter, rradius, strength);
	}
	
    imwrite(outputImageName, ouputImage);
    
    return 0;
}
#endif
