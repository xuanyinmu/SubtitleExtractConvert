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
	// �ͷ���Դ
	avcodec_free_context(&m_CodecContextPtr);
	// �ر�ý���ļ�
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
			// avcodec_decode_subtitle2 ʧ�ܷ��ظ��� ���򷵻������ֽ���
			funcRet = avcodec_decode_subtitle2(m_CodecContextPtr, &subtitle, &gotSubtitle, &packet);
			if (funcRet >= 0 && gotSubtitle != 0) {
				// ������Ļ�ļ�
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
		std::cout << m_OutFileDir << "��ʧ��" << std::endl;
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
	// ����������ļ�/�ļ��е� '\' תΪ '/'
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

	// ɾ������ĸ�ʽ ƴ��Ϊ ����ļ���/�ļ��� ���޸�ʽ�ļ�·��
	pos = m_InFilePath.rfind("/");
	m_OutFileDir += m_InFilePath.substr(pos);
	pos = m_OutFileDir.rfind(".");
	m_OutFileDir.resize(pos);
}

void SubtitleDecoder::FFmpegInit()
{
	// ��ý���ļ�
	avformat_open_input(&m_FormatContextPtr, m_InFilePath.c_str(), nullptr, nullptr);
	// ��ȡý���ļ���Ϣ
	avformat_find_stream_info(m_FormatContextPtr, nullptr);

	// ������Ļ�� codecpar->codec_type�ж�������
	for (unsigned int i = 0; i < m_FormatContextPtr->nb_streams; i++) {
		if (m_FormatContextPtr->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			m_SubtitleStreamIndex = i;
			break;
		}
	}

	if (m_SubtitleStreamIndex == -1)
	{
		std::cout << "δ�ҵ���Ļ��" << std::endl;
	}

	// ���ҽ�����
	AVCodecParameters* codecParameters = m_FormatContextPtr->streams[m_SubtitleStreamIndex]->codecpar;
	const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);

	// ����������������
	m_CodecContextPtr = avcodec_alloc_context3(codec);
	if (avcodec_open2(m_CodecContextPtr, codec, nullptr) != 0)
	{
		std::cout << "��������ʼ��ʧ��" << std::endl;
	}
}
