// TestMobile.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include "stdafx.h"
#include <vector>
#include <string>
#include <cstring>
#include <memory>

#include "TransByteOrderStream.h"
#include "MobileCommands.h"
using namespace std;
using namespace xl;


#define SAFE_DELETE(a) do { if(NULL != (a)) { delete (a); (a) = NULL; } } while(0)
#define SAFE_DELETE_ARRAY(p) do {if(p) { delete [] (p); (p) = NULL; } }while(0)

int _tmain(int argc, _TCHAR* argv[])
{
	{
		cout << " " << CCommandResponseBase::SizeOf() << "\n";
	}

///*
	{
	CFindFilesRequest request;
	request.m_protocol_version = 0x1111;
	request.m_sequence = 0x1111;
	assert(MOBILE_REQUEST_FIND_FILE == request.m_command_name);
	request.m_command_flag = 0x2222;
	request.m_priority_level = 0x2222;
	request.m_partition_index = 0x2222;
	request.m_path_name = "2222";
	request.m_need_recursion = 0x2222;

	std::vector<string> str_vec;
	str_vec.push_back("avi");
	str_vec.push_back("mp4");
	str_vec.push_back("3gp");
	request.m_type_array.push_back(str_vec);
	str_vec.clear();
	str_vec.push_back("mp3");
	str_vec.push_back("wav");
	str_vec.push_back("wma");
	request.m_type_array.push_back(str_vec);
	str_vec.clear();
	request.m_command_length = request.Length();

	cout << " request.Length():" << request.Length() << "\n";
	char buffer[100];
	memset(buffer, '\0', 100);
	CHandleFindFileEncode hEncode;
	hEncode.SetData(&request);
	hEncode.Encode(buffer, sizeof(buffer));

// test CopyCommandObject() ...
	CFindFilesRequest* pRequest2 = (CFindFilesRequest*)hEncode.CopyCommandObject();
	const CFindFilesRequest& request2 = *pRequest2;
	// or ...
// test operator >> () ...
//	CFindFilesRequest request2;
// 	TransByteOrderIOStream ins(buffer, sizeof(buffer));
//
// 	ins >> request2.m_protocol_version;
// 	ins >> request2.m_sequence;
// 	ins >> request2.m_command_length;
// 	ins >> request2.m_command_name;
// 	ins >> request2.m_command_flag;
// 	ins >> request2.m_priority_level;
// 	ins >> request2.m_partition_index;
// 	ins >> request2.m_path_name;
// 	ins >> request2.m_need_recursion;
// 	ins >> request2.m_type_array;


	cout << " request2.m_path_name:" << request2.m_path_name << "\n";
	cout << " request2.m_need_recursion:" << hex << request2.m_need_recursion << "\n";
	cout << " request2.m_priority_level:" << hex << request2.m_priority_level << "\n";

	cout << dec;
	cout << " request2.m_command_length:" << request2.m_command_length << "\n";
	cout << " request2.Length():" << request2.Length() << "\n";

	for (size_t i=0; i < request2.m_type_array.size(); i++)
		for (size_t j=0; j < request2.m_type_array[i].size(); j++)
		{
			cout << " " << request2.m_type_array[i][j] << "\n";
		}
	SAFE_DELETE(pRequest2);
	}
///*
	{// test list partition
	CListPathResponse response;
	response.m_protocol_version = 0x1111;
	response.m_sequence = 0x1111;
	assert(MOBILE_RESPONSE_LIST_PATH == response.m_command_name);

	CFileStruct fileStruct;
	fileStruct.m_file_name = "2222";
	fileStruct.m_file_size = 2222;
	fileStruct.m_is_folder = FALSE;
	fileStruct.m_is_visible = TRUE;
	fileStruct.m_is_writable = TRUE;
	fileStruct.m_file_type = ".avi";
	fileStruct.m_modify_time = "2010/12/12";
	fileStruct.m_create_time = "2010/12/12";
	fileStruct.m_struct_length = fileStruct.Length()-sizeof(fileStruct.m_struct_length);

	response.m_file_array.push_back(fileStruct);
	response.m_file_array.push_back(fileStruct);

	response.m_command_length = response.Length();
	cout << " Length: " << response.m_command_length << "\n";

	char buffer[200];
	memset(buffer, '\0', 100);
	TransByteOrderIOStream outs(buffer, sizeof(buffer));//TransByteOrderOStream !
	outs << response.m_protocol_version;
	outs << response.m_sequence;
	outs << response.m_command_length;
	outs << response.m_command_name;
	outs << response.m_command_flag;
	outs << response.m_result;

	outs << response.m_file_array.size();
	for (size_t i=0; i < response.m_file_array.size(); i++)
	{
		response.m_file_array[i].operator <<(outs);
	}
	CListPathResponse response2;

	CHandleListPathDecode handleDecode;
	handleDecode.SetData(&response2);
	handleDecode.Decode(buffer, sizeof(buffer));
	cout << " response2.Length(): " << response2.Length() << "\n";
	cout << " response2.m_protocol_version: " << hex << response2.m_protocol_version << "\n";
	cout << " response2.m_file_array.size(): " << response2.m_file_array.size() << "\n";
	for (size_t i=0; i < response2.m_file_array.size(); i++)
	{
		cout << " m_create_time: " << response2.m_file_array[i].m_create_time << "\n";
		cout << " m_modify_time: " << response2.m_file_array[i].m_modify_time << "\n";
	}

	}

///*
	{ // test find file command ...
	CFindFileResponse response;
	response.m_protocol_version = 0x1111;
	response.m_sequence = 0x1111;
	assert(MOBILE_RESPONSE_FIND_FILE == response.m_command_name);
	response.m_command_flag = 0x2222;
	response.m_result = 0x2222;
	CTypeCalc typeCalc;
	typeCalc.m_total_file_size = 0x3333;
	typeCalc.m_total_file_count = 0x4444;
	response.m_type_collection.push_back(typeCalc);

	response.m_command_length = response.Length();
	cout << " Length: " << response.m_command_length << "\n";

	char buffer[100];
	memset(buffer, '\0', 100);
	TransByteOrderIOStream outs(buffer, sizeof(buffer));//TransByteOrderOStream !
	outs << response.m_protocol_version;
	outs << response.m_sequence;
	outs << response.m_command_length;
	outs << response.m_command_name;
	outs << response.m_command_flag;
	outs << response.m_result;

	outs << response.m_type_collection.size();
	for (size_t i=0; i < response.m_type_collection.size(); i++)
	{
		response.m_type_collection[i].operator <<(outs);
	}
	CFindFileResponse response2;

	CHandleFindFileDecode handleDecode;
	handleDecode.SetData(&response2);
	handleDecode.Decode(buffer, sizeof(buffer));
	cout << " response2.Length(): " << response2.Length() << "\n";
	cout << " response2.m_protocol_version: " << hex << response2.m_protocol_version << "\n";
	cout << " response2.m_type_collection.size(): " << response2.m_type_collection.size() << "\n";
	}
///*
	{
	CErrorResponse error_response;
	error_response.m_protocol_version = 1;
	error_response.m_sequence = 123456;
	cout << error_response.m_command_name << "\n";
	error_response.m_command_flag = 11111;
	error_response.m_result = 0;//assert
	error_response.m_error_code = 11111;
	error_response.m_command_length = error_response.Length();

	CTimeTickRequset request;
	request.m_protocol_version = 1;
	request.m_sequence = 123456;
	cout << request.m_command_name << "\n";
	request.m_command_flag = 11111;
	request.m_command_length = request.Length();

	cout << request.m_command_length << "\n";

	char buffer[100];
	memset(buffer, '\0', 100);
	TransByteOrderIOStream outs(buffer, sizeof(buffer));//TransByteOrderOStream !
	outs << error_response.m_protocol_version;
	outs << error_response.m_sequence;
	outs << error_response.m_command_length;
	outs << error_response.m_command_name;
	outs << error_response.m_command_flag;
	outs << error_response.m_result;
	outs << error_response.m_error_code;

	CHandleErrorDecode* handle_error_decode = new CHandleErrorDecode;
	CCommandResponseBase* pResponse = new CErrorResponse;
	handle_error_decode->SetData(pResponse);
	handle_error_decode->Decode(buffer, sizeof(buffer));

	CErrorResponse* pErrorResponse = static_cast<CErrorResponse*>(pResponse);
	cout << pErrorResponse->m_error_code << "\n";
	cout << pErrorResponse->m_sequence << "\n";
	cout << pErrorResponse->m_command_flag << "\n";

	IEncodeOperation* pEncode = new CHandleDeleteFileEncode;

	SAFE_DELETE(pEncode);
	SAFE_DELETE(pResponse);
	SAFE_DELETE(handle_error_decode);
	}
///*
	{/// test TransByteOrderIOStream
	char buffer[100];
	uint32 ui = 0x123456789LL;
	cout << " ui " << hex << ui << "\n";
	//hton(ui)
	TransByteOrderIOStream out_stream(buffer, sizeof(buffer));
	out_stream << ui;

	int* int_ptr = (int*)(out_stream.get_ptr()-out_stream.get_bytes_written());
	cout << " *int_ptr " << hex << *int_ptr << "\n";

	TransByteOrderIOStream in_stream(buffer, sizeof(buffer));
	uint32 ui_copy;
	in_stream >> ui_copy;
	cout << " ui_copy " << hex << ui_copy << "\n";
	}

///*
	{/// test TransByteOrderIOStream
	char buffer[100];
	uint32 ui = 0x123456789LL;
	cout << " ui " << hex << ui << "\n";
	//hton(ui)
	TransByteOrderIOStream out_stream(buffer, sizeof(buffer));
	out_stream << ui;

	int* int_ptr = (int*)(out_stream.get_ptr()-out_stream.get_bytes_written());
	cout << " *int_ptr " << hex << *int_ptr << "\n";

	TransByteOrderIOStream in_stream(buffer, sizeof(buffer));
	uint32 ui_copy;
	in_stream >> ui_copy;
	cout << " ui_copy " << hex << ui_copy << "\n";
	}

///*
	{
	cout << " " << TestBigEndian() << "\n";
	cout << " " << TestLittleEndian() << "\n";
	}
	/**                 */
	cout << "Hello world!" << endl;
	return 0;
}

