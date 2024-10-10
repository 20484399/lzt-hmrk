#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;
namespace ImageToVideo {
	int Main(const char* imgsrc, unsigned int size, const char* videosrc, int timelimit, int fps = 10);
	Mat createFrameQrCode(unsigned char** fpstr, int& len, int no);
	void drawQrPoint(Mat& mat);
	void drawOuterEdge(Mat& mat, int i, int j, int len, int color);
	void drawFrameByteAndFlag(Mat& mat, long long allByte);
	void drawFrameNo(Mat& mat, int no);
	void imgBigToSee(Mat& mat);
	Mat bigMat(Mat& mat);
};