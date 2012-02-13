#ifndef _CLOUD_TRANS_COMMANDS_H
#define _CLOUD_TRANS_COMMANDS_H

#include <vector>

class TransByteOrderIOStream;

/// ��������
enum ECloudTransOperType
{
	CLOUD_TRANS_OPER_QUERY = 0,
	CLOUD_TRANS_OPER_REQUEST = 1,
};
/// �ͻ����еķ�Ƭ״̬
enum ETransSubClientState
{
	TRANS_SUB_CLIENT_WAITING = 0,
	TRANS_SUB_CLIENT_RUNNING = 1,
	TRANS_SUB_CLIENT_DONE = 2,
	TRANS_SUB_CLIENT_FAIL = 3
};

/// �ͻ��˶�ת�����������
enum ETransSubClientRevise
{
	TRANS_SUB_CLIENT_REVISE_SERVER = 0,
	TRANS_SUB_CLIENT_REVISE_HOST = 1
};

enum ETransState //ת��״̬
{
	TRANS_STATE_NONE_OFFLINE_FILE = 0,
	TRANS_STATE_NO_TRANS_IN_CLOUD = 1,
	TRANS_STATE_WAITING_IN_CLOUD = 2,
	TRANS_STATE_RUNNING_IN_CLOUD = 3,
	TRANS_STATE_DONE_IN_CLOUD = 4,
	TRANS_STATE_FAIL_IN_CLOUD = 5
};

enum ETransSpec //ת����
{};
/// ��Ƭת��Ľ���
enum ETransSubClientSuggestion
{
	TRANS_SUB_CLIENT_SUGG_NONE = 0,
	TRANS_SUB_CLIENT_SUGG_WAIT = 1
};
/// �������еķ�Ƭ״̬
enum ETransSubServerState
{
	TRANS_SUB_SERVER_WAITING = 0,
	TRANS_SUB_SERVER_RUNNING = 1,
	TRANS_SUB_SERVER_DONE = 2,
	TRANS_SUB_SERVER_FAIL = 3
};

// ת������
// 0���ɹ�
// ������������
enum ECloudTransResponseResult
{
	RESPONSE_OK = 0,
	// ...
};
/// ��������Ͱ���httpͷ������Ӧ��Ҳ��httpͷ��
struct CCloudTransCommandBase
{
	typedef xl::uint16	UInt16;
	typedef xl::uint32	UInt32;
	typedef xl::uint64	UInt64;
	typedef xl::uint32	Bool;
	typedef std::string String;
	
	const String& cmd_name() const { return m_cmd_name; }
	UInt32 magic_number() const { return m_magic_number; }
	UInt16 protocol_ver() const { return m_protocol_ver; }
	UInt32 body_len() const { return m_body_len; }
	//UInt32 set_body_len() const { return m_body_len; }
protected:
	UInt32	m_magic_number;
	UInt16	m_protocol_ver;
	UInt32	m_body_len;
	String	m_cmd_name;
protected:
	CCloudTransCommandBase();
	CCloudTransCommandBase(const String& cmd_name);
	virtual ~CCloudTransCommandBase() {}

	virtual char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	virtual char* TransByteOrderAndRead(char* pOutData, unsigned length);
public:
	static unsigned SizeOf();
	virtual unsigned Length() const;
};


/// ���������
struct CCloudTransRequestBase : public CCloudTransCommandBase
{
	typedef CCloudTransCommandBase CBase;
protected:
	CCloudTransRequestBase() {}
	CCloudTransRequestBase(const String& cmd_name);

	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
private:
	/// ���ǻ���÷��������������
	char* TransByteOrderAndRead(char* pOutData, unsigned length);
public:
	//static unsigned SizeOf();
	virtual unsigned Length() const;
};
/// ��Ӧ������
struct CCloudTransResponseBase : public CCloudTransCommandBase
{
	typedef CCloudTransCommandBase CBase;
	UInt32	m_result;
protected:
	CCloudTransResponseBase() {}
	CCloudTransResponseBase(const String& cmd_name);

	char* TransByteOrderAndRead(char* pOutData, unsigned length);
private:
	/// ���ǻ���÷��������������
	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
public:
	//static unsigned SizeOf();
	virtual unsigned Length() const;
};

