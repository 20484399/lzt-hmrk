#define _CRT_SECURE_NO_WARNINGS
#include <unordered_map>
#include <vector>
using namespace std;

unsigned int crc32_check(unsigned char* buffer, unsigned int size);
int check_1_umap(unsigned int crc32, unsigned int size);
void check_add(unsigned char* res, unsigned int& len, unsigned char tem);
void format(unsigned char* res, unsigned char*& temp, unsigned int& len, unsigned char dest, unsigned char sour, unsigned int size);
pair<unsigned char*, unsigned int> FrameFormat(vector<unsigned char*> cvec, vector<unsigned int> svec, unsigned int size
	, vector<pair<unsigned char, unsigned char>> dest_sour);
unordered_map<unsigned char, vector<unsigned char>> FormatFrame(unsigned char* info, unsigned int len); 
#pragma once