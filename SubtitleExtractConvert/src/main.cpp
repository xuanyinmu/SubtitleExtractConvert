#include "T2SConvert.h"

int main()
{
	std::string rootDir = "C:/Users/Layin/Downloads";
	std::cin >> rootDir;
	T2SConvert convert(rootDir);
	convert.PrintSubtitleFilesPaths();
	convert.SaveDatas();

	return 0;
}

