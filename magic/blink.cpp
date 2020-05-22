#include "blink.h"

static void pinchHorizontal(Mat& image1, Mat& image2, Point center, int w, int h, float strength);
static void leonBlink(Mat& image1, Mat& image2, Point center, int w, int h, float strength);

void blink(Mat& imgIn, Point center, int w, int h, float strength)
{
#if 1
	if (center.y < h)
		h = center.y;
	if (center.y + h>imgIn.rows)
		h = (imgIn.rows - center.y);
#endif

	Mat tmpImage = imgIn.clone();
	pinchHorizontal(imgIn, tmpImage, center, w, h, strength);
	pinchHorizontal(tmpImage, imgIn, center, w, h, strength);
}

void leonBlink(Mat& image1, Mat& image2, Point center, int w, int h, float strength)
{
	int rows = image2.rows;
	int cols = image2.cols;
	
	int wradius = w / 2;
	int hradius = h / 2;
	for (int i = -1 * hradius; i<1 * hradius; i++) {
		for (int j = -wradius; j <= wradius; j++) {
			image2.ptr(i + center.y)[3 * (j + center.x) + 0] = image1.ptr(center.y-hradius)[3 * (j + center.x) + 0];
			image2.ptr(i + center.y)[3 * (j + center.x) + 1] = image1.ptr(center.y - hradius)[3 * (j + center.x) + 1];
			image2.ptr(i + center.y)[3 * (j + center.x) + 2] = image1.ptr(center.y - hradius)[3 * (j + center.x) + 2];
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
		for (int j = -wradius; j <= wradius; j++) {
			int h_star = sqrt(hradius*hradius - j*j);
			if ((i < h_star + hradius && i >= 0)
				|| (i > -h_star - hradius && i <= 0)) {
				int new_i = i;
				if (i >= 0 && i < strength * h_star) {
					new_i = i / strength;
				}
				else if (i <= 0 && i > -strength * h_star) {
					new_i = i / strength;
				}
				else if (i >= 0 && i >= strength * h_star) {
					new_i = (i - strength * h_star) * hradius / (hradius + (1 - strength) * h_star) + h_star;
				}
				else if (i <= 0 && i <= -strength * h_star) {
					new_i = (i + strength * h_star) * hradius / (hradius + (1 - strength) * h_star) - h_star;
				}

				new_i = center.y + new_i;

				if (new_i >= rows) {
					new_i = rows - 1;
				}
				else if (new_i < 0) {
					new_i = 0;
				}

				image2.ptr(i + center.y)[3 * (j + center.x) + 0] = image1.ptr(new_i)[3 * (j + center.x) + 0];
				image2.ptr(i + center.y)[3 * (j + center.x) + 1] = image1.ptr(new_i)[3 * (j + center.x) + 1];
				image2.ptr(i + center.y)[3 * (j + center.x) + 2] = image1.ptr(new_i)[3 * (j + center.x) + 2];
			}
		}
	}

	return;
}
