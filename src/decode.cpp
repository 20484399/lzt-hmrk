#define _CRT_SECURE_NO_WARNINGS
#include "decode.h"
#include "rec.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
using namespace cv;
namespace VideoToImg {
	//�Զ�ά��ͼƬ������ز����Ķ���
	//�ڰ�
	const int black = 0;
	const int white = 1;
	const Vec3b pixel[2] = { Vec3b(0,0,0),Vec3b(255,255,255) };
	//����
	const int bigRate = 5;
	//�Զ�ά�벻ͬλ�õĳߴ���ж���
	const int outSize = 2;
	const int frameSize = 136;
	const int QrPointSize = 18;
	const int smallPointSize = 9;
	//��������ά������������ж���
	const int bytePerFrame = 2070;
	const int rectCnt = 6;
	//ָ����ά��ÿ�����������
	const int frameRectArea[rectCnt][2][2] = {
		{{frameSize - 2 - 2 * QrPointSize, QrPointSize - outSize}, {QrPointSize + 2, outSize}}, // 
		{{QrPointSize - outSize, frameSize - 2 * QrPointSize}, {outSize, QrPointSize}}, //��
		{{frameSize - QrPointSize - smallPointSize - 1, QrPointSize - outSize}, {QrPointSize, frameSize - QrPointSize}}, //�� 
		{{smallPointSize - 1, QrPointSize - smallPointSize - 1}, {frameSize - 1 - smallPointSize, frameSize - QrPointSize}}, //���� 
		{{QrPointSize - outSize, frameSize - 2 * QrPointSize}, {frameSize - QrPointSize, QrPointSize}}, //�� 
		{{frameSize - QrPointSize * 2, frameSize - QrPointSize * 2}, {QrPointSize, QrPointSize}}, //��
	};
	enum class FLAG {
		Begin,
		End,
		BeginToEnd,
		Normal
	};
	void viedoToImg(const char* videosrc) {
		char BUF[256];
		int fps = 60;
		sprintf(BUF, "\"ffmpeg\\bin\\\"ffmpeg.exe -i %s -q:v 2 -f image2 %s", videosrc, "%05d.jpg");
		system(BUF);
	}
	int searchFrameByteAndFlag(Mat& mat) {
		int allbyte = 0;
		for (int i = 15; i >= 2; i--) {
			allbyte <<= 1;
			if (mat.at<Vec3b>(QrPointSize + 1, outSize + i) == pixel[white])
				allbyte |= 1;
		}
		return allbyte;
	}
	int searchFrameNo(Mat& mat) {
		int res = 0;
		for (int i = 15; i >= 0; i--) {
			res <<= 1;
			if (mat.at<Vec3b>(QrPointSize, outSize + i) == pixel[white])
				res |= 1;
		}
		return res;
	}
	unsigned char rrr(unsigned char qaq) {
		unsigned char res = 0;
		for (int i = 7; i >= 0; i--) {
			res <<= 1;
			res |= (qaq & 1);
			qaq >>= 1;
		}
		return res;
	}
	void qrCodeToChar(unsigned char* res, int len, Mat& mat, int& L) {
		unsigned char tempcac = 0;
		int tempcnt = 0, all = -1;
		for (int i = 0; i < rectCnt; i++) {
			for (int q = 0; q < frameRectArea[i][0][0]; q++) {
				for (int t = 0; t < frameRectArea[i][0][1]; t++) {
					if (tempcnt == 8) {
						tempcnt = 0;
						++all;
						res[L++] = rrr(tempcac);
						if (all == len) {
							return;
						}
						tempcac = 0;
					}
					tempcnt++;
					tempcac <<= 1;
					if (mat.at<Vec3b>(frameRectArea[i][1][0] + q, frameRectArea[i][1][1] + t) == pixel[white]) {
						tempcac |= 1;
					}
				}
			}
		}
		res[L++] = rrr(tempcac);
	}
	void fcout(Mat& mat, int q) {
		for (int i = 0; i < frameSize; i++)
			if (mat.at<Vec3b>(q, i) == pixel[white])
				cout << 1;
			else
				cout << 0;
	}
	unsigned char* Main(const char* videosrc, const char* out_) {
		viedoToImg(videosrc);
		unsigned char* temp = (unsigned char*)malloc(sizeof(unsigned char) * 500000);
		int len = 0, mmax = 0, ttt = 0;
		unordered_set<int> vis; //ȷ�ϵ�ǰÿһ���֡Ψһ
		vector<Mat> qrCodev; //��ʱ��ʶ���֡�����˳����ң�������Ԥ����vector���������ǵı�ŵ���˳���ٶ����ļ���
		unordered_map<int, int> acac;
		cout << "���ڽ�����...." << endl;
		for (int i = 1;; i++) {
			char imgsrc[256];
			sprintf(imgsrc, "%05d.jpg", i);
			FILE* test = fopen(imgsrc, "rb");
			if (test == nullptr) {
				break;
			}
			fclose(test);
			Mat imgmat = imread(imgsrc), qrCode;
			if (ImgParse::Main(imgmat, qrCode)) {
				system((string("del ") + imgsrc).c_str());
				continue;
			}
			system((string("del ") + imgsrc).c_str());
			int no = searchFrameNo(qrCode);
			if (no > mmax + 5) {
				continue;
			}
			mmax = max(no, mmax);
			if (vis.count(no)) {
				qrCodev[acac[no]] = qrCode;
				continue;
			}
			vis.insert(no);
			qrCodev.emplace_back(qrCode);
			acac[no] = ttt++;
		}
		//������õĶ�ά�����˳���ʼ¼��������ַ���temp��
		for (int i = 0; i <= mmax; i++) {
			if (acac.count(i) == 0) {
				cout << "��������֡��������Ƶ�������Ի����fps����¼����Ƶ" << endl;
				continue; //���Ը�Ϊreturn ��ǰ��ֹ������Ϊ��ʱ��ȡ����Ϣ�Ѿ�������
			}
			cout << "�� " << (i + 1) << " ��ͼƬ�ѽ������" << endl;

			int qlenl = searchFrameByteAndFlag(qrCodev[acac[i]]);
			qlenl = min(qlenl, bytePerFrame);
			qrCodeToChar(temp, qlenl, qrCodev[acac[i]], len);
		}
		//������õ��Ķ������ַ���temp����out_�ļ�֮��
		FILE* imgp = fopen(out_, "w");
		if (imgp == nullptr) {
			cout << "���ļ�ʧ�ܣ�" << endl;
		}
		destroyAllWindows();
		char board[16] = { '0','1','2','3','4','5','6','7','8','9','A', 'B', 'C', 'D', 'E', 'F' };
		for (int i = 0; i < len; i++) {
			fprintf(imgp, "%c%c ", board[(temp[i] >> 4) & 0xf], board[(temp[i]) & 0xf]);
		}
		fclose(imgp);
		cout << "�������";
		//���ؽ���Ķ������ַ���temp��Դ�ַ������бȽ�
		return temp;
	}
};