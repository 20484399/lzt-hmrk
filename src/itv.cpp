#define _CRT_SECURE_NO_WARNINGS
#include "itv.h"
#include "frame_format.h"
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
	//黑白色
	const int BLACK = 0;
	const int WHITE = 1;
	const Vec3b pixel[2] = { Vec3b(0,0,0), Vec3b(255,255,255) };
	//倍率
	const int BigRate = 5;
	//size
	const int OuterSize = 2;
	const int FrameSize = 136;
	const int QrPointSize = 18;
	const int SmallPointSize = 9;

	const int BytePerFrame = 2070;
	const int RectCnt = 6;
	const int FrameRectArea[RectCnt][2][2] = { // allarea = 16560, 共计2070字节有效信息
		{{FrameSize - 2 - 2 * QrPointSize, QrPointSize - OuterSize}, {QrPointSize + 2, OuterSize}}, // 
		{{QrPointSize - OuterSize, FrameSize - 2 * QrPointSize}, {OuterSize, QrPointSize}}, // 上 a
		{{FrameSize - QrPointSize - SmallPointSize - 1, QrPointSize - OuterSize}, {QrPointSize, FrameSize - QrPointSize}}, // 右 
		{{SmallPointSize - 1, QrPointSize - SmallPointSize - 1}, {FrameSize - 1 - SmallPointSize, FrameSize - QrPointSize}}, // 右下 
		{{QrPointSize - OuterSize, FrameSize - 2 * QrPointSize}, {FrameSize - QrPointSize, QrPointSize}}, // 下 
		{{FrameSize - QrPointSize * 2, FrameSize - QrPointSize * 2}, {QrPointSize, QrPointSize}}, // 中 
	};
	Mat bigMat(Mat& mat) { //放大
		const int BigLen = BigRate * FrameSize;
		Mat tempmat(BigLen, BigLen, CV_8UC3);
		for (int i = 0; i < BigLen; i++) {
			for (int j = 0; j < BigLen; j++)
				tempmat.at<Vec3b>(i, j) = mat.at<Vec3b>(i / BigRate, j / BigRate);
		}
		return tempmat;
	}
	void imgBigToSee(Mat& mat) { //debug图片
		Mat tempmat = bigMat(mat);
		imshow("image", tempmat);
		waitKey(0);
	}
	void drawFrameNo(Mat& mat, int no) { //表明第几帧
		for (int i = 0; i < 16; i++) {
			mat.at<Vec3b>(QrPointSize, OuterSize + i) = pixel[no & 1];
			no >>= 1;
		}
	}
	void drawFrameByteAndFlag(Mat& mat, long long allByte) {
		for (int i = 2; i < 16; i++) {
			mat.at<Vec3b>(QrPointSize + 1, OuterSize + i) = pixel[allByte & 1];
			allByte >>= 1;
		}
	}
	void drawOuterEdge(Mat& mat, int i, int j, int len, int color) {   //用颜色color给一个正方形外沿上色
		for (int t = 0; t < len; t++) {
			mat.at<Vec3b>(i + t, j) = pixel[color];
			mat.at<Vec3b>(i, j + t) = pixel[color];
			mat.at<Vec3b>(i + len - 1, j + t) = pixel[color];
			mat.at<Vec3b>(i + t, j + len - 1) = pixel[color];
		}
	}
	void drawQrPoint(Mat& mat) { //绘制二维码定位点
		const int color[9] = { WHITE, WHITE, BLACK, BLACK, WHITE, WHITE, BLACK, BLACK, BLACK }; // 1:1:1:3:1:1:1
		const int pos[3][2] = { {0,0},{0, FrameSize - QrPointSize},{FrameSize - QrPointSize, 0} };
		const int mincolor[5] = { WHITE, BLACK, WHITE, BLACK, BLACK };
		const int minpos[2] = { FrameSize - SmallPointSize - 1, FrameSize - SmallPointSize - 1 };
		for (int t = 0; t < 3; t++) {
			for (int k = 0; k < 9; k++) {
				drawOuterEdge(mat, pos[t][0] + k, pos[t][1] + k, QrPointSize - 2 * k, color[k]);
			}
		}
		for (int k = 0; k < 5; k++) drawOuterEdge(mat, minpos[0] + k, minpos[1] + k, SmallPointSize - 2 * k, mincolor[k]);
	}
	Mat createFrameQrCode(unsigned char** fpstr, int& len, int no) {
		Mat res(FrameSize, FrameSize, CV_8UC3, Scalar::all(255));  //全白图像
		drawQrPoint(res);
		drawFrameNo(res, no);
		drawFrameByteAndFlag(res, (len >= BytePerFrame ? BytePerFrame : len));
		srand((unsigned)time(nullptr));
		unsigned char tempcac = 0;
		int tempcnt = 8, all = -1;
		for (int i = 0; i < RectCnt; i++) {
			for (int q = 0; q < FrameRectArea[i][0][0]; q++) {
				for (int t = 0; t < FrameRectArea[i][0][1]; t++) {
					if (tempcnt == 8) {
						tempcnt = 0;
						++all;
						if (all == BytePerFrame)
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
					res.at<Vec3b>(FrameRectArea[i][1][0] + q, FrameRectArea[i][1][1] + t) = pixel[tempcac & 1];
					tempcac >>= 1;
					++tempcnt;
				}
			}
		}
		return res;
	}
	int Main(const char* srcfolder, unsigned int size, const char* videosrc, int timelimit, int fps)
	{
		//读取源文件信息
		vector<unsigned char*> cvec;
		vector<unsigned int> svec;
		vector<pair<unsigned char, unsigned char>> dest_sour;
		for (int i = 1; ; i++) {
			char srcname[256];
			sprintf(srcname, "%s\\%05d.bin", srcfolder, i);
			FILE* fp = fopen(srcname, "rb");
			if (fp == NULL)
				break;
			fseek(fp, 0, SEEK_END);
			int fplen = ftell(fp);
			rewind(fp);
			unsigned char* fpstr = (unsigned char*)malloc(sizeof(char) * (fplen + 10));
			fread(fpstr, 1, fplen, fp);
			cvec.push_back(fpstr);
			svec.push_back(fplen);
			dest_sour.push_back({ 1, (unsigned char)i });
			fclose(fp);
		}
		auto TEM = FrameFormat(cvec, svec, size, dest_sour);
		unsigned char* a = TEM.first;
		unsigned char* b = a;
		int fplen = TEM.second;
		cout << endl << endl << fplen << endl << endl;
		Mat res;
		int no = 0;
		//根据视屏限制时长设置最大帧数，向上取整
		int frameMaxCnt = ceil(timelimit * 1.0 / 1000 * fps);
		//编码成多张二维码，存入文件夹
		char fileName[256];
		res = createFrameQrCode(&a, fplen, no++);
		sprintf(fileName, "%05d.%s", no, "jpg");
		imwrite(fileName, bigMat(res));
		while (fplen >= BytePerFrame && frameMaxCnt > no) {
			res = createFrameQrCode(&a, fplen, no++);
			sprintf(fileName, "%05d.%s", no, "jpg");
			imwrite(fileName, bigMat(res));
		}
		//最后一帧的时候额外重复多补几帧，加强最后一帧的识别
		//因为有时候录视频解码后最后一帧经常丢失
		if (fplen >= 0 && frameMaxCnt > no) {
			for (int cnt = 1; cnt <= 5 && frameMaxCnt > no + cnt; cnt++) {
				unsigned char* tempp = (unsigned char*)malloc(sizeof(unsigned char) * (3000));
				unsigned char* temmpp = tempp;
				memcpy(tempp, a, fplen);
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
		//删除文件夹下缓存的二维码图像
		for (int i = 1;; i++) {
			char imgsrc[256];
			sprintf(imgsrc, "%05d.jpg", i);
			FILE* aaa = fopen(imgsrc, "r");
			if (aaa == nullptr)
				break;
			fclose(aaa);
			system((string("del ") + imgsrc).c_str());
		}
		for (auto i : cvec)
			free(i);
		free(b);
		return 0;
	}
};