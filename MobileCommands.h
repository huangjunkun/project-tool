#ifndef __MOBILE_COMMANDS_H_
#define __MOBILE_COMMANDS_H_

//#pragma once
#include "TransByteOrderStream.h"

/**
** 源文件： MobileCommands.h MobileCommands.cpp MobileCommandsTest.cpp
** 引用文件：TransByteOrderStream.h TransByteOrderStream.cpp
** 功能说明：协议编解码示例，如PC-手机通讯交互协议，以下包含多条交互命令，通信数据以字节流编解码完成。
** 详细设计参见相关代码。类似协议的制定，可参考这样的设计模式。使用到类型TransByteOrderStream的编解码处理。

** btw，由于当时项目做了大调整，该协议适用的项目被叫停，所以这些前期探索、演练的代码作废，仅作为后续开发类似协议的参考。但是，在开发设计该程序过程，毕竟花费诸多心思去设计与架构，甚至有时自我陶醉一番哈哈，客观说来代码还算良好，不妨在此开源一睹为快，希望其中存在的问题可以暴露出来。呵呵，题外话了一把。

** 作者：junkun huang
** 日期：2010-12-11 /
*/

//////////////////////////////////////////////////////////////////////////
namespace xl
{

	//////////////////////////////////////////////////////////////////////////

	typedef xl::uint8  uint8;
	typedef xl::uint16 uint16;
	typedef xl::uint32 uint32;
	typedef xl::uint64 uint64;


	const int SIZE_OF_UINT16 = sizeof(uint16);
	const int SIZE_OF_UINT32 = sizeof(uint32);
	const int SIZE_OF_UINT64 = sizeof(uint64);


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
		typedef xl::uint8   UInt8;
		typedef xl::uint16	UInt16;
		typedef xl::uint32	UInt32;
		typedef xl::uint64	UInt64;
		typedef xl::uint32	Bool;
		typedef std::string String;


		// 		template <typename T>
		// 		class Array : public std::vector<T> {};
		//#define Array std::vector

		UInt32    m_protocol_version;
		UInt32    m_sequence;
		UInt32    m_command_length;

		virtual ~CCommandBase() {}
	protected:
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	public:
		static unsigned SizeOf();
		virtual unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// mobile request commands' based class --- CCommandRequestBase
	struct CCommandRequestBase : public CCommandBase
	{
		typedef CCommandBase CBase;
		UInt32	m_command_name;
		UInt32	m_command_flag;
		UInt32	m_priority_level;

		// 		CCommandRequestBase()
		// 			: m_command_name(0), m_command_flag(0) {}
		CCommandRequestBase(UInt32 name)
			: m_command_name(name), m_command_flag(0) {}
			CCommandRequestBase(UInt32 name, UInt32 flag)
				: m_command_name(name), m_command_flag(flag) {}
	protected:
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	private:
		//覆盖基类该方法并在子类禁用
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	public:
		static unsigned SizeOf();
		virtual unsigned Length() const;
		bool SetPriority(UInt32 priority);
	};

	//////////////////////////////////////////////////////////////////////////
	/// mobile response commands' based class --- CCommandResponseBase
	struct CCommandResponseBase : public CCommandBase
	{
		typedef CCommandBase CBase;

		UInt32    m_command_name;
		UInt32    m_command_flag;
		Bool      m_result;

		// 		CCommandResponseBase()
		// 			: m_command_name(0), m_command_flag(0) {}
		CCommandResponseBase(UInt32 name)
			: m_command_name(name), m_command_flag(0) {}
			CCommandResponseBase(UInt32 name, UInt32 flag)
				: m_command_name(name), m_command_flag(flag) {}
	protected:
		char* TransByteOrderAndRead(char* pOutData, unsigned length);
	private:
		//覆盖基类该方法并在子类禁用
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);

	public:
		static unsigned SizeOf();
		virtual unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 1，	出错响应命令
	struct CErrorResponse : public CCommandResponseBase
	{
		typedef CCommandResponseBase CBase;
		UInt32 m_error_code;

