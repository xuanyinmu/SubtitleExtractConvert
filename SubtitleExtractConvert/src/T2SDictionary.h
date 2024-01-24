#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>

class T2SDictionary
{
	friend std::ostream& operator<<(std::ostream& cout, const T2SDictionary& convert);
public:
	T2SDictionary() = delete;
	T2SDictionary(const std::string charDictPath = "./res/T2SConvertDict/TSCharacters.txt",
				const std::string phraseDictPath="./res/T2SConvertDict/TSPhrases.txt");
	T2SDictionary(const char* charDictPath = "./res/T2SConvertDict/TSCharacters.txt",
		const char* phraseDictPath = "./res/T2SConvertDict/TSPhrases.txt");
	~T2SDictionary();
	void LoadDictionary();
	const std::unordered_multimap<wchar_t, wchar_t>& GetCharacterDict() const;
	const std::unordered_map<std::wstring, std::wstring>& GetPhraseDict() const;

private:
	void LoadDictionaryImp();
	void InsertCharMapImp(const std::wstring& str);
	void InsertPhraseMapImp(const std::wstring& str);

private:
	std::unordered_multimap<wchar_t, wchar_t> m_CharactersDict;
	std::unordered_map<std::wstring,std::wstring> m_PhrasesDict;
	std::string m_CharsDictPath;
	std::string m_PhrasesDictPath;

};

static std::ostream& operator<<(std::ostream& cout, const T2SDictionary& convert)
{
	const std::locale utf8(std::locale(), new std::codecvt_utf8<wchar_t>);
	std::wcout.imbue(utf8);
	//std::wstring wKey, wValue;
	cout << "×Ö·û×Öµä" << std::endl;
	for (const auto& kv : convert.m_CharactersDict)
	{
		std::wcout << kv.first << " : " << kv.second << std::endl;
	}

	cout << "¶ÌÓï×Öµä" << std::endl;
	for (const auto& kv : convert.m_PhrasesDict)
	{
		std::wcout << kv.first << " : " << kv.second << std::endl;
	}

	return cout;
}
