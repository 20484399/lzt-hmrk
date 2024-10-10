#define _CRT_SECURE_NO_WARNINGS
#include "frame_format.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
//buffer������Ķ������ַ��������ֽ�Ϊ��С��λ����size�����ֽ����������32λCRC����
unsigned int crc32_check(unsigned char* buffer, unsigned int size) {
	unsigned long long temp = 0;
	for (int i = 0; i < 4; i++) {
		temp <<= 8;
		temp |= buffer[i];
	}
	for (unsigned i = 4; i < size; i++) {
		for (int q = 7; q >= 0; q--) {
			temp <<= 1;
			temp |= buffer[i] & (1u << q) ? 1 : 0;
			if (temp & (0x100000000LL))
				temp ^= 0x104C11DB7LL; //��׼��32���ɶ���ʽ
		}
	}
	return (unsigned)temp;
}
int check_1_umap(unsigned int crc32, unsigned int size) { //���ݱ�׼��������Ĳ�ͬ��������Ӧ���ǵڼ�λ���������ң���1��ʼ�� 
	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 4));
	memset(buffer, 0x00, sizeof(unsigned char) * (size + 4));
	int n = 1;
	for (unsigned i = 0; i < size + 4; i++) {
		for (int t = 7; t >= 0; t--) {
			if ((buffer[i] & (1u << t))) {
				buffer[i] &= ~(buffer[i] & (1u << t));
			}
			else {
				buffer[i] |= (1u << t);
			}
			unsigned int crcc = crc32_check(buffer, size);
			if (crcc == crc32)
				return n;
			++n;
			if ((buffer[i] & (1u << t))) {
				buffer[i] &= ~(buffer[i] & (1u << t));
			}
			else {
				buffer[i] |= (1u << t);
			}
		}
	}
	free(buffer);
	return 0;
}
void check_add(unsigned char* res, unsigned int& len, unsigned char tem) {
	if (tem == 0xCA || tem == 0xCC || tem == 0xDD || tem == 0xFD) { // ת��
		res[len++] = 0xCC;
		res[len++] = tem;
	}
	else {
		res[len++] = tem;
	}
}
unsigned char check_pop(unsigned char* res, int& i, bool& cr) {
	if (res[i] == 0xCC) {
		cr = true;
		++i;
	}
	return res[i++];
}
void format(unsigned char* res, unsigned char*& temp, unsigned int& len, unsigned char dest, unsigned char sour, unsigned int size) {
	res[len++] = 0xCA, res[len++] = 0xCA;
	check_add(res, len, dest);
	check_add(res, len, sour);
	unsigned int crc32;
	unsigned char* qaq = (unsigned char*)malloc(sizeof(unsigned char) * (size + 4));
	for (int i = 0; i < size; i++) qaq[i] = temp[i];
	for (int i = size; i < size + 4; i++) qaq[i] = 0;
	crc32 = crc32_check(qaq, size + 4);
	free(qaq);
	while (size) {
		check_add(res, len, *temp++);
		size--;
	}
	res[len++] = 0xFD;
	for (int i = 0; i < 4; i++) {
		check_add(res, len, (unsigned char)(crc32 & 0xff));
		crc32 >>= 8;
	}
	res[len++] = 0xDD, res[len++] = 0xDD;
}
//����ı�׼֡��ʽ
//֡�׶����*2 Ŀ�ĵ�ַ ԭ��ַ ���� �غ� CRC ֡β�����*2
//֡�׶���� 0xCA ֡β����� 0xDD CRC��ʼ�� 0xFD
//֡������0xCA ��0xCCCA���棬 0xDD��0xCCDD���棬0xFD��0xCCFD���棬 0xCC��0xCCCC����
//size��֡��Ч���ݵ��ֽڳ�
//umap�Ƕ��ձ�������Ϊ0��ʱ�������һλ����
pair<unsigned char*, unsigned int> FrameFormat(vector<unsigned char*> cvec, vector<unsigned int> svec, unsigned int size
	, vector<pair<unsigned char, unsigned char>> dest_sour) {
	unsigned int allsize = 0;
	for (auto i : svec) allsize += i;
	unsigned char* res = (unsigned char*)malloc(sizeof(unsigned char) * 2 * allsize);
	int sz = cvec.size(), i = 0, ret = sz;
	unsigned int len = 0;
	while (ret) { //�����ļ���δ������֡����
		if (svec[i % sz] == 0) { //�Ѿ�������ļ�������ռ��֡����֡
			i++;
			continue;
		}
		int tolen = std::min(size, svec[i % sz]);
		svec[i % sz] -= tolen;
		if (svec[i % sz] == 0) // ����һ���ļ���
			--ret;
		format(res, cvec[i % sz], len, dest_sour[i % sz].first, dest_sour[i % sz].second, tolen);
		++i; //ÿ��δ��ȡ����ļ�����������һ������֡����
	}
	return { res, len };
}
unordered_map<unsigned char, vector<unsigned char>> FormatFrame(unsigned char* info, unsigned int len) {
	unordered_map<unsigned char, vector<unsigned char>> res;
	bool ncheck = false, begin = false, getsour = false;
	unsigned char sour;
	unsigned int lll = 0;
	cout << "�������м������ֻ��������һλ���������ң���1��ʼ����⵽�������Զ�����λȡ��" << endl;
	cout << "������Ҫ���ֵ����ִ�����޷���������֡��һ�����Ч����ȱʧ�����㷴�⣺һ����ɫ�鱻����λ��ɫ�飡(�����ִ���һ����ʾΪ��0λ����)" << endl;
	cout << "����֡�����ŵ���Դ��ַ������ǰ����" << endl;
	for (int i = 0; i < len;) {
		bool cr = false;
		unsigned char cac = check_pop(info, i, cr);
		if (cac == 0xCA && !cr) {
			begin = true;
			getsour = false;
		}
		else if (cac == 0xDD && !cr) {
			begin = false;
		}
		else if (begin && cac == 0xFD && !cr) { //��CRC32����һλ����
			unsigned int q = 0;
			int ll = res[sour].size();
			unsigned char* templll = (unsigned char*)malloc(sizeof(unsigned char) * (ll - lll + 4));
			for (int i = lll; i < ll; i++) templll[i - lll] = res[sour][i];
			unsigned int aaaa = 0;
			for (int cnt = 3; cnt >= 0; cnt--) {
				templll[ll - lll + cnt] = check_pop(info, i, cr);
			}
			q = crc32_check(templll, ll - lll + 4);
			int icrccode = 0;
			if (q)
				icrccode = check_1_umap(q, ll - lll);
			cout << "������֡(" << dec << (int)sour << ")��Ч���ݳ���Ϊ��" << dec << ll - lll << (q ? (",��⵽��" + to_string(icrccode) + "λ����.") : ",δ��⵽����.");
			cout << "��ȡ����CRC32У����λ��";
			for (int i = ll - lll; i < ll - lll + 4; i++)
				cout << hex << (unsigned int)templll[i];
			cout << endl;
			free(templll);
			if (q == 0 || icrccode < 1 || icrccode > ll - lll)
				continue;
			if (res[sour][lll + icrccode / 8] & (1u << ((icrccode - 1) % 8))) {
				res[sour][lll + icrccode / 8] &= ~(res[sour][lll + icrccode / 8] & (1u << ((icrccode - 1) % 8)));
			}
			else {
				res[sour][lll + icrccode / 8] |= (1u << ((icrccode - 1) % 8));
			}
		}
		else {
			if (begin && !getsour) {
				getsour = true;
				ncheck = true;
				sour = check_pop(info, i, cr); //���Դ��ַ
			}
			else if (begin) {
				if (!res.count(sour))
					res[sour] = vector<unsigned char>();
				if (ncheck) {
					ncheck = false;
					lll = res[sour].size();
				}
				res[sour].push_back(cac);
			}
		}
	}
	return res;
}