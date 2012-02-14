#ifndef MOBILECOMMANDS_H
#define MOBILECOMMANDS_H

//#pragma once 
#include "TransByteOrderStream.h"
//////////////////////////////////////////////////////////////////////////
// namespace test_namespace
// {

	typedef xl::uint8  uint8;
	typedef xl::uint16 uint16;
	typedef xl::uint32 uint32;
	typedef xl::uint64 uint64;

#define BOOL uint32 //�����������windows BOOL��ͻ�����Զ����BOOL

	const int SIZE_OF_UINT16 = sizeof(uint16);
	const int SIZE_OF_UINT32 = sizeof(uint32);
	const int SIZE_OF_UINT64 = sizeof(uint64);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	/// mobile command-name ///
	const uint32 MOBILE_REQUEST_LOG_WIFI = 97;
	const uint32 MOBILE_RESPONSE_LOG_WIFI = 98;
	const uint32 MOBILE_RESPONSE_ERROR = 100;
	const uint32 MOBILE_REQUEST_TIMETICK = 101;
	const uint32 MOBILE_RESPONSE_TIMETICK = 102;
	const uint32 MOBILE_REQUEST_LIST_PARTITION = 103;
	const uint32 MOBILE_RESPONSE_LIST_PARTITION = 104;
	const uint32 MOBILE_REQUEST_GET_PARTITION_INFO = 105;
	const uint32 MOBILE_RESPONSE_GET_PARTITION_INFO = 106;
	const uint32 MOBILE_REQUEST_LIST_PATH = 107;
	const uint32 MOBILE_RESPONSE_LIST_PATH = 108;
	const uint32 MOBILE_REQUEST_DELETE_FILE = 109;
	const uint32 MOBILE_RESPONSE_DELETE_FILE = 110;
	const uint32 MOBILE_REQUEST_RENAME_FILE = 111;
	const uint32 MOBILE_RESPONSE_RENAME_FIEL = 112;
	const uint32 MOBILE_REQUEST_IS_EXIST_FIEL = 113;
	const uint32 MOBILE_RESPONSE_IS_EXIST_FILE = 114;
	const uint32 MOBILE_REQUEST_GET_MEDIA_INFO = 115;
	const uint32 MOBILE_RESPONSE_GET_MEDIA_INFO = 116;
	const uint32 MOBILE_REQUEST_CAPTURE_FROM_VEDIO = 117;
	const uint32 MOBILE_RESPONSE_CAPTURE_FROM_VEDIO = 118;
	const uint32 MOBILE_REQUEST_CAPTURE_FROM_IMAGE = 119;
	const uint32 MOBILE_RESPONSE_CAPTURE_FROM_IMAGE = 120;
	const uint32 MOBILE_REQUEST_FIND_FILE = 121;
	const uint32 MOBILE_RESPONSE_FIND_FILE = 122;
	const uint32 MOBILE_REQUEST_GET_MP3_INFO = 123;
	const uint32 MOBILE_RESPONSE_GET_MP3_INFO =124;
	const uint32 MOBILE_REQUEST_SEND_FILE = 125;
	const uint32 MOBILE_RESPONSE_SEND_FILE = 126;
	const uint32 MOBILE_REQUEST_RECV_FILE = 127;
	const uint32 MOBILE_RESPONSE_RECV_FILE = 128;
	const uint32 MOBILE_REQUEST_GET_FILE_SIZE = 129;
	const uint32 MOBILE_RESPONSE_GET_FILE_SIZE = 130;

	//////////////////////////////////////////////////////////////////////////
	/// mobile commands' based class --- CCommandBase
	struct CCommandBase
	{
		typedef xl::uint16	uint16;
		typedef xl::uint32	uint32;
		typedef xl::uint64	uint64;
		typedef uint32		BOOL;
		typedef std::string String;
		

		// 		template <typename T>
		// 		class Array : public std::vector<T> {};
#define Array std::vector	

		uint32    m_protocol_version;
		uint32    m_sequence;
		uint32    m_command_length;

		virtual ~CCommandBase() {}
	protected:
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	public:
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// mobile request commands' based class --- CCommandRequestBase
	struct CCommandRequestBase : public CCommandBase
	{
		typedef CCommandBase CBase;
		uint32	m_command_name;
		uint32	m_command_flag;
		uint32	m_priority_level;

		// 		CCommandRequestBase()
		// 			: m_command_name(0), m_command_flag(0) {}
		CCommandRequestBase(uint32 name)
			: m_command_name(name), m_command_flag(0) {}
			CCommandRequestBase(uint32 name, uint32 flag)
				: m_command_name(name), m_command_flag(flag) {}
	protected:
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	private:
		//���ǻ���÷��������������
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	public:
		unsigned Length() const;
		bool SetPriority(uint32 priority);
	};

	//////////////////////////////////////////////////////////////////////////
	/// mobile response commands' based class --- CCommandResponseBase
	struct CCommandResponseBase : public CCommandBase
	{
		typedef CCommandBase CBase;

		uint32    m_command_name;
		uint32    m_command_flag;
		BOOL      m_result;

		// 		CCommandResponseBase()
		// 			: m_command_name(0), m_command_flag(0) {}
		CCommandResponseBase(uint32 name)
			: m_command_name(name), m_command_flag(0) {}
			CCommandResponseBase(uint32 name, uint32 flag)
				: m_command_name(name), m_command_flag(flag) {}
	protected:
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	private:
		//���ǻ���÷��������������
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);

	public:
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 1��	������Ӧ����
	struct CErrorResponse : public CCommandResponseBase
	{
		typedef CCommandResponseBase CBase;
		uint32 m_error_code;

		CErrorResponse()
			:CCommandResponseBase(MOBILE_RESPONSE_ERROR)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	//////////////////////////////////////////////////////////////////////////
	/// 2��	��¼���Ԥ����wifi��
	struct CLogWifiRequest : public CCommandRequestBase
	{
		String m_password;

		typedef CCommandRequestBase CBase;
		CLogWifiRequest()
			: CCommandRequestBase(MOBILE_REQUEST_LOG_WIFI)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CLogWifiResponse : public CCommandResponseBase
	{
		String m_phone_id;

		typedef CCommandResponseBase CBase;
		CLogWifiResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_LOG_WIFI)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 3��	��������
	struct CTimeTickRequset : public CCommandRequestBase
	{
		typedef CCommandRequestBase CBase;
		CTimeTickRequset()
			: CCommandRequestBase(MOBILE_REQUEST_TIMETICK)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	struct CTimeTickResponse : public CCommandResponseBase
	{
		typedef CCommandResponseBase CBase;
		CTimeTickResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_TIMETICK)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	//////////////////////////////////////////////////////////////////////////
	/// 4��	List����
	struct CListPartitionRequest : public CCommandRequestBase
	{
		typedef CCommandRequestBase CBase;
		CListPartitionRequest()
			: CCommandRequestBase(MOBILE_REQUEST_LIST_PARTITION)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CListPartitionResponse : public CCommandResponseBase
	{
		Array<uint32> m_partitions;

		typedef CCommandResponseBase CBase;
		CListPartitionResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_LIST_PARTITION)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 5��	��ȡ������Ϣ
	struct CGetPatitionInfoRequest : public CCommandRequestBase
	{
		typedef CCommandRequestBase CBase;
		uint32	m_partition_index;

		CGetPatitionInfoRequest()
			: CCommandRequestBase(MOBILE_REQUEST_GET_PARTITION_INFO)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CGetPatitionInfoResponse : public CCommandResponseBase
	{
		uint64	m_patition_total_size;
		uint64	m_partition_remain_size;

		typedef CCommandResponseBase CBase;
		CGetPatitionInfoResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_PARTITION_INFO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 6��	ListĿ¼
	struct CFileStruct
	{
		typedef CCommandBase::String String;
		uint32	m_struct_length;
		String	m_file_name;
		uint64	m_file_size;
		BOOL	m_is_folder;
		BOOL	m_is_visible;
		BOOL	m_is_writable;
		String	m_file_type;
		String	m_modify_time;
		String	m_create_time;

		CFileStruct() {}
		CFileStruct(const CFileStruct& obj)
			: m_struct_length(obj.m_struct_length), m_file_name(obj.m_file_name),
			m_file_size(obj.m_file_size), m_is_folder(obj.m_is_folder),
			m_is_visible(obj.m_is_visible), m_file_type(obj.m_file_type),
			m_modify_time(obj.m_modify_time), m_create_time(obj.m_create_time)
		{}

		unsigned Length() const;

		TransByteOrderIOStream& operator>> (TransByteOrderIOStream& ins);
		TransByteOrderIOStream& operator<< (TransByteOrderIOStream& outs);

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	};

	struct CListPathRequest : public CCommandRequestBase
	{
		uint32 m_partition_index;
		String m_path_name;
		uint32 m_file_mask;

		typedef CCommandRequestBase CBase;
		CListPathRequest()
			: CCommandRequestBase(MOBILE_REQUEST_LIST_PATH)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CListPathResponse : public CCommandResponseBase
	{
		Array<CFileStruct> m_file_structs;

		typedef CCommandResponseBase CBase;
		CListPathResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_LIST_PATH)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 7��	ɾ���ļ�
	struct CDeleteFileRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_full_filename;

		typedef CCommandRequestBase CBase;
		CDeleteFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_DELETE_FILE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CDeleteFileResponse : public CCommandResponseBase
	{
		typedef CCommandResponseBase CBase;
		CDeleteFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_DELETE_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 8��	�������ļ�
	struct CRenameFileRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_old_filename;
		String	m_new_filename;

		typedef CCommandRequestBase CBase;
		CRenameFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_RENAME_FILE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CRenameFileResponse : public CCommandResponseBase
	{
		typedef CCommandResponseBase CBase;
		CRenameFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_RENAME_FIEL)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 9��	�鿴�ļ��Ƿ����
	struct CIsExistFileRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_full_filename;
		BOOL	m_is_file;

		typedef CCommandRequestBase CBase;
		CIsExistFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_IS_EXIST_FIEL)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CIsExistFileResponse : public CCommandResponseBase
	{
		BOOL m_is_exist;

		typedef CCommandResponseBase CBase;
		CIsExistFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_IS_EXIST_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 10����ȡ��Ƶ����Ƶ��Ϣ
	struct CGetMediaInfoRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_full_filename;
		BOOL	m_is_audio;

		typedef CCommandRequestBase CBase;
		CGetMediaInfoRequest()
			: CCommandRequestBase(MOBILE_REQUEST_GET_MEDIA_INFO)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CGetMediaInfoResponse : public CCommandResponseBase
	{
		String	m_duration;
		String	m_audio_format;
		String	m_vedio_format;
		String	m_vedio_width_height;
		uint32	m_tatol_bitrate;
		uint32	m_audio_frequence;
		uint32	m_audio_bitrate;
		uint32	m_frames_per;

		typedef CCommandResponseBase CBase;
		CGetMediaInfoResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_MEDIA_INFO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 11����Ƶ��ͼ����ȡ��Ƶ��Ϣ����ͼ�ͻ�ȡ��Ƶ��Ϣû��Ҫ�����������
	struct CCaptureFromVedioRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_src_filename;
		String	m_picture_format;
		uint32	m_width;
		uint32	m_height;
		uint32	m_begin_second;
		BOOL	m_keep_ratio;
		uint32	m_backgroud;

		typedef CCommandRequestBase CBase;
		CCaptureFromVedioRequest()
			: CCommandRequestBase(MOBILE_REQUEST_CAPTURE_FROM_VEDIO)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CCaptureFromVedioResponse : public CCommandResponseBase
	{
		String	m_dst_filename;
		String	m_duration;
		String	m_audio_format;
		String	m_vedio_format;
		String	m_vedio_width_height;
		uint32	m_tatol_bitrate;
		uint32	m_audio_frequence;
		uint32	m_audio_bitrate;
		uint32	m_frames_per;

		typedef CCommandResponseBase CBase;
		CCaptureFromVedioResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_CAPTURE_FROM_VEDIO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 12��ͼƬ��ͼ����ͼתСͼ
	struct CCaptureFromeImageRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_src_filename;
		String	m_picture_format;
		uint32	m_width;
		uint32	m_height;
		BOOL	m_keep_ratio;
		uint32	m_backgroud;

		typedef CCommandRequestBase CBase;
		CCaptureFromeImageRequest()
			: CCommandRequestBase(MOBILE_REQUEST_CAPTURE_FROM_IMAGE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	struct CCaptureFromImageResponse : public CCommandResponseBase
	{
		String	m_dst_filename;

		typedef CCommandResponseBase CBase;
		CCaptureFromImageResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_CAPTURE_FROM_IMAGE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 13�������������������ļ���С������
	struct CTypeCalc
	{
		uint32	m_length;
		uint64	m_total_file_size;
		uint64	m_total_file_count;

		TransByteOrderIOStream& operator>> (TransByteOrderIOStream& ins)
		{
			ins >> m_length;
			ins >> m_total_file_size;
			ins >> m_total_file_count;

			return ins;
		}
		TransByteOrderIOStream& operator<< (TransByteOrderIOStream& outs)
		{
			outs << m_length;
			outs << m_total_file_size;
			outs << m_total_file_count;

			return outs;
		}
		unsigned Length() const 
		{
			return sizeof(m_length) + \
				sizeof(m_total_file_size) + \
				sizeof(m_total_file_count);
		}
	};
	struct CFindFilesRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_path_name;
		BOOL	m_need_recursion;
		Array< Array<String> > m_type_array;

		typedef CCommandRequestBase CBase;
		CFindFilesRequest()
			: CCommandRequestBase(MOBILE_REQUEST_FIND_FILE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CFindFileResponse : public CCommandResponseBase
	{
		Array<CTypeCalc>	m_type_collection;

		typedef CCommandResponseBase CBase;
		CFindFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_FIND_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 14����ȡmp3�ļ���Ϣ
	struct CGetMP3InfoRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_full_filename;

		typedef CCommandRequestBase CBase;
		CGetMP3InfoRequest()
			: CCommandRequestBase(MOBILE_REQUEST_GET_MP3_INFO)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CGetMP3InfoResponse : public CCommandResponseBase
	{
		String	m_duration;
		String	m_artist;
		String	m_album;

		typedef CCommandResponseBase CBase;
		CGetMP3InfoResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_MP3_INFO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 15��	�����ļ����ֻ��������ļ����ݰ����
	struct CSendFileRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_dst_filename;
		uint64	m_file_size;
		uint64	m_offset;
		String	m_file_content;//string ��ʾ�ļ����ݲ�����

		typedef CCommandRequestBase CBase;

		CSendFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_SEND_FILE)
		{}
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};


	struct CSendFileResponse : public CCommandResponseBase
	{
		uint64	m_success_offset;
		uint64	m_success_length;

		typedef CCommandResponseBase CBase;
		CSendFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_SEND_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 16��	���ֻ������ļ��������ļ����ݰ����
	struct CRecvFileRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_src_filename;

		typedef CCommandRequestBase CBase;

		CRecvFileRequest()
			:CCommandRequestBase(MOBILE_REQUEST_RECV_FILE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CRecvFileResponse : public CCommandResponseBase
	{
		uint64	m_file_size;
		uint64	m_offset;
		String	m_file_content;//string ��ʾ�ļ����ݲ�����
		typedef CCommandResponseBase CBase;

		CRecvFileResponse() 
			: CCommandResponseBase(MOBILE_RESPONSE_RECV_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 17��	�鿴�ļ���С�����������
	struct CGetFileSizeRequest : public CCommandRequestBase
	{
		uint32	m_partition_index;
		String	m_full_filename;

		typedef CCommandRequestBase CBase;

		CGetFileSizeRequest()
			:CCommandRequestBase(MOBILE_REQUEST_GET_FILE_SIZE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CGetFileSizeResponse : public CCommandResponseBase
	{
		uint64	m_file_size;
		typedef CCommandResponseBase CBase;

		CGetFileSizeResponse() 
			: CCommandResponseBase(MOBILE_RESPONSE_GET_FILE_SIZE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	//////////////////////////////////////////////////////////////////////////

	template <typename CommandType>
	struct CDefaultCtorHelper
	{
		typedef CommandType CCommandType;

		CDefaultCtorHelper() 
			: _obj_ptr(0) {}
			// 
			// 		static CCommandBase* CreateCommandObject();
	protected:
		CCommandType*	_obj_ptr;
	};

	// 	template <typename CommandType>
	// 		CCommandBase* CDefaultCtorHelper<CommandType>::CreateCommandObject()
	// 	{
	// 		return (new CommandType);
	// 	}

	interface IEncodeOperation
	{
	public:
		//�������ݣ�������encode��Ҫ���ڴ�ռ䳤��
		virtual DWORD SetData(CCommandRequestBase* pInData) = 0;
		virtual DWORD Encode(char* pOutData, unsigned length) = 0;
		virtual DWORD CallBack() = 0;
		virtual ~IEncodeOperation() {}
	};

	interface IDecodeOperation
	{
	public:
		//�������ݣ�������decode��Ҫ���ڴ�ռ䳤��
		virtual DWORD SetData(CCommandResponseBase* pInData) = 0;
		virtual DWORD Decode(char* pOutData, unsigned length) = 0;
		virtual DWORD CallBack() = 0;
		virtual ~IDecodeOperation() {}
	};
	//////////////////////////////////////////////////////////////////////////

	/// declaration: CHandleEncodeTemplate and CHandleDecodeTemplate...
	template <class CRequest>
	class CHandleEncodeTemplate : public IEncodeOperation,  public CDefaultCtorHelper<CRequest>
	{
	public:
		virtual DWORD SetData(CCommandRequestBase* pInData);
		virtual DWORD Encode(char* pOutData, unsigned length);
		virtual DWORD CallBack() { return 0; }
	};
	template <class CResponse>
	class CHandleDecodeTemplate : public IDecodeOperation,  public CDefaultCtorHelper<CResponse>
	{
	public:
		virtual DWORD SetData(CCommandResponseBase* pInData);
		virtual DWORD Decode(char* pOutData, unsigned length);
		virtual DWORD CallBack() { return 0; }
	};

	/// implemention: template class  CHandleEncodeTemplate and CHandleDecodeTemplate...
	template <class CRequest>
		DWORD CHandleEncodeTemplate<CRequest>::SetData(CCommandRequestBase* pInData)
	{	
		_obj_ptr = static_cast<CCommandType*>(pInData);
		return TRUE;	
	}

	template <class CRequest>
		DWORD CHandleEncodeTemplate<CRequest>::Encode( char* pOutData, unsigned length )
	{
		thunder_assert(_obj_ptr);
		char* pData = _obj_ptr->TransByteOrderAndWrite(pOutData, length);

		release_assert((unsigned)(pData-pOutData) <= length);
		return TRUE;
	}

	template <class CResponse>
		DWORD CHandleDecodeTemplate<CResponse>::SetData(CCommandResponseBase* pInData)
	{
		_obj_ptr = static_cast<CCommandType*>(pInData);

		return TRUE;
	}

	template <class CResponse>
		DWORD CHandleDecodeTemplate<CResponse>::Decode( char* pOutData, unsigned length )
	{
		thunder_assert(_obj_ptr);
		char* pData = _obj_ptr->TransByteOrderAndRead(pOutData, length);

		release_assert((unsigned)(pData-pOutData) <= length);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	/// �������ͣ����� Encode Decode ������� ///
	/// 1��	������Ӧ����
	typedef CHandleDecodeTemplate<CErrorResponse> CHandleErrorDecode;

	/// 2��	Wifi��¼��usbģʽ���ᷢ�ͣ�
	typedef CHandleEncodeTemplate<CLogWifiRequest> CHandleLogWifiEncode;
	typedef CHandleDecodeTemplate<CLogWifiResponse> CHandleLogWifiDecode;

	/// 3��	��������
	typedef CHandleEncodeTemplate<CTimeTickRequset> CHandleTimeTickEncode;
	typedef CHandleDecodeTemplate<CTimeTickResponse> CHandleTimeTickDecode;

	/// 4��	List����
	typedef CHandleEncodeTemplate<CListPartitionRequest> CHandleListPartitionEncode;
	typedef CHandleDecodeTemplate<CListPartitionResponse> CHandleListPartitionDecode;	

	/// 5��	��ȡ������Ϣ
	typedef CHandleEncodeTemplate<CGetPatitionInfoRequest> CHandleGetPartitionInfoEncode;
	typedef CHandleDecodeTemplate<CGetPatitionInfoResponse> CHandleGetPartitionInfoDecode;	

	/// 6��	ListĿ¼
	typedef CHandleEncodeTemplate<CListPathRequest> CHandleListPathEncode;
	typedef CHandleDecodeTemplate<CListPathResponse> CHandleListPathDecode;	

	/// 7��	ɾ���ļ�
	typedef CHandleEncodeTemplate<CDeleteFileRequest> CHandleDeleteFileEncode;
	typedef CHandleDecodeTemplate<CDeleteFileResponse> CHandleDeleteFileDecode;	

	/// 8��	�������ļ�
	typedef CHandleEncodeTemplate<CRenameFileRequest> CHandleRenameFileEncode;
	typedef CHandleDecodeTemplate<CRenameFileResponse> CHandleRenameFileDecode;	

	/// 9��	�鿴�ļ��Ƿ����
	typedef CHandleEncodeTemplate<CIsExistFileRequest> CHandleIsExistFileEncode;
	typedef CHandleDecodeTemplate<CIsExistFileResponse> CHandleIsExistFileDecode;	

	/// 10��	��ȡ��Ƶ����Ƶ��Ϣ
	typedef CHandleEncodeTemplate<CGetMediaInfoRequest> CHandleGetMediaInfoEncode;
	typedef CHandleDecodeTemplate<CGetMediaInfoResponse> CHandleGetMediaInfoDecode;	

	/// 11��	��Ƶ��ͼ����ȡ��Ƶ��Ϣ����ͼ�ͻ�ȡ��Ƶ��Ϣû��Ҫ�����������
	typedef CHandleEncodeTemplate<CCaptureFromVedioRequest> CHandleCaptureFromVedioEncode;
	typedef CHandleDecodeTemplate<CCaptureFromVedioResponse> CHandleCaptureFromVedioDecode;	

	/// 12��	ͼƬ��ͼ����ͼתСͼ��
	typedef CHandleEncodeTemplate<CCaptureFromeImageRequest> CHandleCaptureFromImageEncode;
	typedef CHandleDecodeTemplate<CCaptureFromImageResponse> CHandleCaptureFromImageDecode;	

	/// 13��	�����������������ļ���С������
	typedef CHandleEncodeTemplate<CFindFilesRequest> CHandleFindFileEncode;
	typedef CHandleDecodeTemplate<CFindFileResponse> CHandleFindFileDecode;	

	/// 14��	��ȡmp3�ļ���Ϣ
	typedef CHandleEncodeTemplate<CGetMP3InfoRequest> CHandleGetMP3InfoEncode;
	typedef CHandleDecodeTemplate<CGetMP3InfoResponse> CHandleGetMP3InfoDecode;	

	/// 15��	�����ļ����ֻ��������ļ����ݰ����
	typedef CHandleEncodeTemplate<CSendFileRequest> CHandleSendFileEncode;
	typedef CHandleDecodeTemplate<CSendFileResponse> CHandleSendFileDecode;	

	/// 16��	���ֻ������ļ��������ļ����ݰ����
	typedef CHandleEncodeTemplate<CRecvFileRequest> CHandleRecvFileEncode;
	typedef CHandleDecodeTemplate<CRecvFileResponse> CHandleRecvFileDecode;	

	/// 17��	�鿴�ļ���С�����������
	typedef CHandleEncodeTemplate<CGetFileSizeRequest> CHandleGetFileSizeEncode;
	typedef CHandleDecodeTemplate<CGetFileSizeResponse> CHandleGetFileSizeDecode;	
	//////////////////////////////////////////////////////////////////////////


#undef BOOL  //ж�ظ��ļ���BOOL�궨��

// 
// }// namespace test_namespace


#endif//MOBILECOMMANDS_H