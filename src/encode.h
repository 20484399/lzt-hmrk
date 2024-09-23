#pragma once
#include<opencv2/opencv.hpp>

using namespace cv;
namespace ImageToVideo {
	//程序入口
	int Main(const char* imgsrc, const char* videosrc, int timelimit, int fps = 15);
	//Mat用来存储图像及其他多维数据，此函数用于创建包含二维码的图像帧
	Mat createFrameQrCode(unsigned char** fpstr, int& len, int no);
	//画二维码的点
	void drawQrPoint(Mat& mat);
	//在给定图片上绘制二维码的外边缘
	void drawOutEdge(Mat& mat, int i, int j, int len, int color);
	//在图像上绘制的字节和标志信息
	void drawFrameByteAndFlag(Mat& mat, long long allByte);
	//在图像上绘制帧编号
	void drawFrameNo(Mat& mat, int no);
	//用于放大图像
	void imgBigToSee(Mat& mat);
	//返回放大版的图像
	Mat bigMat(Mat& mat);
}