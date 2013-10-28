// handle_vod_stat.cpp : 定义控制台应用程序的入口点。
//
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <conio.h>
#include <tchar.h>
#include <Windows.h>

using namespace std;

#ifdef _UNICODE
	#define info(msg) do { std::wcout << msg << "\n"; } while(0)
	#define debug(msg) do { std::wcout << msg << "\n"; } while(0)
#else
	#define info(msg) do { std::cout << msg << "\n"; } while(0)
	#define debug(msg) do { std::cout << msg << "\n"; } while(0)
#endif


int test()
{
	return 0;
}

inline void swap1 (int& a, int& b)
{
	int c = a;
	a = b;
	b = c;
}

inline void swap2 (int& a, int& b)
{ // error
	a = b + 0 * (b = a);
}
void test_swap()
{
	int a = 111, b = 999;
	swap1(a, b);
	cout << " " << a << " " << b <<"\n";
	a = 111, b = 999;
	swap2(a, b);
	cout << " " << a << " " << b <<"\n";
}

void stat_first_buffer(ifstream& fin) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	ULONG64 first_buffer_trial_count = 0; // 试播
	ULONG64 first_buffer_noraml_count = 0; // 非试播
	size_t trial_count = 0; // 试播
	size_t noraml_count = 0; // 非试播
	const std::string FIRST_BUFFER_FLAG = "f=firstbuffer";
	const std::string FIRST_BUFFER_TIME_FLAG = "time=";
	const std::string FIRST_BUFFER_PLAYTYPE_FLAG = "playtype=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(fin, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(FIRST_BUFFER_FLAG);
		assert (find_pos != string::npos);
		find_pos += FIRST_BUFFER_FLAG.size();
		find_pos = buffer.find(FIRST_BUFFER_TIME_FLAG, find_pos);
		if (find_pos != string::npos) {
			find_pos += FIRST_BUFFER_TIME_FLAG.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			//char time_buffer[16];
			int time = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line time:" << time << "\n";
			find_pos = next_split_pos;
			find_pos = buffer.find(FIRST_BUFFER_PLAYTYPE_FLAG, find_pos);
			find_pos += FIRST_BUFFER_PLAYTYPE_FLAG.size();
			next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			const std::string playtype = buffer.substr(find_pos, next_split_pos - find_pos);
			//cout << "line playtype:" << playtype << "\n";
			//getch();
			if (playtype == "1") {
				first_buffer_trial_count += time;
				++trial_count;
			} else {
				first_buffer_noraml_count += time;
				++noraml_count;
			}
		}
	}
	printf("cost time=%i\n", ::GetTickCount() - start_tick_count);
	// %I64u、%I64o、%I64x %I64d /// lld，ll，llu，llx，这几个都是输出64位的，
	printf("first_buffer_trial_count=%llu, first_buffer_noraml_count=%llu\n", first_buffer_trial_count, first_buffer_noraml_count);
	printf("trial_count=%u, noraml_count=%u\n", trial_count, noraml_count);
	printf("first_buffer_trial=%llu, first_buffer_noraml=%llu\n", first_buffer_trial_count/trial_count,
		first_buffer_noraml_count/noraml_count);
	//cout << first_buffer_trial_count/trial_count << ", " << first_buffer_noraml_count/noraml_count << "\n";
}

typedef std::map<UINT64, unsigned> map_cc2count;
//bool map_cc2count::iterator::operator==(const map_cc2count::iterator& it )
//{
//	return this.first == it->first;
//}

