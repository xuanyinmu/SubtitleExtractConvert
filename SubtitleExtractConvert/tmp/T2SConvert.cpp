#include "T2SConvert.h"

T2SConvert::T2SConvert(const std::string dirPath, int reserveSize,
	const std::string charDictPath,const std::string phraseDictPath)
{
	m_SFPtr = std::make_unique<SubtitleFile>(dirPath, reserveSize);
	m_T2SDictPtr = std::make_unique<T2SDictionary>(charDictPath, phraseDictPath);
	ConvertDatasImp();

}

T2SConvert::T2SConvert(const char* dirPath, int reserveSize, const char* charDictPath, const char* phraseDictPath)
{
	m_SFPtr = std::make_unique<SubtitleFile>(dirPath, reserveSize);
	m_T2SDictPtr = std::make_unique<T2SDictionary>(charDictPath, phraseDictPath);
	ConvertDatasImp();
}


T2SConvert::~T2SConvert()
{

}

void T2SConvert::SaveDatas()
 {
	auto& subtitleDatasUMap = m_SFPtr->GetSubtitleDatas();
	for (const auto& wdata : subtitleDatasUMap)
	{
		SaveDataImp(wdata);
	}
	std::cout << "全部转换完成" << std::endl;
}

void T2SConvert::PrintSubtitleFilesPaths() const
{
	const auto& paths = m_SFPtr->GetSubtitleFilePaths();
	for (const auto& path : paths)
	{
		std::cout << path << std::endl;
	}
}

void T2SConvert::ConvertDatasImp()
{
	auto& subtitleDatasUMap = m_SFPtr->GetSubtitleDatas();
	const auto& charDictUMap = m_T2SDictPtr->GetCharacterDict();
	const auto& phraseDictUMap = m_T2SDictPtr->GetPhraseDict();

	for (auto& wdata : subtitleDatasUMap)
	{
		// 先处理短语
		ConvertPhraseImp(wdata, phraseDictUMap);
		// 再处理单词
		ConvertCharacterImp(wdata, charDictUMap);
	}
}

void T2SConvert::ConvertPhraseImp(std::pair<const std::string,std::wstring>& wdata,
		const std::unordered_map<std::wstring, std::wstring>& phraseDictUMap)
{
	std::wstring::size_type pos;
	for (const auto& phrasePair : phraseDictUMap)
	{

		if ((pos = wdata.second.find(phrasePair.first)) != std::wstring::npos)
		{
			wdata.second.replace(pos, 1, phrasePair.second);
		}
	}
}

void T2SConvert::ConvertCharacterImp(std::pair<const std::string, std::wstring>& wdata,
		const std::unordered_multimap<wchar_t, wchar_t>& charDictUMap)
{
	for (auto& character : wdata.second)
	{
		// TODO
		// 可能有多个值 -- 选哪个
		/*auto range = charDictUMap.equal_range(character);
		for (auto it = range.first; it != range.second; it++)
		{

		}*/
		auto it = charDictUMap.find(character);
		if (it != charDictUMap.end())
		{
			character = it->second;
		}
	}
}


void T2SConvert::SaveDataImp(const std::pair<std::string, std::wstring>& wdata)
{
	const std::locale utf8(std::locale(), new std::codecvt_utf8<wchar_t>);
	std::wofstream wofs(wdata.first, /*std::ios::trunc | */std::ios::out);
	wofs.imbue(utf8);
	wofs << wdata.second;
	wofs.close();
}
