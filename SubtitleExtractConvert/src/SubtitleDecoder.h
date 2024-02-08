#pragma once
#include <string>
#include <fstream>
#include <memory>
//#include <vector>
#include <unordered_map>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#define CPP_ENDWITH_SRT(path) (path[path.size()-4] == '.' && path[path.size() - 3] == 's' \
	&& path[path.size()-2] == 'r' && path[path.size() - 1] == 't')
#define CPP_ENDWITH_ASS(path) (path[path.size()-4] == '.' && path[path.size() - 3] == 'a' \
	&& path[path.size()-2] == 's' && path[path.size() - 1] == 's')


struct SubtitleStreamInfo
{
	AVCodecContext* codecContext;
	std::string title;
};


class SubtitleDecoder
{
public:
	SubtitleDecoder(const std::string& inFilePath, const std::string &outFileDir = "C:/Users/Layin/Downloads");
	~SubtitleDecoder();
	void Decode();

private:
	void DecodeImp();
	inline void SaveSubtitleFlieImp(const AVSubtitle& subtitle, const std::string& metaTitle);
	inline void JudgeSubtitleTypeImp(const AVSubtitle& subtitle, std::ofstream& ofs);
	inline bool JudgeOutFilePathExistImp();
	void ProcPathImp();
	void FFmpegInit();

private:
	/*std::unique_ptr<AVFormatContext> m_FormatContextPtr;
	std::unique_ptr<AVCodecContext> m_CodecContextPtr;*/

	AVFormatContext* m_FormatContextPtr;
	//AVCodecContext* m_CodecContextPtr;
	//int m_SubtitleStreamIndex;

	std::unordered_map<int, SubtitleStreamInfo> m_IndexCodecContextUMap;
	AVSubtitleType m_SubtitleType;
	std::string m_InFilePath;
	std::string m_OutFileDir;
	//std::ofstream m_OutStream;
	
};

