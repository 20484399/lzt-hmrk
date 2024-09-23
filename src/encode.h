#pragma once
#include<opencv2/opencv.hpp>

using namespace cv;
namespace ImageToVideo {
	//�������
	int Main(const char* imgsrc, const char* videosrc, int timelimit, int fps = 15);
	//Mat�����洢ͼ��������ά���ݣ��˺������ڴ���������ά���ͼ��֡
	Mat createFrameQrCode(unsigned char** fpstr, int& len, int no);
	//����ά��ĵ�
	void drawQrPoint(Mat& mat);
	//�ڸ���ͼƬ�ϻ��ƶ�ά������Ե
	void drawOutEdge(Mat& mat, int i, int j, int len, int color);
	//��ͼ���ϻ��Ƶ��ֽںͱ�־��Ϣ
	void drawFrameByteAndFlag(Mat& mat, long long allByte);
	//��ͼ���ϻ���֡���
	void drawFrameNo(Mat& mat, int no);
	//���ڷŴ�ͼ��
	void imgBigToSee(Mat& mat);
	//���طŴ���ͼ��
	Mat bigMat(Mat& mat);
}