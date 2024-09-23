#define _CRT_SECURE_NO_WARNINGS
#include "encode.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <string>

using namespace cv;
using namespace std;
namespace ImageToVideo {
	//对二维码图片进行相关参数的定义
	//黑白
	const int black = 0;
	const int white = 1;
	const Vec3b pixel[2] = { Vec3b(0,0,0),Vec3b(255,255,255) };
	//倍率
	const int bigRate = 5;
	//对二维码不同位置的尺寸进行定义
	const int outSize = 2;
	const int frameSize = 136;
	const int QrPointSize = 18;
	const int smallPointSize = 9;
	//对整个二维码区域参数进行定义
	const int bytePerFrame = 2070;
	const int rectCnt = 6;
	//指定二维码每个区域的坐标
	const int frameRectArea[rectCnt][2][2] = {
		{{frameSize - 2 - 2 * QrPointSize, QrPointSize - outSize}, {QrPointSize + 2, outSize}}, // 
		{{QrPointSize - outSize, frameSize - 2 * QrPointSize}, {outSize, QrPointSize}}, //上
		{{frameSize - QrPointSize - smallPointSize - 1, QrPointSize - outSize}, {QrPointSize, frameSize - QrPointSize}}, //右 
		{{smallPointSize - 1, QrPointSize - smallPointSize - 1}, {frameSize - 1 - smallPointSize, frameSize - QrPointSize}}, //右下 
		{{QrPointSize - outSize, frameSize - 2 * QrPointSize}, {frameSize - QrPointSize, QrPointSize}}, //下 
		{{frameSize - QrPointSize * 2, frameSize - QrPointSize * 2}, {QrPointSize, QrPointSize}}, //中
	};
	//返回放大版的图像
	Mat bigMat(Mat& mat) {
		const int bigLen = bigRate * frameSize;
		Mat tempmat(bigLen, bigLen, CV_8UC3);
		for (int i = 0; i < bigLen; i++) {
			for (int j = 0; j < bigLen; j++)
				tempmat.at<Vec3b>(i, j) = mat.at<Vec3b>(i / bigRate, j / bigRate);
		}
		return tempmat;
	}
	//用于显示放大的图像
	void imgBigToSee(Mat& mat) {
		Mat tempmat = bigMat(mat);
		imshow("image", tempmat);
		waitKey(0);
	}
	//在图像上绘制帧编号
	void drawFrameNo(Mat& mat, int no) {
		for (int i = 0; i < 16; i++) {
			mat.at<Vec3b>(QrPointSize, outSize + i) = pixel[no & 1];
			no >>= 1;
		}
	}
	//在图像上绘制的字节和标志信息
	void drawFrameByteAndFlag(Mat& mat, long long allByte) {
		for (int i = 2; i < 16; i++) {
			mat.at<Vec3b>(QrPointSize + i, outSize + i) = pixel[allByte & 1];
			allByte >>= 1;
		}
	}
	//在给定图片上绘制二维码的外边缘
	void drawOutEdge(Mat& mat, int i, int j, int len, int color) {
		for (int t = 0; t < len; t++) {
			mat.at<Vec3b>(i + t, j) = pixel[color];
			mat.at<Vec3b>(i, j + t) = pixel[color];
			mat.at<Vec3b>(i + len - 1, j + t) = pixel[color];
			mat.at<Vec3b>(i + t, j + len - 1) = pixel[color];
		}
	}
	//画二维码的点
	void drawQrPoint(Mat& mat) {
		const int color[9] = { white,white,black,black,white,white,black,black,black };
		const int pos[3][2] = { {0,0},{0,frameSize - QrPointSize},{frameSize - QrPointSize,0} };
		const int mincolor[5] = { white,black,white,black,black };
		const int minpos[2] = { frameSize - smallPointSize - 1,frameSize - smallPointSize - 1 };
		for (int t = 0; t < 3; t++) {
			for (int k = 0; k < 9; k++) {
				drawOutEdge(mat, pos[t][0] + k, pos[t][1] + k, QrPointSize - 2 * k, color[k]);
			}
		}
		for (int k = 0; k < 5; k++)
			drawOutEdge(mat, minpos[0] + k, minpos[1] + k, smallPointSize - 2 * k, mincolor[k]);
	}
	//Mat用来存储图像及其他多维数据，此函数用于创建包含二维码的图像帧
	Mat createFrameQrCode(unsigned char** fpstr, int& len, int no) {
		Mat res(frameSize, frameSize, CV_8UC3, Scalar::all(255));  // 全白图像
		drawQrPoint(res);
		drawFrameNo(res, no);
		drawFrameByteAndFlag(res, (len >= bytePerFrame ? bytePerFrame : len));
		srand((unsigned)time(nullptr));
		unsigned char tempcac = 0;
		int tempcnt = 8, all = -1;
		for (int i = 0; i < rectCnt; i++) {
			for (int q = 0; q < frameRectArea[i][0][0]; q++) {
				for (int t = 0; t < frameRectArea[i][0][1]; t++) {
					if (tempcnt == 8) {
						tempcnt = 0;
						++all;
						if (all == bytePerFrame)
							break;
						if (len == 0) {
							tempcac = rand() % 255;
						}
						else {
							tempcac = **fpstr;
							++*fpstr;
							--len;
						}
					}
					res.at<Vec3b>(frameRectArea[i][1][0] + q, frameRectArea[i][1][1] + t) = pixel[tempcac & 1];
					tempcac >>= 1;
					++tempcnt;
				}
			}
		}
		return res;
	}
	//程序入口
	int Main(const char* imgsrc, const char* videosrc, int timelimit, int fps) {
		FILE* fp = fopen(imgsrc, "rb");
		char fileName[128];
		fseek(fp, 0, SEEK_END);
		int fplen = ftell(fp);
		rewind(fp);
		unsigned char* fpstr = (unsigned char*)malloc(sizeof(char) * (fplen + 10));
		unsigned char* qaqfpstr = fpstr;
		fread(fpstr, 1, fplen, fp);
		fclose(fp);
		Mat res;
		int no = 0;
		//根据视屏限制时长设置最大帧数，向上取整
		int frameMaxCnt = ceil(timelimit * 1.0 / 1000 * fps);
		//编码成多张二维码，存入当前文件夹下
		res = createFrameQrCode(&fpstr, fplen, no++);
		sprintf(fileName, "%05d.%s", no, "jpg");
		imwrite(fileName, bigMat(res));
		while (fplen >= bytePerFrame && frameMaxCnt > no) {
			res = createFrameQrCode(&fpstr, fplen, no++);
			sprintf(fileName, "%05d.%s", no, "jpg");
			imwrite(fileName, bigMat(res));
		}
		//最后一帧经常丢失，加强最后一帧的识别
		if (fplen >= 0 && frameMaxCnt > no) {
			for (int cnt = 1; cnt <= 5 && frameMaxCnt > no + cnt; cnt++) {
				unsigned char* tempp = (unsigned char*)malloc(sizeof(unsigned char) * (3000));
				unsigned char* temmpp = tempp;
				memcpy(tempp, fpstr, fplen);
				int templen = fplen;
				res = createFrameQrCode(&tempp, templen, no);
				sprintf(fileName, "%05d.%s", no + cnt, "jpg");
				imwrite(fileName, bigMat(res));
				free(temmpp);
			}
		}
		//将多张二维码合成视频，存入videosrc中
		char BUF[256];
		sprintf(BUF, "\"ffmpeg\\bin\\\"ffmpeg.exe  -r %d -i %s %s", fps, "%05d.jpg", videosrc);
		system(BUF);
		//删除当前文件夹下缓存的二维码图像
		for (int i = 1;; i++) {
			char imgsrc[256];
			sprintf(imgsrc, "%05d.jpg", i);
			FILE* aaa = fopen(imgsrc, "r");
			if (aaa == nullptr)
				break;
			fclose(aaa);
			system((string("del ") + imgsrc).c_str());
		}
		free(qaqfpstr);
		return 0;
	}
};