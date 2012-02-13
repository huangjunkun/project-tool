#include "stdafx.h"
#include "./CloudTransCommands.h"
#include "TransByteOrderStream.h"


CCloudTransCommandBase::CCloudTransCommandBase()
{
	m_magic_number = 0x68C37B1E;
	m_protocol_ver = 10;
}

CCloudTransCommandBase::CCloudTransCommandBase(const String& cmd_name)
	: m_cmd_name(cmd_name)
{
	m_magic_number = 0x68C37B1E;
	m_protocol_ver = 10;
}

char* CCloudTransCommandBase::TransByteOrderAndWrite(char* pOutData, unsigned length)
{
	TransByteOrderIOStream outs(pOutData, length);
	outs << m_magic_number;
	outs << m_protocol_ver;
	outs << m_body_len;
	outs << m_cmd_name;

	return outs.get_ptr();//返回下次开始写的偏移地址
}

char* CCloudTransCommandBase::TransByteOrderAndRead(char* pOutData, unsigned length)
{
	TransByteOrderIOStream ins(pOutData, length);
	ins >> m_magic_number;
	ins >> m_protocol_ver;
	ins >> m_body_len;
	ins >> m_cmd_name;

	return ins.get_ptr();//返回下次开始读的偏移地址
}

unsigned CCloudTransCommandBase::SizeOf()
{
	return sizeof(UInt32) + \
		sizeof(UInt16) + \
		sizeof(UInt32);
}

unsigned CCloudTransCommandBase::Length() const
{
	return sizeof(m_magic_number) + \
		sizeof(m_protocol_ver) + \
		sizeof(m_body_len) + \
		sizeof(m_cmd_name.size()) + m_cmd_name.size();
}

CCloudTransRequestBase::CCloudTransRequestBase(const String& cmd_name)
	: CCloudTransCommandBase(cmd_name)
{

}

char* CCloudTransRequestBase::TransByteOrderAndWrite(char* pOutData, unsigned length)
{
	char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
	ATLASSERT(pDataPos - pOutData <= length);
	return pDataPos;
}

char* CCloudTransRequestBase::TransByteOrderAndRead(char* pOutData, unsigned length)
{/// 覆盖基类该方法并在子类禁用
	return NULL;
}

unsigned CCloudTransRequestBase::Length() const
{
	return CBase::Length();
}

CCloudTransResponseBase::CCloudTransResponseBase(const String& cmd_name)
	: CCloudTransCommandBase(cmd_name)
{

}

char* CCloudTransResponseBase::TransByteOrderAndRead(char* pOutData, unsigned length)
{
	char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
	TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
	ins >> m_result;
	return ins.get_ptr();
}

char* CCloudTransResponseBase::TransByteOrderAndWrite(char* pOutData, unsigned length)
{/// 覆盖基类该方法并在子类禁用
	return NULL;
}

unsigned CCloudTransResponseBase::Length() const
{
	return CBase::Length() + \
		sizeof(m_result);
}

unsigned CTransSubInfoFromClient::Length() const
{//sizeof(Length()) + 
	return sizeof(m_start_time) + \
		sizeof(m_duration_time) + \
		sizeof(m_client_state) + \
		sizeof(m_client_finished) + \
		sizeof(m_client_revise);
}

TransByteOrderIOStream& CTransSubInfoFromClient::operator << (TransByteOrderIOStream& ins)
{
	UInt32 length;
	ins >> length;

	ins >> m_start_time;
	ins >> m_duration_time;
	ins >> m_client_state;
	ins >> m_client_finished;
	ins >> m_client_revise;
	ATLASSERT(length == Length());

	return ins;
}