void stat_interrupt_rate(ifstream& fin) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	ULONG64 first_buffer_trial_count = 0; // 试播
	ULONG64 first_buffer_noraml_count = 0; // 非试播
	size_t trial_count = 0; // 试播
	size_t noraml_count = 0; // 非试播
	const std::string BUFFER_FLAG = "f=buffer";
	const std::string BUFFER_E_FLAG = "e=";
	const std::string BUFFER_CC_FLAG = "cc=";
	const std::string BUFFER_PLAYTYPE_FLAG = "playtype=";
	const char SPLIT_FLAG = '&';
	map_cc2count map_cc2count_e2;
	map_cc2count map_cc2count_e3;

	int start_tick_count = ::GetTickCount();
	while (getline(fin, buffer)) {
		size_t find_pos = buffer.find(BUFFER_FLAG);
		assert (find_pos != string::npos);
		find_pos += BUFFER_FLAG.size();
		find_pos = buffer.find(BUFFER_E_FLAG, find_pos);
		if (find_pos != string::npos) {
			find_pos += BUFFER_E_FLAG.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			int e_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			find_pos = next_split_pos;
			long long cc_value = 0;
			find_pos = buffer.find(BUFFER_CC_FLAG, find_pos);
			find_pos += BUFFER_CC_FLAG.size();
			next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			cc_value = _atoi64(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			if (cc_value != 0) {
				if (e_value == -2) {
					//map_cc2count_e2[cc_value] +=  1;
					map_cc2count::iterator it = map_cc2count_e2.find(cc_value);
					if (it == map_cc2count_e2.end()) {
						map_cc2count_e2.insert(make_pair(cc_value, 1));
					} else {
						it->second += 1;
					}
				} else if (e_value == -3) {
					//map_cc2count_e3[cc_value] +=  1;
					map_cc2count::iterator it = map_cc2count_e3.find(cc_value);
					if (it == map_cc2count_e3.end()) {
						map_cc2count_e3.insert(make_pair(cc_value, 1));
					} else {
						it->second +=  1;
					}
				}
			} else {
				continue;
			}
			//cout << "line buffer:" << buffer << "\n";
			//cout << "line e_value:" << e_value << "\n";
			//cout << "line cc_value:" << cc_value << "\n";
			//getch();
		}
	}

	printf("cost time=%i\n", ::GetTickCount() - start_tick_count);
	printf("[WARN] map_cc2count_e2[0]=%u, map_cc2count_e3[0]=%u\n", map_cc2count_e2[0], map_cc2count_e3[0]);
	printf("map_cc2count_e2.size()=%u, map_cc2count_e3.size()=%u\n", map_cc2count_e2.size(), map_cc2count_e3.size());
	printf("interrupt_rate=%f\n", (double)map_cc2count_e2.size() / map_cc2count_e3.size());
}

void stat_playspeed(ifstream& fin) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	double playspeed_value_count = 0;
	size_t playspeed_count = 0; //
	const std::string PLAYSPEED_FLAG = "f=playspeed";
	const std::string PLAYSPEED_VAULE_FLAG = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(fin, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(PLAYSPEED_FLAG);
		assert (find_pos != string::npos);
		find_pos += PLAYSPEED_FLAG.size();
		find_pos = buffer.find(PLAYSPEED_VAULE_FLAG, find_pos);
		if (find_pos != string::npos) {
			find_pos += PLAYSPEED_VAULE_FLAG.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line speed:" << speed << "\n";
			//getch();
			playspeed_value_count += speed;
			++playspeed_count;
		}
	}
	printf("cost time=%i\n", ::GetTickCount() - start_tick_count);
	printf("playspeed_count=%u, playspeed_value_count=%f\n", playspeed_count, playspeed_value_count);
	printf("playspeed=%f\n", playspeed_value_count / playspeed_count);
}

void stat_trial_playspeed(ifstream& fin) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	double playspeed_value_count = 0;
	size_t playspeed_count = 0; //
	const std::string TRIAL_PLAYSPEED_FLAG = "f=trialPlayspeed";
	const std::string TRIAL_PLAYSPEED_VAULE_FLAG = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(fin, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(TRIAL_PLAYSPEED_FLAG);
		assert (find_pos != string::npos);
		find_pos += TRIAL_PLAYSPEED_FLAG.size();
		find_pos = buffer.find(TRIAL_PLAYSPEED_VAULE_FLAG, find_pos);
		if (find_pos != string::npos) {
			find_pos += TRIAL_PLAYSPEED_VAULE_FLAG.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line speed:" << speed << "\n";
			//getch();
			playspeed_value_count += speed;
			++playspeed_count;
		}
	}
	printf("cost time=%i\n", ::GetTickCount() - start_tick_count);
	printf("trialPlayspeed_count=%u, trialPlayspeed_value_count=%f\n", playspeed_count, playspeed_value_count);
	printf("trialPlayspeed=%f\n", playspeed_value_count / playspeed_count);
}

int _tmain(int argc, _TCHAR* argv[])
{
	/**     */
	if (argc < 3) {
		cout << "Parameters error.\n" << endl;
		cout << "Usage: stat.exe [stat_func_num](0.first buffer, 1.interrupt rate, 2.play speed, 3.trial play speed.) [data_file_path]. \n" << endl;
		return -1;
	}
	int func_num = atoi(argv[1]);
	const char* data_file = argv[2];
	ifstream fin(data_file);
	if (!fin) {
		cout << "load the data file[" << data_file << "] error.\n" << endl;
		return -1;
	}
	cout << "load the data file[" << data_file << "] success. Now start to parsing data ...\n";

	if (func_num == 0) {
		stat_first_buffer(fin);
	} else if (func_num == 1) {
		stat_interrupt_rate(fin);

	} else if (func_num == 2) {
		stat_playspeed(fin);
	} else if (func_num == 3) {
		stat_trial_playspeed(fin);
	} else {
		cout << "parameters error.\n" << endl;
	}

	cout << "\n input any char to end.\n" << endl;
	getch();
	return 0;
}
