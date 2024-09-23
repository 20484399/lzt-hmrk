#define _CRT_SECURE_NO_WARNINGS
#include "encode.h"
#include "decode.h"
#include <iostream>

using namespace std;
int main(int argv, char** argc) {
	bool imageTV = false; //true��ʱ��ִ�н��������ļ�ת��Ϊ��Ƶ
	if (imageTV) {
		//��������������ʱ���һ��������Դ�ļ����ڶ����Ǳ������Ƶ��ŵĵ�ַ������������Ƶ�����ʱ��
		if (argv == 4) {
			ImageToVideo::Main(argc[1], argc[2], std::stoi(argc[3]));
		}
		else if (argv == 5) { //�����ĸ�������ʱ�����һ������������Ϊ������Ƶ��fps,Ĭ��fps��15
			ImageToVideo::Main(argc[1], argc[2], std::stoi(argc[3]), std::stoi(argc[4]));
		}
		else {
			cout << "������������Ϲ��";
			return 0;
		}
	}
	else {
		if (argv != 3 && argv != 5) {
			cout << "������������Ϲ��";
			return 0;
		}
		else {
			unsigned char* o = VideoToImg::Main(argc[1], argc[2]);
			if (argv == 5) {
				FILE* tempo = fopen(argc[4], "rb");
				FILE* vout = fopen(argc[3], "wb");
				fseek(tempo, 0, SEEK_END);
				int fplen = ftell(tempo);
				rewind(tempo);
				unsigned char* t = (unsigned char*)malloc(sizeof(unsigned char) * fplen);
				fread(t, 1, fplen, tempo);
				fclose(tempo);
				char board[16] = { '0','1','2','3','4','5','6','7','8','9','A', 'B', 'C', 'D', 'E', 'F' };
				for (int i = 0; i < fplen; i++) { //����ǰ�����õ��Ķ����ƴ��Ա�
					unsigned char qaqtemp = ~(o[i] ^ t[i]);
					fprintf(vout, "%c%c ", board[(qaqtemp >> 4) & 0xf], board[(qaqtemp) & 0xf]);
				}
				fclose(vout);
				free(t);
			}
			free(o);
		}
	}
	return 0;
}