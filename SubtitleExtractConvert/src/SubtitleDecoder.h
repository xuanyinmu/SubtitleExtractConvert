#pragma once
#include <string>
#include <fstream>
#include <memory>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class SubtitleDecoder
{
public:
	SubtitleDecoder(const std::string& inFilePath, const std::string &outFileDir = "C:/Users/Layin/Downloads");
	~SubtitleDecoder();
	void Decode();

private:
	void DecodeImp();
	inline void SaveSubtitleFlieImp(const AVSubtitle& subtitle);
	inline void JudgeSubtitleTypeImp(const AVSubtitle& subtitle);
	void ProcPathImp();
	void FFmpegInit();

private:
	/*std::unique_ptr<AVFormatContext> m_FormatContextPtr;
	std::unique_ptr<AVCodecContext> m_CodecContextPtr;*/
	AVFormatContext* m_FormatContextPtr;
	AVCodecContext* m_CodecContextPtr;
	int m_SubtitleStreamIndex;
	AVSubtitleType m_SubtitleType;
	std::string m_InFilePath;
	std::string m_OutFileDir;
	
};

