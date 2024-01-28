#pragma once
#ifdef _WIN64
#include <io.h>
#endif // _WIN64
//#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
//#include <locale>
#include <codecvt>


#define REVERSE_SIZE 32
#define CPP_ENDWITH_SRT(path) (path[path.size()-4] == '.' && path[path.size() - 3] == 's' \
	&& path[path.size()-2] == 'r' && path[path.size() - 1] == 't')
#define CPP_ENDWITH_ASS(path) (path[path.size()-4] == '.' && path[path.size() - 3] == 'a' \
	&& path[path.size()-2] == 's' && path[path.size() - 1] == 's')
#define C_ENDWITH_SRT(path) (path[strlen(path)-4] == '.' && path[strlen(path) - 3] == 's' \
	&& path[strlen(path)-2] == 'r' && path[strlen(path) - 1] == 't')
#define C_ENDWITH_ASS(path) (path[strlen(path)-4] == '.' && path[strlen(path) - 3] == 'a' \
	&& path[strlen(path)-2] == 's' && path[strlen(path) - 1] == 's')

class SubtitleFile
{
	friend std::ostream& operator<<(std::ostream& cout, const SubtitleFile& sf);
public:
	SubtitleFile() = delete;
	//std::ostream& operator<<(std::ostream& cout, const SubtitleFile& sf);
	SubtitleFile(const std::string dirPath,int reservSize = REVERSE_SIZE);
	SubtitleFile(const char* dirPath, int reservSize = REVERSE_SIZE);
	~SubtitleFile();

	void FindSubtitleFiles(std::string path);
	//const std::vector<std::string>& GetSubtitleFilePaths(const std::string path) const;
	const std::unordered_set<std::string>& GetSubtitleFilePaths() const;

	std::unordered_map<std::string, std::wstring>& GetSubtitleDatas();

private:
	void FindSubtitleFilesImp(std::string path);
	void ReadSubtitleFilesImp();
	void ReadSubtitleDataImp(const std::string path);
	void ProcRootDirPathImp();

private:
	std::string m_RootDirPath;
	//std::vector<std::string> m_SubtitleFilePaths;
	std::unordered_set<std::string> m_SubtitleFilePaths;
	std::unordered_map<std::string,std::wstring> m_SubtitleDatas;
};


static std::ostream& operator<<(std::ostream& cout, const SubtitleFile& sf)
{
	for (const auto& path : sf.m_SubtitleFilePaths)
	{
		cout << path << std::endl;
	}
	return cout;
}

