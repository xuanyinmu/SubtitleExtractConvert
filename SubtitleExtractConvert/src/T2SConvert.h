#pragma once
#include "SubtitleFile.h"
#include "T2SDictionary.h"
#include <memory>

class T2SConvert
{
public:
	T2SConvert() = delete;
	T2SConvert(const std::string dirPath, int reservSize = REVERSE_SIZE,
		const std::string charDictPath = "./res/T2SConvertDict/TSCharacters.txt",
		const std::string phraseDictPath = "./res/T2SConvertDict/TSPhrases.txt");
	T2SConvert(const char* path, int reservSize = REVERSE_SIZE,
		const char* charDictPath = "./res/T2SConvertDict/TSCharacters.txt",
		const char* phraseDictPath = "./res/T2SConvertDict/TSPhrases.txt");
	~T2SConvert();
	void SaveDatas();
	void PrintSubtitleFilesPaths() const;

private:
	void ConvertDatasImp();
	void ConvertPhraseImp(std::pair<const std::string, std::wstring>& wdata,
		const std::unordered_map<std::wstring, std::wstring>& phraseDictUMap);
	void ConvertCharacterImp(std::pair<const std::string, std::wstring>& wdata,
		const std::unordered_multimap<wchar_t, wchar_t>& charDictUMap);
	void SaveDataImp(const std::pair < std::string, std::wstring>& wdata);

private:
	std::unique_ptr<SubtitleFile> m_SFPtr;
	std::unique_ptr<T2SDictionary> m_T2SDictPtr;
};

