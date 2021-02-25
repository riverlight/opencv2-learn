
#include "hdr.h"

static Mat guidedFilter(cv::Mat& I, cv::Mat& p, int r, float eps);


void ALTMRetinex(const Mat& src, Mat& dst, bool LocalAdaptation , bool ContrastCorrect)
{

    Mat temp, src_gray;

    src.convertTo(temp, CV_32FC3);
    //灰度图
    cvtColor(temp, src_gray, COLOR_BGR2GRAY);

    double LwMax;
    //得到最大值
    minMaxLoc(src_gray, NULL, &LwMax);

    Mat Lw_;
    const int num = src.rows * src.cols;
    //计算每个数组元素绝对值的自然对数
    cv::log(src_gray + 1e-3f, Lw_);
    //矩阵自然指数
    float LwAver = exp(cv::sum(Lw_)[0] / num);

    Mat Lg;
    log(src_gray / LwAver + 1.f, Lg);
    //矩阵除法
    cv::divide(Lg, log(LwMax / LwAver + 1.f), Lg);

    //局部自适应
    Mat Lout;
    if (LocalAdaptation)
    {
        int kernelSize = floor(std::max(3, std::max(src.rows / 100, src.cols / 100)));
        Mat Lp, kernel = cv::getStructuringElement(MORPH_RECT, Size(kernelSize, kernelSize));
        cv::dilate(Lg, Lp, kernel);
        Mat Hg = guidedFilter(Lg, Lp, 10, 0.01f);

        double eta = 36;
        double LgMax;
        cv::minMaxLoc(Lg, NULL, &LgMax);
        Mat alpha = 1.0f + Lg * (eta / LgMax);

        Mat Lg_;
        cv::log(Lg + 1e-3f, Lg_);
        float LgAver = exp(cv::sum(Lg_)[0] / num);
        float lambda = 10;
        float beta = lambda * LgAver;

        cv::log(Lg / Hg + beta, Lout);
        cv::multiply(alpha, Lout, Lout);
        cv::normalize(Lout, Lout, 0, 255, NORM_MINMAX);
    }
    else
    {
        cv::normalize(Lg, Lout, 0, 255, NORM_MINMAX);
    }

    Mat gain(src.rows, src.cols, CV_32F);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            float x = src_gray.at<float>(i, j);
            float y = Lout.at<float>(i, j);
            if (0 == x) gain.at<float>(i, j) = y;
            else gain.at<float>(i, j) = y / x;
        }
    }

    Mat bgr[3];
    cv::split(temp, bgr);
    if (ContrastCorrect)
    {
        // 校正图像对比度
        bgr[0] = (gain.mul(bgr[0] + src_gray) + bgr[0] - src_gray) * 0.5f;
        bgr[1] = (gain.mul(bgr[1] + src_gray) + bgr[1] - src_gray) * 0.5f;
        bgr[2] = (gain.mul(bgr[2] + src_gray) + bgr[2] - src_gray) * 0.5f;
    }
    else
    {
        cv::multiply(bgr[0], gain, bgr[0]);
        cv::multiply(bgr[1], gain, bgr[1]);
        cv::multiply(bgr[2], gain, bgr[2]);
    }

    cv::merge(bgr, 3, dst);
    dst.convertTo(dst, CV_8UC3);
}

