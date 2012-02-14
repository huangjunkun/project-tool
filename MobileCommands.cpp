#include "stdafx.h"
#include "./TransByteOrderStream.h"
#include "MobileCommands.h"


// namespace test_namespace
// {
//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// mobile commands' based class --- CCommandBase
	char* CCommandBase::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		TransByteOrderIOStream outs(pOutData, length);
		outs << m_protocol_version;
		outs << m_sequence;
		outs << m_command_length;
		return outs.get_ptr();//返回下次开始写的偏移地址
	}

	char* CCommandBase::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		TransByteOrderIOStream ins(pOutData, length);
		ins >> m_protocol_version;
		ins >> m_sequence;
		ins >> m_command_length;
		return ins.get_ptr();//返回下次开始读的偏移地址
	}

	unsigned CCommandBase::Length() const 
	{
		return 0;
		// 			return sizeof(m_protocol_version) + \
		// 				sizeof(m_sequence) + \
		// 				sizeof(m_command_length);
	}

	//////////////////////////////////////////////////////////////////////////
	/// mobile request commands' based class --- CCommandRequestBase

	char* CCommandRequestBase::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_command_name;
		outs << m_command_flag;
		outs << m_priority_level;

		return outs.get_ptr();
	}

	unsigned CCommandRequestBase::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_command_name) + \
			sizeof(m_command_flag) + \
			sizeof(m_priority_level);
	}

	bool CCommandRequestBase::SetPriority(uint32 priority) 
	{
		if (priority > 9)
			return false;
		m_priority_level = priority;
		return true;
	}

	char* CCommandRequestBase::TransByteOrderAndRead( char* pOutData, unsigned length )
	{
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	/// mobile response commands' based class --- CCommandResponseBase

	char* CCommandResponseBase::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_command_name;
		ins >> m_command_flag;
		ins >> m_result;

		return ins.get_ptr();
	}

	unsigned CCommandResponseBase::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_command_name) + \
			sizeof(m_command_flag) + \
			sizeof(m_result);
	}

	char* CCommandResponseBase::TransByteOrderAndWrite( char* pOutData, unsigned length )
	{
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 1，	出错响应命令

	char* CErrorResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_error_code;
		return ins.get_ptr();
	}

	unsigned CErrorResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_error_code);
	}
	//////////////////////////////////////////////////////////////////////////
	/// 2，	登录命令（预留给wifi）
	char* CLogWifiRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_password;
		return ins.get_ptr();
	}

	unsigned CLogWifiRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_password.size()) + m_password.size();
	}
	char* CLogWifiResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_phone_id;
		return ins.get_ptr();
	}

	unsigned CLogWifiResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_phone_id.size()) + m_phone_id.size();
	}
	//////////////////////////////////////////////////////////////////////////
	/// 3，	心跳命令

	char* CTimeTickRequset::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		release_assert(pDataPos - pOutData <= length);
		return pDataPos;
	}

	unsigned CTimeTickRequset::Length() const 
	{
		return CBase::Length();
	}

	char* CTimeTickResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		release_assert(pDataPos - pOutData <= length);
		return pDataPos;
	}

	unsigned CTimeTickResponse::Length() const 
	{
		return CBase::Length();
	}

	//////////////////////////////////////////////////////////////////////////
	/// 4，	List分区
	char* CListPartitionRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		release_assert(pDataPos - pOutData <= length);
		return pDataPos;
	}
	unsigned CListPartitionRequest::Length() const 
	{
		return CBase::Length();
	}

	char* CListPartitionResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		//			//or ....
		// 			unsigned size;
		// 			ins >> size;
		// 			for (size_t i=0; i < size; i++)
		// 			{
		// 				uint32 tmp;
		// 				ins >> tmp;
		// 				m_partitions.push_back(tmp);
		// 			}
		ins >> m_partitions;
		return ins.get_ptr();
	}

	unsigned CListPartitionResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partitions.size()) + sizeof(uint32) * m_partitions.size();
	}
	//////////////////////////////////////////////////////////////////////////
	/// 5，	获取分区信息

	char* CGetPatitionInfoRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		return outs.get_ptr();
	}

	unsigned CGetPatitionInfoRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index);
	}

	char* CGetPatitionInfoResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_patition_total_size;
		ins >> m_partition_remain_size;
		return ins.get_ptr();
	}

	unsigned CGetPatitionInfoResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_remain_size) + \
			sizeof(m_patition_total_size);
	}

	//////////////////////////////////////////////////////////////////////////
	/// 6，	List目录
	unsigned CFileStruct::Length() const 
	{
		return sizeof(m_struct_length) + \
			sizeof(m_file_name.size()) + m_file_name.size() + \
			sizeof(m_file_size) + \
			sizeof(m_is_folder) + \
			sizeof(m_is_visible) + \
			sizeof(m_is_writable) + \
			sizeof(m_file_type.size()) + m_file_type.size() + \
			sizeof(m_modify_time.size()) + m_modify_time.size() + \
			sizeof(m_create_time.size()) + m_create_time.size();
	}

	TransByteOrderIOStream& CFileStruct::operator>> (TransByteOrderIOStream& ins)
	{
		ins >> m_struct_length;
		ins >> m_file_name;
		ins >> m_file_size;
		ins >> m_is_folder;
		ins >> m_is_visible;
		ins >> m_is_writable;
		ins >> m_file_type;
		ins >> m_modify_time;
		ins >> m_create_time;
		return ins;
	}
	TransByteOrderIOStream& CFileStruct::operator<< (TransByteOrderIOStream& outs)
	{
		outs << m_struct_length;
		outs << m_file_name;
		outs << m_file_size;
		outs << m_is_folder;
		outs << m_is_visible;
		outs << m_is_writable;
		outs << m_file_type;
		outs << m_modify_time;
		outs << m_create_time;
		return outs;
	}

	char* CFileStruct::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		TransByteOrderIOStream ins(pOutData, length);
		ins >> m_struct_length;
		ins >> m_file_name;
		ins >> m_file_size;
		ins >> m_is_folder;
		ins >> m_is_visible;
		ins >> m_is_writable;
		ins >> m_file_type;
		ins >> m_modify_time;
		ins >> m_create_time;

		return ins.get_ptr();
	}

	char* CFileStruct::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		TransByteOrderIOStream outs(pOutData, length);
		outs << m_struct_length;
		outs << m_file_name;
		outs << m_file_size;
		outs << m_is_folder;
		outs << m_is_visible;
		outs << m_is_writable;
		outs << m_file_type;
		outs << m_modify_time;
		outs << m_create_time;

		return outs.get_ptr();
	}

	char* CListPathRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_path_name;
		outs << m_file_mask;
		return outs.get_ptr();
	}

	unsigned CListPathRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_path_name.size()) + m_path_name.size() + \
			sizeof(m_file_mask);
	}

	char* CListPathResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		uint32 size;
		ins >> size;
		// 			pDataPos = ins.get_ptr();//plan 2
		//  		unsigned buffer_len = ins.get_len();//plan 2
		for (size_t i=0; i < size; i++)
		{
			CFileStruct filestruct;
			//ins >> filestruct;//plan 1  这里不能编译通过，因为编译器仅针对ins成员函数解析 ？
			filestruct.operator >>(ins);//plan 1
			//				//or...
			// 				pDataPos = filestruct.TransByteOrderAndRead(pDataPos, buffer_len)//plan 2
			// 				unsigned buffer_len = pDataPos - ins.get_ptr();//plan 2

			m_file_structs.push_back(filestruct);
		}
		return ins.get_ptr();
		//			return pDataPos;//plan 2
	}

	unsigned CListPathResponse::Length() const 
	{
		unsigned length = CBase::Length();
		length += sizeof(m_file_structs.size());

		for( size_t i=0; i < m_file_structs.size(); i++)
			length += m_file_structs[i].Length();

		return length;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 7，	删除文件
	char* CDeleteFileRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_full_filename;

		return outs.get_ptr();
	}

	unsigned CDeleteFileRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_full_filename.size()) + m_full_filename.size();
	}
	char* CDeleteFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		return pDataPos;
	}

	unsigned CDeleteFileResponse::Length() const 
	{
		return CBase::Length();
	}

	//////////////////////////////////////////////////////////////////////////
	/// 8，	重命名文件
	char* CRenameFileRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_old_filename;
		outs << m_new_filename;

		return outs.get_ptr();
	}

	unsigned CRenameFileRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_old_filename.size()) + m_old_filename.size() + \
			sizeof(m_new_filename.size()) + m_new_filename.size();
	}
	char* CRenameFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		return pDataPos;
	}

	unsigned CRenameFileResponse::Length() const 
	{
		return CBase::Length();
	}

	//////////////////////////////////////////////////////////////////////////
	/// 9，	查看文件是否存在
	char* CIsExistFileRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_full_filename;
		outs << m_is_file;

		return outs.get_ptr();
	}

	unsigned CIsExistFileRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_full_filename.size()) + m_full_filename.size() + \
			sizeof(m_is_file);
	}
	char* CIsExistFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_is_exist;

		return ins.get_ptr();
	}

	unsigned CIsExistFileResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_is_exist);
	}
	//////////////////////////////////////////////////////////////////////////
	/// 10，获取视频和音频信息
	char* CGetMediaInfoRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_full_filename;
		outs << m_is_audio;

		return outs.get_ptr();
	}

	unsigned CGetMediaInfoRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_full_filename.size()) + m_full_filename.size() + \
			sizeof(m_is_audio);
	}
	char* CGetMediaInfoResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_duration;
		ins >> m_audio_format;
		ins >> m_vedio_format;
		ins >> m_vedio_width_height;
		ins >> m_tatol_bitrate;
		ins >> m_audio_frequence;
		ins >> m_audio_bitrate;
		ins >> m_frames_per;

		return ins.get_ptr();
	}

	unsigned CGetMediaInfoResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_duration.size()) + m_duration.size() + \
			sizeof(m_audio_format.size()) + m_audio_format.size() + \
			sizeof(m_vedio_format.size()) + m_vedio_format.size() + \
			sizeof(m_vedio_width_height.size()) + m_vedio_width_height.size() + \
			sizeof(m_tatol_bitrate) + \
			sizeof(m_audio_frequence) + \
			sizeof(m_audio_bitrate) + \
			sizeof(m_frames_per);
	}
	//////////////////////////////////////////////////////////////////////////
	/// 11，视频截图并获取视频信息（截图和获取视频信息没必要连发两个命令）
	char* CCaptureFromVedioRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_src_filename;
		outs << m_picture_format;
		outs << m_width;
		outs << m_height;
		outs << m_begin_second;
		outs << m_keep_ratio;
		outs << m_backgroud;

		return outs.get_ptr();
	}

	unsigned CCaptureFromVedioRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_picture_format) + \
			sizeof(m_src_filename.size()) + m_src_filename.size() + \
			sizeof(m_picture_format.size()) + m_picture_format.size() + \
			sizeof(m_width) + \
			sizeof(m_height) + \
			sizeof(m_begin_second) + \
			sizeof(m_keep_ratio) + \
			sizeof(m_backgroud);
	}

	char* CCaptureFromVedioResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_dst_filename;
		ins >> m_duration;
		ins >> m_audio_format;
		ins >> m_vedio_format;
		ins >> m_vedio_width_height;
		ins >> m_tatol_bitrate;
		ins >> m_audio_frequence;
		ins >> m_audio_bitrate;
		ins >> m_frames_per;

		return ins.get_ptr();
	}

	unsigned CCaptureFromVedioResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_dst_filename.size()) + m_dst_filename.size() + \
			sizeof(m_duration.size()) + m_duration.size() + \
			sizeof(m_audio_format.size()) + m_audio_format.size() + \
			sizeof(m_vedio_format.size()) + m_vedio_format.size() + \
			sizeof(m_vedio_width_height.size()) + m_vedio_width_height.size() + \
			sizeof(m_tatol_bitrate) + \
			sizeof(m_audio_frequence) + \
			sizeof(m_audio_bitrate) + \
			sizeof(m_frames_per);

	}
	//////////////////////////////////////////////////////////////////////////
	/// 12，图片截图（大图转小图
	char* CCaptureFromeImageRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_src_filename;
		outs << m_picture_format;
		outs << m_width;
		outs << m_height;
		outs << m_keep_ratio;
		outs << m_backgroud;

		return outs.get_ptr();
	}

	unsigned CCaptureFromeImageRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_picture_format) + \
			sizeof(m_src_filename.size()) + m_src_filename.size() + \
			sizeof(m_picture_format.size()) + m_picture_format.size() + \
			sizeof(m_width) + \
			sizeof(m_height) + \
			sizeof(m_keep_ratio) + \
			sizeof(m_backgroud);
	}

	char* CCaptureFromImageResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_dst_filename;

		return ins.get_ptr();
	}

	unsigned CCaptureFromImageResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_dst_filename);
	}
	//////////////////////////////////////////////////////////////////////////
	/// 13，遍历搜索计算类型文件大小及个数
	char* CFindFilesRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_path_name;
		outs << m_need_recursion;
		outs << m_type_array;//这里注意

		return outs.get_ptr();
	}

	unsigned CFindFilesRequest::Length() const 
	{
		unsigned length = CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_path_name.size()) + m_path_name.size() + \
			sizeof(m_need_recursion) + \
			sizeof(m_type_array.size());

		for (size_t i=0; i < m_type_array.size(); i++)
		{
			length += sizeof(m_type_array[i].size());
			for (size_t j=0; j < m_type_array[i].size(); j++)
			{
				length += (sizeof(m_type_array[i][j].size()) + m_type_array[i][j].size());
			}
		}

		return length;
	}
	char* CFindFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		uint32 size = 0;
		ins >> size;
		for (size_t i=0; i < size; i++)
		{
			CTypeCalc tmp;
			tmp.operator >>(ins);
			m_type_collection.push_back(tmp);
		}

		return ins.get_ptr();
	}

	unsigned CFindFileResponse::Length() const 
	{
		unsigned length = CBase::Length() + sizeof(m_type_collection.size());
		for (size_t i=0; i < m_type_collection.size(); i++)
		{
			length += m_type_collection[i].Length();
		}
		return length;

	}
	//////////////////////////////////////////////////////////////////////////
	/// 14，获取mp3文件信息
	char* CGetMP3InfoRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_full_filename;

		return outs.get_ptr();
	}

	unsigned CGetMP3InfoRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_full_filename.size()) + m_full_filename.size();
	}
	char* CGetMP3InfoResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_duration;
		ins >> m_artist;
		ins >> m_album;

		return ins.get_ptr();
	}

	unsigned CGetMP3InfoResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_duration.size()) + m_duration.size() + \
			sizeof(m_artist.size()) + m_artist.size() + \
			sizeof(m_album.size()) + m_album.size();
	}
	//////////////////////////////////////////////////////////////////////////
	/// 15，	发送文件到手机（发送文件数据包命令）
	char* CSendFileRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_dst_filename;
		outs << m_file_size;
		outs << m_offset;
		outs << m_file_content;//...

		return outs.get_ptr();
	}

	unsigned CSendFileRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_dst_filename.size()) + m_dst_filename.size() + \
			sizeof(m_file_size) + \
			sizeof(m_offset) + \
			sizeof(m_file_content.size()) + m_file_content.size();

	}
	char* CSendFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		return pDataPos;
	}

	unsigned CSendFileResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_success_offset) + \
			sizeof(m_success_length);

	}
	//////////////////////////////////////////////////////////////////////////
	/// 16，	从手机接收文件（接收文件数据包命令）
	char* CRecvFileRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_src_filename;

		return outs.get_ptr();
	}

	unsigned CRecvFileRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_src_filename.size()) + m_src_filename.size();

	}

	char* CRecvFileResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_file_size;
		ins >> m_offset;
		ins >> m_file_content;// ...
		return ins.get_ptr();
	}
	unsigned CRecvFileResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_file_size) + \
			sizeof(m_offset) + \
			sizeof(m_file_content.size()) + m_file_content.size();
	}
	//////////////////////////////////////////////////////////////////////////
	/// 17，	查看文件大小（主连接命令）
	char* CGetFileSizeRequest::TransByteOrderAndWrite(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
		TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
		outs << m_partition_index;
		outs << m_full_filename;

		return outs.get_ptr();
	}

	unsigned CGetFileSizeRequest::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_partition_index) + \
			sizeof(m_full_filename.size()) + m_full_filename.size();

	}
	char* CGetFileSizeResponse::TransByteOrderAndRead(char* pOutData, unsigned length)
	{
		char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
		TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
		ins >> m_file_size;
		return ins.get_ptr();
	}
	unsigned CGetFileSizeResponse::Length() const 
	{
		return CBase::Length() + \
			sizeof(m_file_size);
	}

//////////////////////////////////////////////////////////////////////////
//}//namespace test_namespace