TransByteOrderIOStream& CTransSubInfoFromClient::operator >> (TransByteOrderIOStream& outs)
{
	outs << (UInt32) Length();
	outs << m_start_time;
	outs << m_duration_time;
	outs << m_client_state;
	outs << m_client_finished;
	outs << m_client_revise;

	return outs;
}
/// for test ...
TransByteOrderIOStream& operator >> (TransByteOrderIOStream& ins, CTransSubInfoFromClient& obj)
{
	CCloudTransCommandBase::UInt32 length;
	ins >> length;
	ins >> obj.m_start_time;
	ins >> obj.m_duration_time;
	ins >> obj.m_client_state;
	ins >> obj.m_client_finished;
	ins >> obj.m_client_revise;
	ATLASSERT(length == obj.Length());

	return ins;
}

TransByteOrderIOStream& operator << (TransByteOrderIOStream& outs, const CTransSubInfoFromClient& obj)
{
	outs << (CCloudTransCommandBase::UInt32) obj.Length();
	outs << obj.m_start_time;
	outs << obj.m_duration_time;
	outs << obj.m_client_state;
	outs << obj.m_client_finished;
	outs << obj.m_client_revise;

	return outs;
}

CClientRequest::CClientRequest()
	: CCloudTransRequestBase("ClientRequest")
{
	m_file_size = 0;
	m_duration = 0;
	m_operation_type = 0;
	m_trans_height = 0;
	m_trans_width = 0;
	m_trans_quality = 0;
	m_tell_me_trans_info = 0;
	m_trans_sub_info_ver = 0;
	m_trans_tool_ver = 1;
	m_trans_args_ver = 1;
}

char* CClientRequest::TransByteOrderAndWrite( char* pOutData, unsigned length )
{
	m_body_len = Length() - CCloudTransCommandBase::SizeOf();
	char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
	TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
	outs << m_thunder_ver;
	outs << m_user_id;
	outs << m_cid;
	outs << m_gcid;
	outs << m_file_size;
	outs << m_duration;
	outs << m_operation_type;
	outs << m_trans_height;
	outs << m_trans_width;
	outs << m_trans_quality;
	outs << m_trans_container_name;
	outs << m_tell_me_trans_info;
	outs << m_trans_tool_ver;
	outs << m_trans_args_ver;
	outs << m_trans_sub_info_ver;

	/// way 1.// 
	outs << m_trans_sub_info_from_client;
	// 	/// way 2.or 
//	outs << (UInt32)m_trans_sub_info_from_client.size();
// 	for (size_t i=0; i < m_trans_sub_info_from_client.size(); i++)
// 		m_trans_sub_info_from_client[i].operator >> (outs);

	return outs.get_ptr();
}

unsigned CClientRequest::Length() const
{
	unsigned length =  CBase::Length() + \
		sizeof(m_thunder_ver.size()) + m_thunder_ver.size() + \
		sizeof(m_user_id.size()) + m_user_id.size() + \
		sizeof(m_cid.size()) + m_cid.size() + \
		sizeof(m_gcid.size()) + m_gcid.size() + \
		sizeof(m_file_size) + \
		sizeof(m_duration) + \
		sizeof(m_operation_type) + \
		sizeof(m_trans_height) + \
		sizeof(m_trans_width) + \
		sizeof(m_trans_quality) + \
		sizeof(m_trans_container_name.size()) + m_trans_container_name.size() + \
		sizeof(m_tell_me_trans_info) + \
		sizeof(m_trans_tool_ver) + \
		sizeof(m_trans_args_ver) + \
		sizeof(m_trans_sub_info_ver) + \
		sizeof(m_trans_sub_info_from_client.size());

	for (size_t i=0; i < m_trans_sub_info_from_client.size(); i++)
	{
		length += sizeof(m_trans_sub_info_from_client[i].Length());
		length += m_trans_sub_info_from_client[i].Length();
	}

	return length;
}

unsigned CTransSubInfoToClient::Length() const
{
	return sizeof(m_start_time) + \
		sizeof(m_duration_time) + \
		sizeof(m_client_suggestion) + \
		sizeof(m_server_state) + \
		sizeof(m_server_finished) + \
		sizeof(m_url);

}