//导向滤波器
Mat guidedFilter(cv::Mat& I, cv::Mat& p, int r, float eps)
{
    /*
    × GUIDEDFILTER   O(N) time implementation of guided filter.
    ×
    ×   - guidance image: I (should be a gray-scale/single channel image)
    ×   - filtering input image: p (should be a gray-scale/single channel image)
    ×   - local window radius: r
    ×   - regularization parameter: eps
    */

    cv::Mat _I;
    I.convertTo(_I, CV_32FC1);
    I = _I;

    cv::Mat _p;
    p.convertTo(_p, CV_32FC1);
    p = _p;

    //因为opencv自带的boxFilter（）中的Size,比如9x9,我们说半径为4
    r = 2 * r + 1;

    //mean_I = boxfilter(I, r) ./ N;
    cv::Mat mean_I;
    cv::boxFilter(I, mean_I, CV_32FC1, cv::Size(r, r));

    //mean_p = boxfilter(p, r) ./ N;
    cv::Mat mean_p;
    cv::boxFilter(p, mean_p, CV_32FC1, cv::Size(r, r));

    //mean_Ip = boxfilter(I.*p, r) ./ N;
    cv::Mat mean_Ip;
    cv::boxFilter(I.mul(p), mean_Ip, CV_32FC1, cv::Size(r, r));

    //cov_Ip = mean_Ip - mean_I .* mean_p; % this is the covariance of (I, p) in each local patch.
    cv::Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);

    //mean_II = boxfilter(I.*I, r) ./ N;
    cv::Mat mean_II;
    cv::boxFilter(I.mul(I), mean_II, CV_32FC1, cv::Size(r, r));

    //var_I = mean_II - mean_I .* mean_I;
    cv::Mat var_I = mean_II - mean_I.mul(mean_I);

    //a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;
    cv::Mat a = cov_Ip / (var_I + eps);

    //b = mean_p - a .* mean_I; % Eqn. (6) in the paper;
    cv::Mat b = mean_p - a.mul(mean_I);

    //mean_a = boxfilter(a, r) ./ N;
    cv::Mat mean_a;
    cv::boxFilter(a, mean_a, CV_32FC1, cv::Size(r, r));

    //mean_b = boxfilter(b, r) ./ N;
    cv::Mat mean_b;
    cv::boxFilter(b, mean_b, CV_32FC1, cv::Size(r, r));

    //q = mean_a .* I + mean_b; % Eqn. (8) in the paper;
    cv::Mat q = mean_a.mul(I) + mean_b;

    return q;
}

double Transform(double x)
{
    if (x <= 0.05)return x * 2.64;
    return 1.099 * pow(x, 0.9 / 2.2) - 0.099;
}
struct zxy {
    double x, y, z;
}s[2500][2500];

int hdr2(cv::Mat input_img, cv::Mat out_img) {
    int rows = input_img.rows;
    int cols = input_img.cols;
    double r, g, b;
    double lwmax = -1.0, base = 0.75;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            b = (double)input_img.at<Vec3b>(i, j)[0] / 255.0;
            g = (double)input_img.at<Vec3b>(i, j)[1] / 255.0;
            r = (double)input_img.at<Vec3b>(i, j)[2] / 255.0;
            s[i][j].x = (0.4124 * r + 0.3576 * g + 0.1805 * b);
            s[i][j].y = (0.2126 * r + 0.7152 * g + 0.0722 * b);
            s[i][j].z = (0.0193 * r + 0.1192 * g + 0.9505 * b);
            lwmax = max(lwmax, s[i][j].y);
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double xx = s[i][j].x / (s[i][j].x + s[i][j].y + s[i][j].z);
            double yy = s[i][j].y / (s[i][j].x + s[i][j].y + s[i][j].z);
            double tp = s[i][j].y;
            //修改CIE:X,Y,Z
            s[i][j].y = 1.0 * log(s[i][j].y + 1) / log(2 + 8.0 * pow((s[i][j].y / lwmax), log(base) / log(0.5))) / log10(lwmax + 1);
            double x = s[i][j].y / yy * xx;
            double y = s[i][j].y;
            double z = s[i][j].y / yy * (1 - xx - yy);

            //转化为用RGB表示
            r = 3.2410 * x - 1.5374 * y - 0.4986 * z;
            g = -0.9692 * x + 1.8760 * y + 0.0416 * z;
            b = 0.0556 * x - 0.2040 * y + 1.0570 * z;

            if (r < 0)r = 0; if (r > 1)r = 1;
            if (g < 0)g = 0; if (g > 1)g = 1;
            if (b < 0)b = 0; if (b > 1)b = 1;

            //修正补偿
            r = Transform(r), g = Transform(g), b = Transform(b);
            out_img.at<Vec3b>(i, j)[0] = int(b * 255);
            out_img.at<Vec3b>(i, j)[1] = int(g * 255);
            out_img.at<Vec3b>(i, j)[2] = int(r * 255);
        }
    }
    return 0;
}
