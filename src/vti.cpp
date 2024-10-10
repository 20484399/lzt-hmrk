#define _CRT_SECURE_NO_WARNINGS
#include "frame_format.h"
#include "vti.h"
#include "pic.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
using namespace cv;
namespace VideoToImg {
	const int BLACK = 0;
	const int WHITE = 1;
	const Vec3b pixel[2] = { Vec3b(0,0,0), Vec3b(255,255,255) };
	// ����
	const int BigRate = 5;
	// size
	const int OuterSize = 2;
	const int FrameSize = 136;
	const int QrPointSize = 18;
	const int SmallPointSize = 9;
	//
	const int BytePerFrame = 2070;
	const int RectCnt = 6;
	const int FrameRectArea[RectCnt][2][2] = { // allarea = 16560, ����2070�ֽ���Ч��Ϣ
		{{FrameSize - 2 - 2 * QrPointSize, QrPointSize - OuterSize}, {QrPointSize + 2, OuterSize}}, // 
		{{QrPointSize - OuterSize, FrameSize - 2 * QrPointSize}, {OuterSize, QrPointSize}}, // �� a
		{{FrameSize - QrPointSize - SmallPointSize - 1, QrPointSize - OuterSize}, {QrPointSize, FrameSize - QrPointSize}}, // �� 
		{{SmallPointSize - 1, QrPointSize - SmallPointSize - 1}, {FrameSize - 1 - SmallPointSize, FrameSize - QrPointSize}}, // ���� 
		{{QrPointSize - OuterSize, FrameSize - 2 * QrPointSize}, {FrameSize - QrPointSize, QrPointSize}}, // �� 
		{{FrameSize - QrPointSize * 2, FrameSize - QrPointSize * 2}, {QrPointSize, QrPointSize}}, // �� 
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
			if (mat.at<Vec3b>(QrPointSize + 1, OuterSize + i) == pixel[WHITE])
				allbyte |= 1;
		}
		return allbyte;
	}
	int searchFrameNo(Mat& mat) {
		int res = 0;
		for (int i = 15; i >= 0; i--) {
			res <<= 1;
			if (mat.at<Vec3b>(QrPointSize, OuterSize + i) == pixel[WHITE])
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
		for (int i = 0; i < RectCnt; i++) {
			for (int q = 0; q < FrameRectArea[i][0][0]; q++) {
				for (int t = 0; t < FrameRectArea[i][0][1]; t++) {
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
					if (mat.at<Vec3b>(FrameRectArea[i][1][0] + q, FrameRectArea[i][1][1] + t) == pixel[WHITE]) {
						tempcac |= 1;
					}
				}
			}
		}
		res[L++] = rrr(tempcac);
	}
	void fcout(Mat& mat, int q) {
		for (int i = 0; i < FrameSize; i++)
			if (mat.at<Vec3b>(q, i) == pixel[WHITE])
				cout << 1;
			else
				cout << 0;
	}
	unordered_map<unsigned char, pair<unsigned char*, unsigned int>> Main(const char* videosrc, const char* out_folder) {
		viedoToImg(videosrc);
		unsigned char* temp = (unsigned char*)malloc(sizeof(unsigned char) * 5000000);
		unordered_map<unsigned char, pair<unsigned char*, unsigned int>> res;
		int len = 0, mmax = 0, ttt = 0;
		unordered_set<int> vis; // ȷ�ϵ�ǰÿһ���֡Ψһ
		vector<Mat> qrCodev; // ��ʱ��ʶ���֡�����˳����ң�������Ԥ����vector���������ǵı�ŵ���˳���ٶ����ļ���
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
		// ������õĶ�ά�����˳���ʼ¼��������ַ���temp��
		for (int i = 0; i <= mmax; i++) {
			if (acac.count(i) == 0) {
				cout << "��������֡" << endl;
				continue; // ���Ը�Ϊreturn ��ǰ��ֹ������Ϊ��ʱ��ȡ����Ϣ�Ѿ�������
			}
			cout << "�� " << (i + 1) << " ��ͼƬ�ѽ������" << endl;
			/*		imshow("A", qrCodev[acac[i]]);
					waitKey();*/
			int qlenl = searchFrameByteAndFlag(qrCodev[acac[i]]);
			qlenl = min(qlenl, BytePerFrame);
			qrCodeToChar(temp, qlenl, qrCodev[acac[i]], len);
		}
		// ������õ��Ķ������ַ���temp������res��
		cout << "���ڿ�ʼ�Խ��������������ݸ�����Դ��ַ���ಢʹ�ø��ӵ�CRC32У����У��" << endl;
		auto temppp = FormatFrame(temp, len);
		for (auto& i : temppp) {
			unsigned char* ttt = (unsigned char*)malloc(sizeof(unsigned char) * (i.second.size()));
			for (int t = 0; t < i.second.size(); t++) ttt[t] = i.second[t];
			res[i.first] = { ttt, i.second.size() };
		}
		// �ֿ�д��
		for (auto& i : res) {
			char filename[256];
			sprintf(filename, "%s\\%05d.bin", out_folder, i.first);
			FILE* imgp = fopen(filename, "wb");
			if (imgp == nullptr) {
				cout << "���ļ�ʧ�ܣ�" << endl;
			}
			fwrite(i.second.first, 1, i.second.second, imgp);
			fclose(imgp);
		}
		destroyAllWindows();
		/*char board[16] = { '0','1','2','3','4','5','6','7','8','9','A', 'B', 'C', 'D', 'E', 'F' };
		for (int i = 0; i < len; i++) {
			fprintf(imgp, "%c%c ", board[(temp[i] >> 4) & 0xf], board[(temp[i]) & 0xf]);
		}
		fclose(imgp);*/
		cout << "�������";
		// ���ؽ���Ķ������ַ���temp��Դ�ַ������бȽ�
		return res;
	}
};