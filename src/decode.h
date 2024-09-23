#pragma once
#include "rec.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
namespace VideoToImg {
	//����ͼ�����ض����ֽںͱ�־
	int searchFrameByteAndFlag(Mat& mat);
	//����ͼ���е�֡����Ϣ
	int searchFrameNo(Mat& mat);
	//�������ַ���λ˳��ת
	unsigned char rrr(unsigned char qaq);
	//����ά��ͼ��ת�����ַ�����
	void qrCodeToChar(unsigned char* res, int len, Mat& mat, int& L);
	//������ͼ����ִ���������
	void fcout(Mat& mat, int q);
	//����Ƶת����ͼ��
	void viedoToImg(const char* videosrc);
	unsigned char* Main(const char* videosrc, const char* out_);
}