		CErrorResponse()
			:CCommandResponseBase(MOBILE_RESPONSE_ERROR)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	//////////////////////////////////////////////////////////////////////////
	/// 2，	登录命令（预留给wifi）
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
	/// 3，	心跳命令
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
	/// 4，	List分区
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
		std::vector<UInt32> m_partitions;

		typedef CCommandResponseBase CBase;
		CListPartitionResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_LIST_PARTITION)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 5，	获取分区信息
	struct CGetPatitionInfoRequest : public CCommandRequestBase
	{
		typedef CCommandRequestBase CBase;
		UInt32	m_partition_index;

		CGetPatitionInfoRequest()
			: CCommandRequestBase(MOBILE_REQUEST_GET_PARTITION_INFO)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CGetPatitionInfoResponse : public CCommandResponseBase
	{
		UInt64	m_patition_total_size;
		UInt64	m_partition_remain_size;

		typedef CCommandResponseBase CBase;
		CGetPatitionInfoResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_PARTITION_INFO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 6，	List目录
	struct CFileStruct
	{
		typedef CCommandBase::String	String;
		typedef CCommandBase::Bool		Bool;
		uint32	m_struct_length;
		String	m_file_name;
		uint64	m_file_size;
		Bool	m_is_folder;
		Bool	m_is_visible;
		Bool	m_is_writable;
		String	m_file_type;
		String	m_modify_time;
		String	m_create_time;

		// 		CFileStruct() {}
		// 		CFileStruct(const CFileStruct& obj)
		// 			: m_struct_length(obj.m_struct_length), m_file_name(obj.m_file_name),
		// 			m_file_size(obj.m_file_size), m_is_folder(obj.m_is_folder),
		// 			m_is_visible(obj.m_is_visible), m_is_writable(obj.m_is_writable),
		// 			m_file_type(obj.m_file_type),
		// 			m_modify_time(obj.m_modify_time), m_create_time(obj.m_create_time)
		// 		{}

		unsigned Length() const;

		TransByteOrderIOStream& operator>> (TransByteOrderIOStream& ins);
		TransByteOrderIOStream& operator<< (TransByteOrderIOStream& outs);

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	};

	struct CListPathRequest : public CCommandRequestBase
	{
		UInt32 m_partition_index;
		String m_path_name;
		UInt32 m_file_mask;

		typedef CCommandRequestBase CBase;
		CListPathRequest()
			: CCommandRequestBase(MOBILE_REQUEST_LIST_PATH)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CListPathResponse : public CCommandResponseBase
	{
		std::vector<CFileStruct> m_file_array;

		typedef CCommandResponseBase CBase;
		CListPathResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_LIST_PATH)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 7，	删除文件
	struct CDeleteFileRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
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
	/// 8，	重命名文件
	struct CRenameFileRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
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
	/// 9，	查看文件是否存在
	struct CIsExistFileRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
		String	m_full_filename;
		Bool	m_is_file;

		typedef CCommandRequestBase CBase;
		CIsExistFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_IS_EXIST_FIEL)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CIsExistFileResponse : public CCommandResponseBase
	{
		Bool m_is_exist;

		typedef CCommandResponseBase CBase;
		CIsExistFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_IS_EXIST_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 10，获取视频和音频信息
	struct CGetMediaInfoRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
		String	m_full_filename;
		Bool	m_is_audio;

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
		UInt32	m_tatol_bitrate;
		UInt32	m_audio_frequence;
		UInt32	m_audio_bitrate;
		UInt32	m_frames_per;

		typedef CCommandResponseBase CBase;
		CGetMediaInfoResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_MEDIA_INFO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 11，视频截图并获取视频信息（截图和获取视频信息没必要连发两个命令）
	struct CCaptureFromVedioRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
		String	m_src_filename;
		String	m_picture_format;
		UInt32	m_width;
		UInt32	m_height;
		UInt32	m_begin_second;
		Bool	m_keep_ratio;
		UInt32	m_backgroud;

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
		UInt32	m_tatol_bitrate;
		UInt32	m_audio_frequence;
		UInt32	m_audio_bitrate;
		UInt32	m_frames_per;

		typedef CCommandResponseBase CBase;
		CCaptureFromVedioResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_CAPTURE_FROM_VEDIO)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 12，图片截图（大图转小图
	struct CCaptureFromeImageRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
		String	m_src_filename;
		String	m_picture_format;
		UInt32	m_width;
		UInt32	m_height;
		Bool	m_keep_ratio;
		UInt32	m_backgroud;

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
	/// 13，遍历搜索计算类型文件大小及个数
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
		UInt32	m_partition_index;
		String	m_path_name;
		Bool	m_need_recursion;
		std::vector< std::vector<String> > m_type_array;

		typedef CCommandRequestBase CBase;
		CFindFilesRequest()
			: CCommandRequestBase(MOBILE_REQUEST_FIND_FILE)
		{}

		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	struct CFindFileResponse : public CCommandResponseBase
	{
		std::vector<CTypeCalc>	m_type_collection;

		typedef CCommandResponseBase CBase;
		CFindFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_FIND_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 14，获取mp3文件信息
	struct CGetMP3InfoRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
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
	/// 15，	发送文件到手机（发送文件数据包命令）
	struct CSendFileRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
		String	m_dst_filename;
		UInt64	m_file_size;
		UInt64	m_offset;
		String	m_file_content;//string 表示文件内容不合适

		typedef CCommandRequestBase CBase;

		CSendFileRequest()
			: CCommandRequestBase(MOBILE_REQUEST_SEND_FILE)
		{}
		char* TransByteOrderAndWrite(char* pOutData, unsigned length);
		unsigned Length() const;
	};


	struct CSendFileResponse : public CCommandResponseBase
	{
		UInt64	m_success_offset;
		UInt64	m_success_length;

		typedef CCommandResponseBase CBase;
		CSendFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_SEND_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 16，	从手机接收文件（接收文件数据包命令）
	struct CRecvFileRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
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
		UInt64	m_file_size;
		UInt64	m_offset;
		String	m_file_content;//string 表示文件内容不合适
		typedef CCommandResponseBase CBase;

		CRecvFileResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_RECV_FILE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};

	//////////////////////////////////////////////////////////////////////////
	/// 17，	查看文件大小（主连接命令）
	struct CGetFileSizeRequest : public CCommandRequestBase
	{
		UInt32	m_partition_index;
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
		UInt64	m_file_size;
		typedef CCommandResponseBase CBase;

		CGetFileSizeResponse()
			: CCommandResponseBase(MOBILE_RESPONSE_GET_FILE_SIZE)
		{}

		char* TransByteOrderAndRead(char* pOutData, unsigned length);
		unsigned Length() const;
	};
	//////////////////////////////////////////////////////////////////////////
	// interface ICopyCommandObject
	// {
	// public:
	// 	virtual void* CopyCommandObject() const = 0;
	// 	virtual ~ICopyCommandObject() {}
	// };

	template <typename CommandType>
	struct CCommandWrapHelper// : public ICopyCommandObject
	{
		typedef CommandType CCommandType;

		CCommandWrapHelper()
			: _obj_ptr(0) {}
			// 	void* CopyCommandObject() const
			// 	{
			// 		if (_obj_ptr)
			// 			return (new CCommandType(*_obj_ptr));
			// 		else
			// 			return NULL;
			// 	}
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
		virtual CCommandRequestBase* CopyCommandObject() const = 0;
		//设置数据，并返回encode需要的内存空间长度
		virtual DWORD SetData(CCommandRequestBase* pInData) = 0;
		virtual DWORD Encode(char* pOutData, unsigned length) = 0;
		virtual DWORD CallBack() = 0;
		virtual ~IEncodeOperation() {}
	};

	interface IDecodeOperation
	{
	public:
		virtual CCommandResponseBase* CopyCommandObject() const = 0;
		//设置数据，并返回decode需要的内存空间长度
		virtual DWORD SetData(CCommandResponseBase* pInData) = 0;
		virtual DWORD Decode(char* pOutData, unsigned length) = 0;
		virtual DWORD CallBack() = 0;
		virtual ~IDecodeOperation() {}
	};
	//////////////////////////////////////////////////////////////////////////

	/// declaration: CHandleEncodeTemplate and CHandleDecodeTemplate...
	template <class CRequest>
	class CHandleEncodeTemplate : public IEncodeOperation,  public CCommandWrapHelper<CRequest>
	{
    public:
        using CCommandWrapHelper<CRequest>::_obj_ptr;
        typedef typename CCommandWrapHelper<CRequest>::CCommandType CCommandType;

	public:
		virtual CCommandRequestBase* CopyCommandObject() const;

		virtual DWORD SetData(CCommandRequestBase* pInData);
		virtual DWORD Encode(char* pOutData, unsigned length);
		virtual DWORD CallBack() { return 0; }
	};

	template <class CResponse>
	class CHandleDecodeTemplate : public IDecodeOperation,  public CCommandWrapHelper<CResponse>
	{
    public:
        using CCommandWrapHelper<CResponse>::_obj_ptr;
        typedef typename CCommandWrapHelper<CResponse>::CCommandType CCommandType;

	public:
		virtual CCommandResponseBase* CopyCommandObject() const;

		virtual DWORD SetData(CCommandResponseBase* pInData);
		virtual DWORD Decode(char* pOutData, unsigned length);
		virtual DWORD CallBack() { return 0; }
	};


	/// implemention: template class  CHandleEncodeTemplate and CHandleDecodeTemplate...

	template <class CRequest>
		CCommandRequestBase* CHandleEncodeTemplate<CRequest>::CopyCommandObject() const
	{
		if (_obj_ptr)
			return (new CCommandType(*_obj_ptr));
		else
			return NULL;
	}

	template <class CRequest>
		DWORD CHandleEncodeTemplate<CRequest>::SetData(CCommandRequestBase* pInData)
	{
		_obj_ptr = static_cast<CCommandType*>(pInData);
		return TRUE;
	}

	template <class CRequest>
		DWORD CHandleEncodeTemplate<CRequest>::Encode( char* pOutData, unsigned length )
	{
		assert(_obj_ptr);
		/// TransByteOrderAndWrite 之前给请求命令长度赋值
		_obj_ptr->m_command_length = _obj_ptr->Length();
		char* pData = _obj_ptr->TransByteOrderAndWrite(pOutData, length);
		assert((unsigned)(pData-pOutData) <= length);

		return TRUE;
	}

	template <class CResponse>
		CCommandResponseBase* CHandleDecodeTemplate<CResponse>::CopyCommandObject() const
	{
		if (_obj_ptr)
			return (new CCommandType(*_obj_ptr));
		else
			return NULL;
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
		assert(_obj_ptr);
		char* pData = _obj_ptr->TransByteOrderAndRead(pOutData, length);
		/// TransByteOrderAndRead 之后给响应命令长度赋值
		_obj_ptr->m_command_length = _obj_ptr->Length();

		assert((unsigned)(pData-pOutData) <= length);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	/// 以下类型：处理 Encode 和 Decode 命令对象 ///
	/// 1，	出错响应命令
	typedef CHandleDecodeTemplate<CErrorResponse> CHandleErrorDecode;

	/// 2，	Wifi登录（usb模式不会发送）
	typedef CHandleEncodeTemplate<CLogWifiRequest> CHandleLogWifiEncode;
	typedef CHandleDecodeTemplate<CLogWifiResponse> CHandleLogWifiDecode;

	/// 3，	心跳命令
	typedef CHandleEncodeTemplate<CTimeTickRequset> CHandleTimeTickEncode;
	typedef CHandleDecodeTemplate<CTimeTickResponse> CHandleTimeTickDecode;

	/// 4，	List分区
	typedef CHandleEncodeTemplate<CListPartitionRequest> CHandleListPartitionEncode;
	typedef CHandleDecodeTemplate<CListPartitionResponse> CHandleListPartitionDecode;

	/// 5，	获取分区信息
	typedef CHandleEncodeTemplate<CGetPatitionInfoRequest> CHandleGetPartitionInfoEncode;
	typedef CHandleDecodeTemplate<CGetPatitionInfoResponse> CHandleGetPartitionInfoDecode;

	/// 6，	List目录
	typedef CHandleEncodeTemplate<CListPathRequest> CHandleListPathEncode;
	typedef CHandleDecodeTemplate<CListPathResponse> CHandleListPathDecode;

	/// 7，	删除文件
	typedef CHandleEncodeTemplate<CDeleteFileRequest> CHandleDeleteFileEncode;
	typedef CHandleDecodeTemplate<CDeleteFileResponse> CHandleDeleteFileDecode;

	/// 8，	重命名文件
	typedef CHandleEncodeTemplate<CRenameFileRequest> CHandleRenameFileEncode;
	typedef CHandleDecodeTemplate<CRenameFileResponse> CHandleRenameFileDecode;

	/// 9，	查看文件是否存在
	typedef CHandleEncodeTemplate<CIsExistFileRequest> CHandleIsExistFileEncode;
	typedef CHandleDecodeTemplate<CIsExistFileResponse> CHandleIsExistFileDecode;

	/// 10，	获取视频和音频信息
	typedef CHandleEncodeTemplate<CGetMediaInfoRequest> CHandleGetMediaInfoEncode;
	typedef CHandleDecodeTemplate<CGetMediaInfoResponse> CHandleGetMediaInfoDecode;

	/// 11，	视频截图并获取视频信息（截图和获取视频信息没必要连发两个命令）
	typedef CHandleEncodeTemplate<CCaptureFromVedioRequest> CHandleCaptureFromVedioEncode;
	typedef CHandleDecodeTemplate<CCaptureFromVedioResponse> CHandleCaptureFromVedioDecode;

	/// 12，	图片截图（大图转小图）
	typedef CHandleEncodeTemplate<CCaptureFromeImageRequest> CHandleCaptureFromImageEncode;
	typedef CHandleDecodeTemplate<CCaptureFromImageResponse> CHandleCaptureFromImageDecode;

	/// 13，	遍历搜索计算类型文件大小及个数
	typedef CHandleEncodeTemplate<CFindFilesRequest> CHandleFindFileEncode;
	typedef CHandleDecodeTemplate<CFindFileResponse> CHandleFindFileDecode;

	/// 14，	获取mp3文件信息
	typedef CHandleEncodeTemplate<CGetMP3InfoRequest> CHandleGetMP3InfoEncode;
	typedef CHandleDecodeTemplate<CGetMP3InfoResponse> CHandleGetMP3InfoDecode;

	/// 15，	发送文件到手机（发送文件数据包命令）
	typedef CHandleEncodeTemplate<CSendFileRequest> CHandleSendFileEncode;
	typedef CHandleDecodeTemplate<CSendFileResponse> CHandleSendFileDecode;

	/// 16，	从手机接收文件（接收文件数据包命令）
	typedef CHandleEncodeTemplate<CRecvFileRequest> CHandleRecvFileEncode;
	typedef CHandleDecodeTemplate<CRecvFileResponse> CHandleRecvFileDecode;

	/// 17，	查看文件大小（主连接命令）
	typedef CHandleEncodeTemplate<CGetFileSizeRequest> CHandleGetFileSizeEncode;
	typedef CHandleDecodeTemplate<CGetFileSizeResponse> CHandleGetFileSizeDecode;
	//////////////////////////////////////////////////////////////////////////



	//
}// namespace xl


#endif//__MOBILE_COMMANDS_H_