/// ��Client֮���Э�顣
/// 1. client_request -- �ͻ��˷������ת�����
struct CTransSubInfoFromClient
{
	typedef CCloudTransCommandBase::UInt16 UInt16;
	typedef CCloudTransCommandBase::UInt32 UInt32;
	typedef CCloudTransCommandBase::UInt64 UInt64;
	typedef CCloudTransCommandBase::String String;

	UInt64	m_start_time;
	UInt64	m_duration_time;
	UInt32	m_client_state;
	UInt64	m_client_finished;
	UInt32	m_client_revise;
public:
	CTransSubInfoFromClient()
		: m_start_time(0), m_duration_time(0), m_client_state(0), 
		m_client_finished(0), m_client_revise(0)
	{}
	unsigned Length() const;
/// way 1 ...
	TransByteOrderIOStream& operator << (TransByteOrderIOStream& ins);
	TransByteOrderIOStream& operator >> (TransByteOrderIOStream& outs);
};

///.way 2  for test ...
TransByteOrderIOStream& operator >> (TransByteOrderIOStream& ins, CTransSubInfoFromClient& obj);
TransByteOrderIOStream& operator << (TransByteOrderIOStream& outs, const CTransSubInfoFromClient& obj);

struct CClientRequest : public CCloudTransRequestBase
{
	typedef CCloudTransRequestBase CBase;

	String	m_thunder_ver;
	String	m_user_id;
	String	m_cid;
	String	m_gcid;
	UInt64	m_file_size;
	UInt64	m_duration;
	UInt32	m_operation_type;
	UInt32	m_trans_height;
	UInt32	m_trans_width;
	UInt32	m_trans_quality;
	String	m_trans_container_name;
	UInt32	m_tell_me_trans_info;
	UInt32	m_trans_tool_ver;
	UInt32	m_trans_args_ver;
	UInt32	m_trans_sub_info_ver;
	std::vector<CTransSubInfoFromClient>	m_trans_sub_info_from_client;
public:
	CClientRequest();

	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	unsigned Length() const;
};

struct CTransSubInfoToClient
{
	typedef CCloudTransCommandBase::UInt16 UInt16;
	typedef CCloudTransCommandBase::UInt32 UInt32;
	typedef CCloudTransCommandBase::UInt64 UInt64;
	typedef CCloudTransCommandBase::String String;
	
	UInt64	m_start_time;
	UInt64	m_duration_time;
	UInt32	m_client_suggestion;
	UInt32	m_server_state;
	UInt64	m_server_finished;
	String	m_url;
public:
	unsigned Length() const;
	TransByteOrderIOStream& operator << (TransByteOrderIOStream& ins);
	TransByteOrderIOStream& operator >> (TransByteOrderIOStream& outs);
};

struct CClientResponse : public CCloudTransResponseBase
{
	typedef CCloudTransResponseBase CBase;

	UInt32	m_trans_state;
	String	m_gcid;
	UInt32	m_trans_spec;
	UInt32	m_trans_tool_ver;
	UInt32	m_trans_args_ver;
	String	m_trans_args;
	String	m_trans_cid;
	String	m_trans_gcid;
	UInt64	m_trans_file_size;
	String	m_trans_url;
	UInt32	m_trans_sub_info_ver;
	std::vector<CTransSubInfoToClient> m_trans_sub_info_to_client;
public:
	CClientResponse();

	char* TransByteOrderAndRead(char* pOutData, unsigned length);
	unsigned Length() const;
};

/// ����ȷ�����֮���Э��
/// 1.	scheduler_ping -- ���ȷ�������ʱ�ϱ���
struct CSchedulerPingRequest : public CCloudTransRequestBase
{
	typedef CCloudTransRequestBase CBase;

	UInt32	m_service_port;
	UInt32	m_capability;
	UInt32	m_tasks;
	String	m_section;
public:
	CSchedulerPingRequest();

	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	unsigned Length() const;
};

struct CSchedulerPingResponse : public CCloudTransResponseBase
{
	typedef CCloudTransResponseBase CBase;

public:
	CSchedulerPingResponse();

	char* TransByteOrderAndRead(char* pOutData, unsigned length);
	unsigned Length() const;
};