TransByteOrderIOStream& CTransSubInfoToClient::operator << (TransByteOrderIOStream& ins)
{
	UInt32 length;
	ins >> length;	

	ins >> m_start_time;
	ins >> m_duration_time;
	ins >> m_client_suggestion;
	ins >> m_server_state;
	ins >> m_server_finished;
	ins >> m_url;
	ATLASSERT(length == Length());

	return ins;
}

TransByteOrderIOStream& CTransSubInfoToClient::operator >> (TransByteOrderIOStream& outs)
{
	outs << (UInt32) Length();
	outs << m_start_time;
	outs << m_duration_time;
	outs << m_client_suggestion;
	outs << m_server_state;
	outs << m_server_finished;
	outs << m_url;

	return outs;
}

CClientResponse::CClientResponse()
	: CCloudTransResponseBase("ClientRequestResp")
{

}

char* CClientResponse::TransByteOrderAndRead( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
	TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
	ins >> m_trans_state;
	ins >> m_gcid;
	ins >> m_trans_spec;
	ins >> m_trans_args;
	ins >> m_trans_cid;
	ins >> m_trans_gcid;
	ins >> m_trans_file_size;
	ins >> m_trans_url;
	ins >> m_trans_tool_ver;
	ins >> m_trans_args_ver;
	ins >> m_trans_sub_info_ver;

	UInt32 size;
	ins >> size;
	for (size_t i=0; i < size; i++)
	{
		CTransSubInfoToClient temp;
		temp << ins;/// or: temp.operator <<(ins);
		m_trans_sub_info_to_client.push_back(temp);
	}

	return ins.get_ptr();
}

unsigned CClientResponse::Length() const
{
	unsigned length = CBase::Length() + \
		sizeof(m_trans_state) + \
		sizeof(m_gcid.size()) + m_gcid.size() + \
		sizeof(m_trans_spec) + \
		sizeof(m_trans_args.size()) + m_trans_args.size() + \
		sizeof(m_trans_cid.size()) + m_trans_cid.size() + \
		sizeof(m_trans_gcid.size()) + m_trans_gcid.size() + \
		sizeof(m_trans_file_size) + \
		sizeof(m_trans_url.size()) + m_trans_url.size() + \
		sizeof(m_trans_tool_ver) + \
		sizeof(m_trans_args_ver) + \
		sizeof(m_trans_sub_info_ver) + \
		sizeof(m_trans_sub_info_to_client.size());

	for (size_t i=0; i < m_trans_sub_info_to_client.size(); i++)
	{
		length += sizeof(m_trans_sub_info_to_client[i].Length());
		length += m_trans_sub_info_to_client[i].Length();
	}
	return length;

}

CSchedulerPingRequest::CSchedulerPingRequest()
	: CCloudTransRequestBase("SchedulerPing")
{

}

char* CSchedulerPingRequest::TransByteOrderAndWrite( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
	TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
	outs << m_service_port;
	outs << m_capability;
	outs << m_tasks;
	outs << m_section;

	return outs.get_ptr();
}

unsigned CSchedulerPingRequest::Length() const
{
	return CBase::Length() + \
		sizeof(m_service_port) + \
		sizeof(m_capability) + \
		sizeof(m_tasks) + \
		sizeof(m_section.size()) + m_section.size();

}

CSchedulerPingResponse::CSchedulerPingResponse()
	: CCloudTransResponseBase("SchedulerPingResp")
{

}

char* CSchedulerPingResponse::TransByteOrderAndRead( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);

	ATLASSERT(pDataPos - pOutData <= length);
	return pDataPos;
}

unsigned CSchedulerPingResponse::Length() const
{
	return CBase::Length();
}

unsigned CTransInfoToServer::Length() const
{
	return sizeof(m_start_time) + \
		sizeof(m_duration_time) + \
		sizeof(m_server_suggestion);
}

