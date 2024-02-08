#include "SubtitleDecoder.h"
#include <iostream>

SubtitleDecoder::SubtitleDecoder(const std::string& inFilePath, const std::string& outFileDir)
	:m_FormatContextPtr(nullptr), /*m_CodecContextPtr(nullptr), m_SubtitleStreamIndex(-1),*/
	m_SubtitleType(SUBTITLE_NONE), m_InFilePath(inFilePath), m_OutFileDir(outFileDir)
{
	FFmpegInit();

	ProcPathImp();
}

SubtitleDecoder::~SubtitleDecoder()
{
	// 释放解码器上下文
	for (auto& codecContext : m_IndexCodecContextUMap)
	{
		avcodec_free_context(&codecContext.second.codecContext);
	}
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
	
	// 读取媒体文件的帧 av_read_frame 
	while (av_read_frame(m_FormatContextPtr, &packet) >= 0) 
	{
		for (const auto& pair : m_IndexCodecContextUMap)
		{
			if (packet.stream_index == pair.first) {
				if (!pair.second.codecContext)
				{
					std::cerr << "codecContext == nullptr" << std::endl;
				}
				// avcodec_decode_subtitle2 使用对应解码器上下文界面字幕
				funcRet = avcodec_decode_subtitle2(pair.second.codecContext, &subtitle, &gotSubtitle, &packet);
				if (funcRet >= 0 && gotSubtitle != 0) {
					// 保存字幕信息
					SaveSubtitleFlieImp(subtitle, pair.second.title);
					m_OutFileDir.resize(m_OutFileDir.size() - pair.second.title.size());
				}
				avsubtitle_free(&subtitle);
			}
			av_packet_unref(&packet);
		}
	}
	
}

inline void SubtitleDecoder::SaveSubtitleFlieImp(const AVSubtitle& subtitle, const std::string& metaTitle)
{
	std::ofstream ofs;
	
	m_OutFileDir += metaTitle;

	JudgeSubtitleTypeImp(subtitle, ofs);
	if (m_SubtitleType == SUBTITLE_NONE)
	{
		std::cerr << "m_SubtitleType == SUBTITLE_NONE" << std::endl;
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
		std::cerr << m_OutFileDir << "打开失败" << std::endl;
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
	// 把所有 '\' 换成 '/'
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

	// 输出路径拼接成 输出根目录/输入文件名 无格式
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

	m_IndexCodecContextUMap.reserve(10);
	// 查找字幕流 codecpar->codec_type 流类型
	for (unsigned int i = 0; i < m_FormatContextPtr->nb_streams; i++)
	{
		if (m_FormatContextPtr->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
		{
			m_IndexCodecContextUMap.emplace(i, SubtitleStreamInfo({ nullptr, ""}));
		}
	}

	if (m_IndexCodecContextUMap.empty())
	{
		std::cerr << "找不到字幕流" << std::endl;
	}

	AVDictionaryEntry* tag = nullptr;
	for (auto& pair : m_IndexCodecContextUMap)
	{
		// 查找解码器
		AVCodecParameters* codecParameters = m_FormatContextPtr->streams[pair.first]->codecpar;
		const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);

		/*
		// 创建解码器上下文 avcodec_alloc_context3
		// 打开解码器 avcodec_open2
		通常只需要保存AVCodecContext*， 用来读取所有帧和数据
		*/
		AVCodecContext* codecContext = avcodec_alloc_context3(codec);
		if (avcodec_open2(codecContext, codec, nullptr) == 0)
		{
			tag = av_dict_get(m_FormatContextPtr->streams[pair.first]->metadata, "title", nullptr, 0);
            m_IndexCodecContextUMap[pair.first] = { codecContext, "" };
			if (tag)
			{
				m_IndexCodecContextUMap[pair.first] = { codecContext, tag->value };
			}
		}
		else
		{
			std::cerr << pair.first <<" : " << "解码器打开失败" << std::endl;
		}
	}
}
