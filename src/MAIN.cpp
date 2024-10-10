#define _CRT_SECURE_NO_WARNINGS
#include "itv.h"
#include "vti.h"
#include <iostream>
#include <algorithm>
#define _hello
#ifdef _hello
using namespace std;
int main(int argv, char** argc) {
	bool imageTV = true; //true的时候执行将二进制文件转存为视频
	if (imageTV) {
		if (argv == 5) { //输入四个参数的时候最后一个参数可以作为编码视频的fps,默认fps是15
			ImageToVideo::Main(argc[1], std::stoi(argc[2]), (argc[3]), std::stoi(argc[4]));
		}
		else if (argv == 6) {
			ImageToVideo::Main(argc[1], std::stoi(argc[2]), (argc[3]), std::stoi(argc[4]), std::stoi(argc[5]));
		}
		else {
			cout << "输入参数不符合规格！";
			return 0;
		}
	}
	else {
		//输入两个额外参数的示例： qaq.mp4 aca.bin 其中qaq.mp4是要解码的视频，aca.bin是解码后存放二进制结果的文件
		//输入四个额外参数的示例:  qaq.mp4 aca.bin tat.bin tmt.jpg 其中第一二个同上，第三个是用来输出指示二进制结果的每一位是否有效的文件
		//第四个是源文件，用来跟解码后的结果对比辅助输出到tat.bin中
		if (argv != 3 && argv != 5) {
			cout << "输入参数不符合规格！";
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
					for (int tt = 0; tt < fplen; tt++) { //编码前解码后得到的二进制串对比
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