TransByteOrderIOStream& CTransInfoToServer::operator << (TransByteOrderIOStream& ins)
{
	UInt32 length;
	ins >> length;	

	ins >> m_start_time;
	ins >> m_duration_time;
	ins >> m_server_suggestion;
	ATLASSERT(length == Length());

	return ins;
}

TransByteOrderIOStream& CTransInfoToServer::operator >> (TransByteOrderIOStream& outs)
{
	outs << (UInt32) Length();
	outs << m_start_time;
	outs << m_duration_time;
	outs << m_server_suggestion;

	return outs;
}

SubmitCloudTransRequest::SubmitCloudTransRequest()
	: CCloudTransRequestBase("SubmitCloudTransRequest")
{

}

char* SubmitCloudTransRequest::TransByteOrderAndWrite( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
	TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
	outs << m_cid;
	outs << m_gcid;
	outs << m_file_size;
	outs << m_duration;
	outs << m_operation_type;
	outs << m_trans_spec;
	outs << m_trans_container_name;
	outs << m_trans_args;
	outs << (UInt32)m_tans_sub_info_to_server.size();
	for (size_t i=0; i < m_tans_sub_info_to_server.size(); i++)
		m_tans_sub_info_to_server[i] >> outs;

	return outs.get_ptr();
}

unsigned SubmitCloudTransRequest::Length() const
{
	unsigned length = CBase::Length() + \
		sizeof(m_cid) + \
		sizeof(m_gcid) + \
		sizeof(m_file_size) + \
		sizeof(m_duration) + \
		sizeof(m_operation_type) + \
		sizeof(m_trans_spec) + \
		sizeof(m_trans_container_name.size()) + m_trans_container_name.size() + \
		sizeof(m_trans_args.size()) + m_trans_args.size() + \
		sizeof(m_tans_sub_info_to_server.size());
	for (size_t i=0 ;i < m_tans_sub_info_to_server.size(); i++)
	{
		length += sizeof(m_tans_sub_info_to_server[i].Length());
		length += m_tans_sub_info_to_server[i].Length();
	}

	return	length;
}

unsigned CTransSubInfoFromServer::Length() const
{
	return sizeof(m_start_time) + \
		sizeof(m_duration_time) + \
		sizeof(m_server_state) + \
		sizeof(m_server_finished) + \
		sizeof(m_trans_url.size()) + m_trans_url.size();
}

TransByteOrderIOStream& CTransSubInfoFromServer::operator << (TransByteOrderIOStream& ins)
{
	UInt32 length;
	ins >> length;
	ins >> m_start_time;
	ins >> m_duration_time;
	ins >> m_server_state;
	ins >> m_server_finished;
	ins >> m_trans_url;
	ATLASSERT(length == Length());

	return ins;
}

TransByteOrderIOStream& CTransSubInfoFromServer::operator >> (TransByteOrderIOStream& outs)
{
	outs << (UInt32) Length();
	outs << m_start_time;
	outs << m_duration_time;
	outs << m_server_state;
	outs << m_server_finished;
	outs << m_trans_url;

	return outs;
}

SubmitCloudTransResponse::SubmitCloudTransResponse()
	: CCloudTransResponseBase("SubmitCloudTransRequestResp")
{

}

char* SubmitCloudTransResponse::TransByteOrderAndRead( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
	TransByteOrderIOStream ins(pDataPos, length-(pDataPos-pOutData));
	ins >> m_trans_state;
	ins >> m_trans_cid;
	ins >> m_trans_gcid;
	ins >> m_trans_file_size;
	ins >> m_trans_url;
	UInt32 size;
	ins >> size;
	for (size_t i=0; i < size; i++)
	{
		CTransSubInfoFromServer temp;
		temp << ins;
		m_trans_sub_info_from_server.push_back(temp);
	}

	return ins.get_ptr();
}

