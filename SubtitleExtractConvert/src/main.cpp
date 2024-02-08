#include "T2SConvert.h"
#include "SubtitleDecoder.h"
//#include <Windows.h>

int main()
{
    // 设置控制台编码格式为 utf-8
    system("chcp 65001");
	std::string rootDir = "C:/Users/Layin/Downloads";
	std::string videoPath = rootDir + "/[LoliHouse] Ore dake Level Up na Ken - 04 [WebRip 1080p HEVC-10bit AAC ASSx2].mkv";
	std::string outPath = rootDir + "/Subtitle";
	SubtitleDecoder decoder(videoPath,outPath);
	decoder.Decode();

	//std::cin >> rootDir;
	T2SConvert convert(rootDir);
	convert.PrintSubtitleFilesPaths();
	convert.SaveDatas();

	return 0;
}

