#define _CRT_SECURE_NO_WARNINGS
#include "itv.h"
#include "vti.h"
#include <iostream>
#include <algorithm>
#define _hello
#ifdef _hello
using namespace std;
int main(int argv, char** argc) {
	bool imageTV = true; //true��ʱ��ִ�н��������ļ�ת��Ϊ��Ƶ
	if (imageTV) {
		if (argv == 5) { //�����ĸ�������ʱ�����һ������������Ϊ������Ƶ��fps,Ĭ��fps��15
			ImageToVideo::Main(argc[1], std::stoi(argc[2]), (argc[3]), std::stoi(argc[4]));
		}
		else if (argv == 6) {
			ImageToVideo::Main(argc[1], std::stoi(argc[2]), (argc[3]), std::stoi(argc[4]), std::stoi(argc[5]));
		}
		else {
			cout << "������������Ϲ��";
			return 0;
		}
	}
	else {
		//�����������������ʾ���� qaq.mp4 aca.bin ����qaq.mp4��Ҫ�������Ƶ��aca.bin�ǽ�����Ŷ����ƽ�����ļ�
		//�����ĸ����������ʾ��:  qaq.mp4 aca.bin tat.bin tmt.jpg ���е�һ����ͬ�ϣ����������������ָʾ�����ƽ����ÿһλ�Ƿ���Ч���ļ�
		//���ĸ���Դ�ļ��������������Ľ���Աȸ��������tat.bin��
		if (argv != 3 && argv != 5) {
			cout << "������������Ϲ��";
			return 0;
		}
		else {
			auto o = VideoToImg::Main(argc[1], argc[2]);
			if (argv == 5) {
				char filename[256];
				for (int i = 1; ; i++) {
					if (!o.count(i))
						continue;
					sprintf(filename, "%s\\%05d.bin", argc[4], i);
					FILE* tempo = fopen(filename, "rb");
					if (tempo == NULL)
						break;
					cout << filename << endl;
					fseek(tempo, 0, SEEK_END);
					int fplen = ftell(tempo);
					rewind(tempo);
					unsigned char* t = (unsigned char*)malloc(sizeof(unsigned char) * fplen);
					fread(t, 1, fplen, tempo);
					fclose(tempo);
					sprintf(filename, "%s\\%05d.val", argc[3], i);
					FILE* vout = fopen(filename, "w");
					char board[16] = { '0','1','2','3','4','5','6','7','8','9','A', 'B', 'C', 'D', 'E', 'F' };
					fplen = std::min(fplen, (int)o[i].second);
					for (int tt = 0; tt < fplen; tt++) { //����ǰ�����õ��Ķ����ƴ��Ա�
						unsigned char qaqtemp = ~(o[i].first[tt] ^ t[tt]);
						fprintf(vout, "%c%c ", board[(qaqtemp >> 4) & 0xf], board[(qaqtemp) & 0xf]);
					}
					fclose(vout);
					free(t);
				}
			}
			for (auto& i : o) {
				free(i.second.first);
			}
		}
	}
	return 0;
}
#endif