unsigned SubmitCloudTransResponse::Length() const
{
	unsigned length = CBase::Length() + \
		sizeof(m_trans_state) + \
		sizeof(m_trans_cid) + \
		sizeof(m_trans_gcid) + \
		sizeof(m_trans_file_size) + \
		sizeof(m_trans_url.size()) + m_trans_url.size() + \
		sizeof(m_trans_sub_info_from_server.size());

	for (size_t i=0; i < m_trans_sub_info_from_server.size(); i++)
	{
		length += sizeof(m_trans_sub_info_from_server[i].Length());
		length += m_trans_sub_info_from_server[i].Length();
	}

	return length;
}

unsigned CTransSubInfoFinished::Length() const
{
	return sizeof(m_start_time) + \
		sizeof(m_duration_time) + \
		sizeof(m_server_state) + \
		sizeof(m_server_finished) + \
		sizeof(m_trans_url.size()) + m_trans_url.size();
}

TransByteOrderIOStream& CTransSubInfoFinished::operator << (TransByteOrderIOStream& ins)
{
	UInt32 length;
	ins >> length;

	ins >> m_start_time;
	ins >> m_duration_time;
	ins >> m_server_state;
	ins >> m_server_finished;
	ins >> m_trans_url;
	ATLASSERT(length == Length());

	return ins;
}

TransByteOrderIOStream& CTransSubInfoFinished::operator >> (TransByteOrderIOStream& outs)
{
	outs << (UInt32) Length();
	outs << m_start_time;
	outs << m_duration_time;
	outs << m_server_state;
	outs << m_server_finished;
	outs << m_trans_url;

	return outs;
}

CShedulerReportRequest::CShedulerReportRequest()
	: CCloudTransRequestBase("SchedulerReport")
{

}

char* CShedulerReportRequest::TransByteOrderAndWrite( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndWrite(pOutData, length);
	TransByteOrderIOStream outs(pDataPos, length-(pDataPos-pOutData));
	outs << m_cid;
	outs << m_gcid;
	outs << m_file_size;
	outs << m_duration;
	outs << m_trans_state;
	outs << m_trans_cid;
	outs << m_trans_gcid;
	outs << m_trans_file_size;
	outs << m_trans_url;
	outs << (UInt32)m_trans_sub_info_finished.size();
	for (size_t i=0; i < m_trans_sub_info_finished.size(); i++)
	{
		m_trans_sub_info_finished[i] >> outs;
	}

	return outs.get_ptr();
}

unsigned CShedulerReportRequest::Length() const
{
	unsigned length = CBase::Length() + \
		sizeof(m_cid.size()) + m_cid.size() + \
		sizeof(m_gcid.size()) + m_gcid.size() + \
		sizeof(m_file_size) + \
		sizeof(m_duration) + \
		sizeof(m_trans_state) + \
		sizeof(m_trans_cid.size()) + m_trans_cid.size() + \
		sizeof(m_trans_gcid.size()) + m_trans_gcid.size() + \
		sizeof(m_trans_file_size) + \
		sizeof(m_trans_url.size()) + m_trans_url.size() + \
		sizeof(m_trans_sub_info_finished.size());
	for (size_t i=0; i < m_trans_sub_info_finished.size(); i++)
	{
		length += sizeof(m_trans_sub_info_finished[i].Length());
		length += m_trans_sub_info_finished[i].Length();
	}

	return length;

}

CShedulerReportResponse::CShedulerReportResponse()
	: CCloudTransResponseBase("SchedulerReportResp")
{

}

char* CShedulerReportResponse::TransByteOrderAndRead( char* pOutData, unsigned length )
{
	char* pDataPos = CBase::TransByteOrderAndRead(pOutData, length);
	// ...
	return pDataPos;
}

unsigned CShedulerReportResponse::Length() const
{
	return CBase::Length();
}

