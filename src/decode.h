#pragma once
#include "rec.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
namespace VideoToImg {
	//搜索图像中特定的字节和标志
	int searchFrameByteAndFlag(Mat& mat);
	//搜索图像中的帧号信息
	int searchFrameNo(Mat& mat);
	//将输入字符的位顺序反转
	unsigned char rrr(unsigned char qaq);
	//将二维码图像转换成字符数组
	void qrCodeToChar(unsigned char* res, int len, Mat& mat, int& L);
	//用于在图像上执行输出操作
	void fcout(Mat& mat, int q);
	//将视频转换成图像
	void viedoToImg(const char* videosrc);
	unsigned char* Main(const char* videosrc, const char* out_);
}