#include "SubtitleDecoder.h"
#include <iostream>

SubtitleDecoder::SubtitleDecoder(const std::string& inFilePath, const std::string& outFileDir)
	:m_FormatContextPtr(nullptr), m_CodecContextPtr(nullptr), m_SubtitleStreamIndex(-1),
	m_SubtitleType(SUBTITLE_NONE), m_InFilePath(inFilePath), m_OutFileDir(outFileDir)
{
	FFmpegInit();

	ProcPathImp();
}

SubtitleDecoder::~SubtitleDecoder()
{
	// 释放资源
	avcodec_free_context(&m_CodecContextPtr);
	// 关闭媒体文件
	avformat_close_input(&m_FormatContextPtr);
}

void SubtitleDecoder::Decode()
{
	DecodeImp();
}

void SubtitleDecoder::DecodeImp()
{
	AVPacket packet;
	int gotSubtitle, funcRet = -1;
	AVSubtitle subtitle;
	while (av_read_frame(m_FormatContextPtr, &packet) >= 0) {
		if (packet.stream_index == m_SubtitleStreamIndex) {
			// avcodec_decode_subtitle2 失败返回负数 否则返回已用字节数
			funcRet = avcodec_decode_subtitle2(m_CodecContextPtr, &subtitle, &gotSubtitle, &packet);
			if (funcRet >= 0 && gotSubtitle != 0) {
				// 保存字幕文件
				SaveSubtitleFlieImp(subtitle);
			}
			avsubtitle_free(&subtitle);
		}
		av_packet_unref(&packet);
	}
}

inline void SubtitleDecoder::SaveSubtitleFlieImp(const AVSubtitle& subtitle)
{
	std::ofstream ofs;

	JudgeSubtitleTypeImp(subtitle, ofs);
	if (m_SubtitleType == SUBTITLE_NONE)
	{
		return;
	}

	for (unsigned int i = 0; i < subtitle.num_rects; i++)
	{

		switch (m_SubtitleType)
		{
			case SUBTITLE_TEXT:
			{
				ofs << subtitle.rects[i]->text << std::endl;
				break;
			}
			case SUBTITLE_ASS:
			{
				ofs << subtitle.rects[i]->ass << std::endl;
				break;
			}
			case SUBTITLE_BITMAP:
			{// Handle bitmap subtitles
				std::cout << "Subtitle Type: SUBTITLE_BITMAP" << std::endl;
				break;
			}
			default:
				break;
		}
	}

	ofs.close();
}

inline void SubtitleDecoder::JudgeSubtitleTypeImp(const AVSubtitle& subtitle, std::ofstream& ofs)
{
	if (subtitle.num_rects > 0 && (!JudgeOutFilePathExistImp()))
	{
		switch (subtitle.rects[0]->type)
		{
			case SUBTITLE_TEXT:
			{
				m_OutFileDir += ".srt";
				m_SubtitleType = SUBTITLE_TEXT;
				break;

			}
			case SUBTITLE_ASS:
			{
				m_OutFileDir += ".ass";
				m_SubtitleType = SUBTITLE_ASS;
				break;

			}
			case SUBTITLE_BITMAP:
			{
				m_SubtitleType = SUBTITLE_BITMAP;
				break;
			}
			default:
				break;
		}

	}
	ofs.open(m_OutFileDir, std::ios::out | std::ios::app);
	if (!ofs.is_open())
	{
		std::cout << m_OutFileDir << "打开失败" << std::endl;
	}
}

inline bool SubtitleDecoder::JudgeOutFilePathExistImp()
{
	if (m_OutFileDir.size() > 4 && 
		(CPP_ENDWITH_SRT(m_OutFileDir) || CPP_ENDWITH_ASS(m_OutFileDir)))
	{
		return true;
	}
	return false;
}

void SubtitleDecoder::ProcPathImp()
{
	// 将输入输出文件/文件夹的 '\' 转为 '/'
	size_t pos = m_InFilePath.find("\\");
	while (pos != std::string::npos)
	{
		m_InFilePath.replace(pos, 1, "/");
		pos = m_InFilePath.find("\\");
	}
	pos = m_OutFileDir.find("\\");
	while (pos != std::string::npos)
	{
		m_OutFileDir.replace(pos, 1, "/");
		pos = m_OutFileDir.find("\\");
	}

	if (m_OutFileDir[m_OutFileDir.size()-1] == '/')
	{
		m_OutFileDir.resize(m_OutFileDir.size() - 1);
	}

	// 删除后面的格式 拼接为 输出文件夹/文件名 的无格式文件路径
	pos = m_InFilePath.rfind("/");
	m_OutFileDir += m_InFilePath.substr(pos);
	pos = m_OutFileDir.rfind(".");
	m_OutFileDir.resize(pos);
}

void SubtitleDecoder::FFmpegInit()
{
	// 打开媒体文件
	avformat_open_input(&m_FormatContextPtr, m_InFilePath.c_str(), nullptr, nullptr);
	// 获取媒体文件信息
	avformat_find_stream_info(m_FormatContextPtr, nullptr);

	// 查找字幕流 codecpar->codec_type判断流类型
	for (unsigned int i = 0; i < m_FormatContextPtr->nb_streams; i++) {
		if (m_FormatContextPtr->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			m_SubtitleStreamIndex = i;
			break;
		}
	}

	if (m_SubtitleStreamIndex == -1)
	{
		std::cout << "未找到字幕流" << std::endl;
	}

	// 查找解码器
	AVCodecParameters* codecParameters = m_FormatContextPtr->streams[m_SubtitleStreamIndex]->codecpar;
	const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);

	// 创建解码器上下文
	m_CodecContextPtr = avcodec_alloc_context3(codec);
	if (avcodec_open2(m_CodecContextPtr, codec, nullptr) != 0)
	{
		std::cout << "解码器初始化失败" << std::endl;
	}
}
