#include "SubtitleFile.h"

SubtitleFile::SubtitleFile(const std::string path, int reserve_size)
	:m_RootDirPath(path)
{
	m_SubtitleFilePaths.reserve(reserve_size);
	ProcRootDirPathImp();
	FindSubtitleFilesImp(m_RootDirPath);
	ReadSubtitleFilesImp();
}

SubtitleFile::SubtitleFile(const char* path, int reserve_size)
	:m_RootDirPath(path)
{
	m_SubtitleFilePaths.reserve(reserve_size);
	ProcRootDirPathImp();
	FindSubtitleFilesImp(m_RootDirPath);
	ReadSubtitleFilesImp();
}

SubtitleFile::~SubtitleFile()
{

}

void SubtitleFile::FindSubtitleFiles(std::string path)
{
	FindSubtitleFilesImp(path);
}

const std::unordered_set<std::string>& SubtitleFile::GetSubtitleFilePaths() const
{
	return m_SubtitleFilePaths;
}

std::unordered_map<std::string, std::wstring>& SubtitleFile::GetSubtitleDatas()
{
	return m_SubtitleDatas;
}

void SubtitleFile::FindSubtitleFilesImp(std::string path)
{
#ifdef _WIN64
	/*
	报错：_findnext() (ntdll.dll)处引发的异常: 0xC0000005: 写入位置 0xFFFFFFFFD2A51A80 时发生访问冲突。
	将long 改为 intptr_t
	*/
	intptr_t handleFile = -1;
	struct _finddata_t fileInfo;
	if ((handleFile = _findfirst(path.append("/*").c_str(), &fileInfo)) != -1)
	{
		path.resize(path.size() - 1);
		do
		{
			if (fileInfo.attrib & _A_SUBDIR)	// 文件夹 
			{
				if (strcmp(fileInfo.name,".") != 0 && strcmp(fileInfo.name, "..") != 0)
				{
					FindSubtitleFilesImp(path.append(fileInfo.name));
					path.resize(path.size() - strlen(fileInfo.name)); // 子文件夹已经遍历完成
				}
			}
			else if (strlen(fileInfo.name) > 4 && (C_ENDWITH_ASS(fileInfo.name) || C_ENDWITH_SRT(fileInfo.name)))
			{
				//m_SubtitleFilePaths.emplace_back(path.append(fileInfo.name));
				m_SubtitleFilePaths.emplace(path.append(fileInfo.name));
				path.resize(path.size() - strlen(fileInfo.name));
			}

		} while (_findnext(handleFile,&fileInfo) == 0);

		_findclose(handleFile);
	}
#endif // _WIN64
}

void SubtitleFile::ReadSubtitleFilesImp()
{
	
	for (const auto& path : m_SubtitleFilePaths)
	{
		ReadSubtitleDataImp(path);
	}
}

void SubtitleFile::ReadSubtitleDataImp(const std::string path)
{
	const std::locale utf8(std::locale(), new std::codecvt_utf8<wchar_t>());

	std::wifstream wifs(path, std::ios::in);
	wifs.imbue(utf8);
	std::wstring data;

	if (wifs.is_open())
	{
		std::wstring wline;
		while (std::getline(wifs,wline))
		{
			data += wline;
			data += static_cast<wchar_t>('\n');
		}
	}

	m_SubtitleDatas.emplace(path,data);
	wifs.close();
}

void SubtitleFile::ProcRootDirPathImp()
{
	size_t pos = m_RootDirPath.find("\\");
	while (pos != std::string::npos)
	{
		m_RootDirPath.replace(pos, 1, "/");
		pos = m_RootDirPath.find("\\");
	}
	if (m_RootDirPath[m_RootDirPath.size()-1] == '/')
	{
		m_RootDirPath.resize(m_RootDirPath.size() - 1);
	}
}



