#include "T2SDictionary.h"

T2SDictionary::T2SDictionary(const std::string charDictPath, const std::string phraseDictPath)
	:m_CharsDictPath(charDictPath),m_PhrasesDictPath(phraseDictPath)
{
	LoadDictionaryImp();
}

T2SDictionary::T2SDictionary(const char* charDictPath, const char* phraseDictPath)
	:m_CharsDictPath(charDictPath), m_PhrasesDictPath(phraseDictPath)
{
	LoadDictionaryImp();
}

T2SDictionary::~T2SDictionary()
{

}

void T2SDictionary::LoadDictionary()
{
	LoadDictionaryImp();
}

const std::unordered_multimap<wchar_t, wchar_t>& T2SDictionary::GetCharacterDict() const
{
	return m_CharactersDict;
}

const std::unordered_map<std::wstring, std::wstring>& T2SDictionary::GetPhraseDict() const
{
	return m_PhrasesDict;
}


/*
	utf-8文件读写问题 -- https://sf-zhou.github.io/programming/chinese_encoding.html
*/
void T2SDictionary::LoadDictionaryImp()
{
	const std::locale utf8(std::locale(), new std::codecvt_utf8<wchar_t>());
	
	std::wifstream charStream(m_CharsDictPath, std::ios::in);
	charStream.imbue(utf8);

	std::wstring wline;
	if (charStream.is_open())
	{
		while (std::getline(charStream,wline))
		{
			InsertCharMapImp(wline);
		}
	}
	charStream.close();

	std::wifstream phraseStream(m_PhrasesDictPath, std::ios::in);
	phraseStream.imbue(utf8);

	if (phraseStream.is_open())
	{
		while (std::getline(phraseStream, wline))
		{
			InsertPhraseMapImp(wline);
		}
	}
	phraseStream.close();
}

void T2SDictionary::InsertCharMapImp(const std::wstring& str)
{
	const wchar_t wspace = static_cast<wchar_t>(' ');
	const wchar_t wtab = static_cast<wchar_t>('\t');
	size_t len = str.size();
	for (size_t i = 1; i < len; i++)
	{
		if (str[i] != wspace && str[i] != wtab)
		{
			m_CharactersDict.emplace(str[0], str[i]);
		}
	}
}

void T2SDictionary::InsertPhraseMapImp(const std::wstring& str)
{
	const wchar_t wspace = static_cast<wchar_t>(' ');
	const wchar_t wtab = static_cast<wchar_t>('\t');
	std::wstring::size_type pos = (str.find(wspace) == std::wstring::npos ? str.find(wtab) : str.find(wspace));
	std::wstring key, value;

	key = str.substr(0, pos);
	value = str.substr(pos + 1);
	m_PhrasesDict.emplace(key,value);
}
