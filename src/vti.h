#pragma once
#include "pic.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
namespace VideoToImg {
	int searchFrameByteAndFlag(Mat& mat);
	int searchFrameNo(Mat& mat);
	unsigned char rrr(unsigned char qaq);
	void qrCodeToChar(unsigned char* res, int len, Mat& mat, int& L);
	void fcout(Mat& mat, int q);
	void viedoToImg(const char* videosrc);
	unordered_map<unsigned char, pair<unsigned char*, unsigned int>> Main(const char* videosrc, const char* out_folder);
}