/// 2.	submit_cloud_trans_request -- �ύ��ת����������ȷ�������
struct CTransInfoToServer
{
	typedef CCloudTransCommandBase::UInt16 UInt16;
	typedef CCloudTransCommandBase::UInt32 UInt32;
	typedef CCloudTransCommandBase::UInt64 UInt64;
	typedef CCloudTransCommandBase::String String;

	UInt64	m_start_time;
	UInt64	m_duration_time;
	UInt32	m_server_suggestion;
public:
	unsigned Length() const;
	TransByteOrderIOStream& operator << (TransByteOrderIOStream& ins);
	TransByteOrderIOStream& operator >> (TransByteOrderIOStream& outs);
};

struct SubmitCloudTransRequest : public CCloudTransRequestBase
{	
	typedef CCloudTransRequestBase CBase;

	String	m_cid;
	String	m_gcid;
	UInt64	m_file_size;
	UInt64	m_duration;
	UInt32	m_operation_type;
	UInt32	m_trans_spec;
	String	m_trans_container_name;
	String	m_trans_args;
	std::vector<CTransInfoToServer> m_tans_sub_info_to_server;
public:
	SubmitCloudTransRequest();

	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	unsigned Length() const;
};

struct CTransSubInfoFromServer
{
	typedef CCloudTransCommandBase::UInt16 UInt16;
	typedef CCloudTransCommandBase::UInt32 UInt32;
	typedef CCloudTransCommandBase::UInt64 UInt64;
	typedef CCloudTransCommandBase::String String;
	
	UInt64	m_start_time;
	UInt64	m_duration_time;
	UInt32	m_server_state;
	UInt64	m_server_finished;
	String	m_trans_url;
public:
	unsigned Length() const;
	TransByteOrderIOStream& operator << (TransByteOrderIOStream& ins);
	TransByteOrderIOStream& operator >> (TransByteOrderIOStream& outs);
};

struct SubmitCloudTransResponse : public CCloudTransResponseBase
{
	typedef CCloudTransResponseBase CBase;

	UInt32	m_trans_state;
	String	m_trans_cid;
	String	m_trans_gcid;
	UInt64	m_trans_file_size;
	String	m_trans_url;
	std::vector<CTransSubInfoFromServer> m_trans_sub_info_from_server;
public:
	SubmitCloudTransResponse();

	char* TransByteOrderAndRead(char* pOutData, unsigned length);
	unsigned Length() const;
};
/// 3.	scheduler_report -- ���ȷ����������ת����ϱ�ת������ֻ��ת��ɹ����ϱ�����Ч��
struct CTransSubInfoFinished // the same as CTransSubInfoFromServer ...
{
	typedef CCloudTransCommandBase::UInt16 UInt16;
	typedef CCloudTransCommandBase::UInt32 UInt32;
	typedef CCloudTransCommandBase::UInt64 UInt64;
	typedef CCloudTransCommandBase::String String;

	UInt64	m_start_time;
	UInt64	m_duration_time;
	UInt32	m_server_state;
	UInt64	m_server_finished;
	String	m_trans_url;
public:
	unsigned Length() const;
	TransByteOrderIOStream& operator << (TransByteOrderIOStream& ins);
	TransByteOrderIOStream& operator >> (TransByteOrderIOStream& outs);
};


struct CShedulerReportRequest : public CCloudTransRequestBase
{
	typedef CCloudTransRequestBase CBase;

	String	m_cid;
	String	m_gcid;
	UInt64	m_file_size;
	UInt64	m_duration;
	UInt32	m_trans_state;
	String	m_trans_cid;
	String	m_trans_gcid;
	UInt64	m_trans_file_size;
	String	m_trans_url;
	std::vector<CTransSubInfoFinished> m_trans_sub_info_finished;
public:
	CShedulerReportRequest();

	char* TransByteOrderAndWrite(char* pOutData, unsigned length);
	unsigned Length() const;
};

struct CShedulerReportResponse : public CCloudTransResponseBase
{
	typedef CCloudTransResponseBase CBase;

public:
	CShedulerReportResponse();

	char* TransByteOrderAndRead(char* pOutData, unsigned length);
	unsigned Length() const;
};

//////////////////////////////////////////////////////////////////////////
#endif