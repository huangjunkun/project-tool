// handle_vod_stat.cpp : 定义控制台应用程序的入口点。
//
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdlib>
#include <ctime>
#include <cstring>
//#include <tchar.h>
//#include <Windows.h>

using namespace std;

static std::ofstream s_fout("stat_result.log", ios::app);
//static std::string s_fout_buffer;
static std::ostringstream s_fout_buffer;


static const size_t FIRSTBUFFER_DIST_COUNT = 11;
static const size_t MAP_CC2COUNT_COUNT = 2;
static const size_t SPEED_DIST_COUNT = 6;

typedef std::map<unsigned long long, unsigned> map_cc2count;
struct stat_data_bundle {
    string time;//lable
    string machine;
    unsigned long long firstbuffer_sum; // 非试播
    unsigned long long t_firstbuffer_sum; // 试播
    size_t firstbuffer_count; // 非试播
    size_t t_firstbuffer_count; // 试播

    size_t firstbuffer_dist[FIRSTBUFFER_DIST_COUNT];// 分布0(0-1000), 1(1000-2000), 2(2000-3000), 3(3000-4000), 4(4000-5000), 5(5000-6000) ....
    size_t t_firstbuffer_dist[FIRSTBUFFER_DIST_COUNT];//

    map_cc2count map_cc2count_e2[MAP_CC2COUNT_COUNT]; //[0] 点播统计, [1]试播统计
    map_cc2count map_cc2count_e3[MAP_CC2COUNT_COUNT];
    double playspeed_sum;
    size_t playspeed_count; //
    size_t speed_dist[SPEED_DIST_COUNT];// 速度分布0(0-100), 1(100-200), 2(200-300), 3(300-400), 4(400-500), 5(500-)
    double t_playspeed_sum;
    size_t t_playspeed_count; //
    size_t t_speed_dist[SPEED_DIST_COUNT];//
};

static const size_t VDM_COUNT = 2;
//typedef stat_data_bundle v_stat_data_bundle[VDM_COUNT];
typedef vector<stat_data_bundle> v_stat_data_bundle;

long GetTickCount() {
    time_t timer;
    time(&timer);
    return timer;
}

void stat_first_buffer(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	unsigned long long t_firstbuffer_sum = 0; // 试播
	unsigned long long firstbuffer_sum = 0; // 非试播
	size_t t_firstbuffer_count = 0; // 试播
	size_t firstbuffer_count = 0; // 非试播
	const std::string FUNC_FLAG_FIRST_BUFFER = "f=firstbuffer";
	const std::string ARG_FLAG_FIRST_BUFFER_TIME = "time=";
	const std::string ARG_FLAG_FIRST_BUFFER_PLAYTYPE = "playtype=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(FUNC_FLAG_FIRST_BUFFER);
		assert (find_pos != string::npos);
		find_pos += FUNC_FLAG_FIRST_BUFFER.size();
		find_pos = buffer.find(ARG_FLAG_FIRST_BUFFER_TIME, find_pos);
		if (find_pos != string::npos) {
			find_pos += ARG_FLAG_FIRST_BUFFER_TIME.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			//char time_buffer[16];
			int time = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line time:" << time << "\n";
			find_pos = next_split_pos;
			find_pos = buffer.find(ARG_FLAG_FIRST_BUFFER_PLAYTYPE, find_pos);
			find_pos += ARG_FLAG_FIRST_BUFFER_PLAYTYPE.size();
			next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			const std::string playtype = buffer.substr(find_pos, next_split_pos - find_pos);
			//cout << "line playtype:" << playtype << "\n";
			//getch();
			if (playtype == "1") {
				t_firstbuffer_sum += time;
				++t_firstbuffer_count;
			} else {
				firstbuffer_sum += time;
				++firstbuffer_count;
			}
		}
	}
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	// %I64u、%I64o、%I64x %I64d /// lld，ll，llu，llx，这几个都是输出64位的，
	printf("t_firstbuffer_sum=%llu, firstbuffer_sum=%llu\n", t_firstbuffer_sum, firstbuffer_sum);
	printf("t_firstbuffer_count=%u, firstbuffer_count=%u\n", t_firstbuffer_count, firstbuffer_count);
	printf("t_firstbuffer=%llu, firstbuffer=%llu\n", t_firstbuffer_sum/t_firstbuffer_count,
		firstbuffer_sum/firstbuffer_count);

    s_fout_buffer << " t_firstbuffer=" << t_firstbuffer_sum/t_firstbuffer_count << " firstbuffer="
        << firstbuffer_sum/firstbuffer_count;
	//cout << t_firstbuffer_sum/t_firstbuffer_count << ", " << firstbuffer_sum/firstbuffer_count << "\n";
}


void stat_interrupt_rate(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	const std::string FUNC_FLAG_BUFFER = "f=buffer";
	const std::string ARG_FLAG_BUFFER_E = "e=";
	const std::string ARG_FLAG_BUFFER_CC = "cc=";
	const std::string ARG_FLAG_BUFFER_PLAYTYPE = "playtype=";
	const char SPLIT_FLAG = '&';
	map_cc2count map_cc2count_e2[MAP_CC2COUNT_COUNT]; //[0] 点播统计, [1]试播统计
	map_cc2count map_cc2count_e3[MAP_CC2COUNT_COUNT];

	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
		size_t find_pos = buffer.find(FUNC_FLAG_BUFFER);
		assert (find_pos != string::npos);
		find_pos += FUNC_FLAG_BUFFER.size();
		size_t playtype_pos = buffer.find(ARG_FLAG_BUFFER_PLAYTYPE, find_pos);
		int playtype = 0;// 默认为点播（播放方式）
		if (playtype_pos != string::npos) {
            playtype_pos = playtype_pos + ARG_FLAG_BUFFER_PLAYTYPE.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, playtype_pos);
            playtype = atoi(buffer.substr(playtype_pos, next_split_pos - playtype_pos).c_str());
            assert (playtype < 2);
            find_pos = playtype_pos;
		} else continue;

		find_pos = buffer.find(ARG_FLAG_BUFFER_E, find_pos);
		if (find_pos != string::npos) {
			find_pos += ARG_FLAG_BUFFER_E.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			int e_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			find_pos = next_split_pos;
			long long cc_value = 0;
			find_pos = buffer.find(ARG_FLAG_BUFFER_CC, find_pos);
			find_pos += ARG_FLAG_BUFFER_CC.size();
			next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			cc_value = atoll(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			if (cc_value != 0) {
				if (e_value == -2) {
					//map_cc2count_e2[playtype][cc_value] +=  1;
					map_cc2count::iterator it = map_cc2count_e2[playtype].find(cc_value);
					if (it == map_cc2count_e2[playtype].end()) {
						map_cc2count_e2[playtype].insert(make_pair(cc_value, 1));
					} else {
						it->second += 1;
					}
				} else if (e_value == -3) {
					//map_cc2count_e3[playtype][cc_value] +=  1;
					map_cc2count::iterator it = map_cc2count_e3[playtype].find(cc_value);
					if (it == map_cc2count_e3[playtype].end()) {
						map_cc2count_e3[playtype].insert(make_pair(cc_value, 1));
					} else {
						it->second +=  1;
					}
				} else continue;
			} else {
				continue;
			}
//			cout << "line buffer:" << buffer << "\n";
//			cout << "line e_value:" << e_value << "\n";
//			cout << "line cc_value:" << cc_value << "\n";
//			cout << "line playtype:" << playtype << "\n";
//			getch();
		}
	}

	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	for (size_t i = 0; i < MAP_CC2COUNT_COUNT; ++i) {
        //printf("[WARN] map_cc2count_e2[%u][0]=%u, map_cc2count_e3[%u][0]=%u\n", i, map_cc2count_e2[i][0], i, map_cc2count_e3[i][0]);
        printf("map_cc2count_e2[%u].size()=%u, map_cc2count_e3[%u].size()=%u\n", i, map_cc2count_e2[i].size(), i, map_cc2count_e3[i].size());
        printf("interrupt_rate[%u]=%f\n", i, (double)map_cc2count_e2[i].size() / map_cc2count_e3[i].size());
        s_fout_buffer << " interrupt_rate" << i << "=" << (double)map_cc2count_e2[i].size() / map_cc2count_e3[i].size();
	}
}

void stat_playspeed(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	double playspeed_sum = 0;
	size_t playspeed_count = 0; //
	const std::string FUNC_FLAG_PLAYSPEED = "f=playspeed";
	const std::string ARG_FLAG_PLAYSPEED_VAULE = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(FUNC_FLAG_PLAYSPEED);
		assert (find_pos != string::npos);
		find_pos += FUNC_FLAG_PLAYSPEED.size();
		find_pos = buffer.find(ARG_FLAG_PLAYSPEED_VAULE, find_pos);
		if (find_pos != string::npos) {
			find_pos += ARG_FLAG_PLAYSPEED_VAULE.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line speed:" << speed << "\n";
			//getch();
			playspeed_sum += speed;
			++playspeed_count;
		}
	}
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	printf("playspeed_count=%u, playspeed_sum=%f\n", playspeed_count, playspeed_sum);
	printf("playspeed=%f\n", playspeed_sum / playspeed_count);
	s_fout_buffer << " playspeed=" << playspeed_sum / playspeed_count;
}

void stat_trial_playspeed(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	double t_playspeed_sum = 0;
	size_t t_playspeed_count = 0; //
	const std::string FUNC_FLAG_TRIAL_PLAYSPEED = "f=trialPlayspeed";
	const std::string ARG_FLAG_TRIAL_PLAYSPEED_VAULE = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		size_t find_pos = buffer.find(FUNC_FLAG_TRIAL_PLAYSPEED);
		assert (find_pos != string::npos);
		find_pos += FUNC_FLAG_TRIAL_PLAYSPEED.size();
		find_pos = buffer.find(ARG_FLAG_TRIAL_PLAYSPEED_VAULE, find_pos);
		if (find_pos != string::npos) {
			find_pos += ARG_FLAG_TRIAL_PLAYSPEED_VAULE.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
			double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
			//cout << "line speed:" << speed << "\n";
			//getch();
			t_playspeed_sum += speed;
			++t_playspeed_count;
		}
	}
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	printf("trialPlayspeed_count=%u, trialPlayspeed_value_count=%f\n", t_playspeed_count, t_playspeed_sum);
	printf("trialPlayspeed=%f\n", t_playspeed_sum / t_playspeed_count);
	s_fout_buffer << " trialPlayspeed=" << t_playspeed_sum / t_playspeed_count;
}
int stat_single_func(int argc, char* argv[]) {
    //s_fout_buffer.clear();
	/**     */
	int func_num = -1;
	const size_t FUNC_INFOS_SIZE = 4;
	const char* FUNC_INFOS[] = {"firstbuffer", "buffer", "playspeed", "trialPlayspeed"};
	const char* data_file = NULL;
	istream* ins_ptr = NULL;
	if (argc == 3) {
        func_num = atoi(argv[1]);
        data_file = argv[2];
    } else if(argc == 2) {
        // 后门捷径！
        data_file = argv[1];
        if (strstr(data_file, ".gz") != NULL) {
            //cerr << "Sorry it's not .gz file." << data_file << endl;
            return -1;
        }
        if (strstr(data_file, FUNC_INFOS[0]) != NULL) {
            func_num = 0;
        } else if (strstr(data_file, FUNC_INFOS[1]) != NULL) {
            func_num = 1;
        } else if (strstr(data_file, FUNC_INFOS[3]) != NULL) {
            func_num = 3;
        } else if (strstr(data_file, FUNC_INFOS[2]) != NULL) {
            func_num = 2;
        }
    } else {
//		cerr << "Parameters error.\n" << endl;
//		cerr << "Usage: stat.exe [stat_func_num](0.first buffer, 1.interrupt rate, 2.play speed, 3.trial play speed.) [data_file_path]. \n" << endl;
//		return -1;
	}
	ifstream fin(data_file);
    if (!fin) {
        cerr << "load the data file[" << data_file << "] error, and load from std::cin.\n" << endl;
        ins_ptr = &std::cin;
        //return -1;
    } else {
        cout << "load the data file[" << data_file << "] success. \nNow start to parsing data ...\n";
        ins_ptr = &fin;
    }

    if (func_num < FUNC_INFOS_SIZE);
        s_fout_buffer << "[FILE]" << data_file << " [FUNC]" << FUNC_INFOS[func_num];

	if (func_num == 0) {
		stat_first_buffer(*ins_ptr);
	} else if (func_num == 1) {
		stat_interrupt_rate(*ins_ptr);
	} else if (func_num == 2) {
		stat_playspeed(*ins_ptr);
	} else if (func_num == 3) {
		stat_trial_playspeed(*ins_ptr);
	} else {
		cout << "Parameters error.\n" << endl;
		cout << "Usage: stat.exe [stat_func_num](0.first buffer, 1.interrupt rate, 2.play speed, 3.trial play speed.) [data_file_path]. \n" << endl;
		return -1;
	}
    return 0;
}


int index_of_machine(int func_num, const vector<v_stat_data_bundle>& stats, const string& buffer, size_t find_offset = 0) {
	if (func_num == 0) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = "vod=" + stats[i][0].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }
	} else if (func_num == 1) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i][0].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }

	} else if (func_num == 2) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i][0].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }

	} else if (func_num == 3) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i][0].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }
	}
	//assert(false);
    return -1;
}

void stat_all_func(vector<v_stat_data_bundle>& stats) {
    const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
    std::string buffer;
    buffer.reserve(DEFAULT_BUFFER_SIZE);

    const std::string FUNC_FLAG_FIRST_BUFFER = "firstbuffer";
    const std::string ARG_FLAG_FIRST_BUFFER_TIME = "&time=";
    const std::string ARG_FLAG_FIRST_BUFFER_PLAYTYPE = "&playtype=";
    const std::string SPLIT_FLAG = "&";
    const std::string FUNC_FLAG_BUFFER = "buffer";
    const std::string ARG_FLAG_BUFFER_E = "&e=";
    const std::string ARG_FLAG_BUFFER_CC = "&cc=";
    const std::string ARG_FLAG_BUFFER_PLAYTYPE = "&playtype=";
    const std::string FUNC_FLAG_PLAYSPEED = "playspeed";
    const std::string ARG_FLAG_PLAYSPEED_VAULE = "&s=";
	const std::string FUNC_FLAG_TRIAL_PLAYSPEED = "trialPlayspeed";
	const std::string ARG_FLAG_TRIAL_PLAYSPEED_VAULE = "&s=";
	const std::string FUNC_FLAG = "&f=";
	const std::string VOD_DOWNLOAD_MODE_FLAG = "&vdm=";

    int start_tick_count = GetTickCount();
    while (getline(std::cin, buffer)) {
        //cout << "line buffer:" << buffer << "\n";
        size_t func_flag_pos = string::npos;
        if ((func_flag_pos = buffer.find(FUNC_FLAG)) == string::npos) {
            continue;
        }
        func_flag_pos += FUNC_FLAG.size();
        size_t find_pos = buffer.find(SPLIT_FLAG, func_flag_pos);
        //assert (find_pos != string::npos);
        if (find_pos == string::npos) {
            continue;
        }
        const string func_value = buffer.substr(func_flag_pos, find_pos - func_flag_pos);
        stat_data_bundle* stat_ptr = NULL;
        int vdm_value = -1;
        size_t next_split_pos;
        if (func_value == FUNC_FLAG_FIRST_BUFFER) {
            int index = index_of_machine(0, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            find_pos = buffer.find(VOD_DOWNLOAD_MODE_FLAG, find_pos);
            if (find_pos == string::npos) {
                continue;
            }
            find_pos += VOD_DOWNLOAD_MODE_FLAG.size();
            next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
            vdm_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
            assert (vdm_value < VDM_COUNT);
            stat_ptr = &stats[index][vdm_value];
            find_pos = next_split_pos;
            find_pos = buffer.find(ARG_FLAG_FIRST_BUFFER_TIME, find_pos);
            if (find_pos != string::npos) {
                find_pos += ARG_FLAG_FIRST_BUFFER_TIME.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                int time = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                // TEST ...
//                if (time > 60 * 1000) {
//                    printf("[WARNING] time=%i, buffer=%s\n", time, buffer.c_str());
//                    continue;
//                }

                find_pos = next_split_pos;
                find_pos = buffer.find(ARG_FLAG_FIRST_BUFFER_PLAYTYPE, find_pos);
                find_pos += ARG_FLAG_FIRST_BUFFER_PLAYTYPE.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                const std::string playtype = buffer.substr(find_pos, next_split_pos - find_pos);
                if (playtype == "1") {
                    stat_ptr->t_firstbuffer_sum += time;
                    ++stat_ptr->t_firstbuffer_count;
                    int dist_i = time/1000;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)FIRSTBUFFER_DIST_COUNT-1);
                    ++(stat_ptr->t_firstbuffer_dist[dist_i]);
                } else {
                    stat_ptr->firstbuffer_sum += time;
                    ++stat_ptr->firstbuffer_count;
                    int dist_i = time/1000;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)FIRSTBUFFER_DIST_COUNT-1);
                    ++(stat_ptr->firstbuffer_dist[dist_i]);
                }
            }
        } else if (func_value == FUNC_FLAG_BUFFER) {
            int index = index_of_machine(1, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            find_pos = buffer.find(VOD_DOWNLOAD_MODE_FLAG, find_pos);
            if (find_pos == string::npos) {
                continue;
            }
            find_pos += VOD_DOWNLOAD_MODE_FLAG.size();
            next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
            vdm_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
            assert (vdm_value < VDM_COUNT);
            find_pos = next_split_pos;
            stat_ptr = &stats[index][vdm_value];
            size_t playtype_pos = buffer.find(ARG_FLAG_BUFFER_PLAYTYPE, find_pos);
            int playtype = 0;// 默认为点播（播放方式）
            if (playtype_pos != string::npos) {
                playtype_pos = playtype_pos + ARG_FLAG_BUFFER_PLAYTYPE.size();
                next_split_pos = buffer.find(SPLIT_FLAG, playtype_pos);
                playtype = atoi(buffer.substr(playtype_pos, next_split_pos - playtype_pos).c_str());
                assert (playtype < 2);
                find_pos = playtype_pos;
            } else continue;

            find_pos = buffer.find(ARG_FLAG_BUFFER_E, find_pos);
            if (find_pos != string::npos) {
                find_pos += ARG_FLAG_BUFFER_E.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                int e_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                find_pos = next_split_pos;
                long long cc_value = 0;
                find_pos = buffer.find(ARG_FLAG_BUFFER_CC, find_pos);
                find_pos += ARG_FLAG_BUFFER_CC.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                cc_value = atoll(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                if (cc_value != 0) {
                    if (e_value == -2) {
                        //map_cc2count_e2[playtype][cc_value] +=  1;
                        map_cc2count::iterator it = stat_ptr->map_cc2count_e2[playtype].find(cc_value);
                        if (it == stat_ptr->map_cc2count_e2[playtype].end()) {
                            stat_ptr->map_cc2count_e2[playtype].insert(make_pair(cc_value, 1));
                        } else {
                            it->second += 1;
                        }
                    } else if (e_value == -3) {
                        //map_cc2count_e3[playtype][cc_value] +=  1;
                        map_cc2count::iterator it = stat_ptr->map_cc2count_e3[playtype].find(cc_value);
                        if (it == stat_ptr->map_cc2count_e3[playtype].end()) {
                            stat_ptr->map_cc2count_e3[playtype].insert(make_pair(cc_value, 1));
                        } else {
                            it->second +=  1;
                        }
                    } else continue;
                } else {
                    continue;
                }
            }
        } else if (func_value == FUNC_FLAG_PLAYSPEED) {
            int index = index_of_machine(2, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            find_pos = buffer.find(VOD_DOWNLOAD_MODE_FLAG, find_pos);
            if (find_pos == string::npos) {
                continue;
            }
            find_pos += VOD_DOWNLOAD_MODE_FLAG.size();
            next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
            vdm_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
            assert (vdm_value < VDM_COUNT);
            find_pos = next_split_pos;
            stat_ptr = &stats[index][vdm_value];
            find_pos = buffer.find(ARG_FLAG_PLAYSPEED_VAULE, find_pos);
            if (find_pos != string::npos) {
                find_pos += ARG_FLAG_PLAYSPEED_VAULE.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                if (speed > 0 && speed < 10 * 1024 * 1024) {
                    stat_ptr->playspeed_sum += speed;
                    ++(stat_ptr->playspeed_count);
                    int dist_i = ((int)speed)/100;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)SPEED_DIST_COUNT-1);
                    ++(stat_ptr->speed_dist[dist_i]);
                }

            }
        } else if (func_value == FUNC_FLAG_TRIAL_PLAYSPEED) {
            int index = index_of_machine(3, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            find_pos = buffer.find(VOD_DOWNLOAD_MODE_FLAG, find_pos);
            if (find_pos == string::npos) {
                continue;
            }
            find_pos += VOD_DOWNLOAD_MODE_FLAG.size();
            next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
            vdm_value = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
            assert (vdm_value < VDM_COUNT);
            find_pos = next_split_pos;
            stat_ptr = &stats[index][vdm_value];
            find_pos = buffer.find(ARG_FLAG_TRIAL_PLAYSPEED_VAULE, find_pos);
            if (find_pos != string::npos) {
                find_pos += ARG_FLAG_TRIAL_PLAYSPEED_VAULE.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                if (speed > 0 && speed < 10 * 1024 * 1024) {
                    stat_ptr->t_playspeed_sum += speed;
                    ++(stat_ptr->t_playspeed_count);
                    int dist_i = ((int)speed)/100;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, 5);
                    ++(stat_ptr->t_speed_dist[dist_i]);
                }
            }
        }
    }

    printf("cost time=%i\n", GetTickCount() - start_tick_count);
    for (size_t i = 0; i < stats.size(); ++i) {
        for (size_t j = 0; j < stats[i].size(); ++j) {
			stat_data_bundle& bundle = stats[i][j];
			// %I64u、%I64o、%I64x %I64d /// lld，ll，llu，llx，这几个都是输出64位的，
			const string fout_prefix = "[time]" + bundle.time + "[machine]" + bundle.machine + (j==0 ? "[P2S]":"[P2P]");
			printf("==== %s ====\n", fout_prefix.c_str());
			long firstbuffer = (bundle.firstbuffer_count==0 ? 0 : bundle.firstbuffer_sum/bundle.firstbuffer_count);
			long t_firstbuffer = (bundle.t_firstbuffer_count==0 ? 0 : bundle.t_firstbuffer_sum/bundle.t_firstbuffer_count);
			printf("firstbuffer_sum=%llu, t_firstbuffer_sum=%llu, firstbuffer_count=%u, t_firstbuffer_count=%u, firstbuffer=%u, t_firstbuffer=%u\n",
				   bundle.firstbuffer_sum, bundle.t_firstbuffer_sum,
				   bundle.firstbuffer_count, bundle.t_firstbuffer_count,
				   firstbuffer, t_firstbuffer);
			printf("firstbuffer_dist distribution:");
			for (size_t j = 0; j < FIRSTBUFFER_DIST_COUNT; ++j) {
				printf(" [%u-%u)(%u)", j*1000, j*1000+1000, bundle.firstbuffer_dist[j]);
			}
			printf("\n");
			printf("t_firstbuffer_dist distribution:");
			for (size_t j = 0; j < FIRSTBUFFER_DIST_COUNT; ++j) {
				printf(" [%u-%u)(%u)", j*1000, j*1000+1000, bundle.t_firstbuffer_dist[j]);
			}
			printf("\n");

			s_fout_buffer << fout_prefix << " firstbuffer_count=" << bundle.firstbuffer_count << " t_firstbuffer_count=" << bundle.t_firstbuffer_count
			<< " firstbuffer=" << firstbuffer << " t_firstbuffer=" << t_firstbuffer;

			for (size_t j = 0; j < MAP_CC2COUNT_COUNT; ++j) {
				//printf("[WARN] map_cc2count_e2[%u][0]=%u, map_cc2count_e3[%u][0]=%u\n", j, bundle.map_cc2count_e2[j][0], j, bundle.map_cc2count_e3[j][0]);
				printf("cc2count_e2[%u]=%u, map_cc2count_e3[%u]=%u, ", j, bundle.map_cc2count_e2[j].size(), j, bundle.map_cc2count_e3[j].size());
				double interrupt_rate = (bundle.map_cc2count_e3[j].size()==0 ? 0 : (double)bundle.map_cc2count_e2[j].size() / bundle.map_cc2count_e3[j].size());
				s_fout_buffer << " " << (j==0 ? "interrupt_rate" : "t_interrupt_rate") << "=" << interrupt_rate;
				printf("interrupt_rate%u=%f\n", j, interrupt_rate);
			}

			double playspeed = (bundle.playspeed_count==0 ? 0 : bundle.playspeed_sum/bundle.playspeed_count);
			printf("playspeed_count=%u, playspeed_sum=%f, playspeed=%f\n",
				   bundle.playspeed_count, bundle.playspeed_sum, playspeed);
			printf("playspeed distribution:");
			for (size_t j = 0; j < SPEED_DIST_COUNT; ++j) {
				printf(" [%u-%u)(%u)", j*100, j*100+100, bundle.speed_dist[j]);
			}
			printf("\n");
			s_fout_buffer << " playspeed=" << playspeed;

			double trialPlayspeed = (bundle.t_playspeed_count==0 ? 0 : bundle.t_playspeed_sum/bundle.t_playspeed_count);
			printf("trialPlayspeed_count=%u, trialPlayspeed_value_count=%f, trialPlayspeed=%f\n",
				   bundle.t_playspeed_count, bundle.t_playspeed_sum, trialPlayspeed);
			printf("trialPlayspeed distribution:");
			for (size_t j = 0; j < SPEED_DIST_COUNT; ++j) {
				printf(" [%u-%u)(%u)", j*100, j*100+100, bundle.t_speed_dist[j]);
			}
			printf("\n");
			s_fout_buffer << " trialPlayspeed=" << trialPlayspeed << "\n";
			printf("=============================================\n");
        }
    }
}

int stat_all_func(int argc, char* argv[]) {
    if (argc < 3) {
		cerr << "Parameters error.\n" << endl;
		cerr << "Usage: stat.exe [time](e.g 2013110520) [machines](e.g 221.235.189.167 ). \n" << endl;
		return -1;
    }
    //std::string time = argv[1];
    vector<v_stat_data_bundle> stat_datas;
    stat_datas.resize(argc-2);
    for (size_t i = 0; i < stat_datas.size(); ++i) {
        stat_datas[i].resize(VDM_COUNT);
        for (size_t j = 0; j < VDM_COUNT; ++j) {
            stat_datas[i][j].time = argv[1];
            stat_datas[i][j].machine = argv[i + 2];
            stat_datas[i][j].t_firstbuffer_sum = 0;
            stat_datas[i][j].firstbuffer_sum = 0;
            stat_datas[i][j].t_firstbuffer_count = 0;
            stat_datas[i][j].firstbuffer_count = 0;
            stat_datas[i][j].playspeed_sum = 0;
            stat_datas[i][j].playspeed_count = 0;
            stat_datas[i][j].t_playspeed_sum = 0;
            stat_datas[i][j].t_playspeed_count = 0;
            memset(&stat_datas[i][j].firstbuffer_dist, 0, sizeof(stat_datas[i][j].firstbuffer_dist));
            memset(&stat_datas[i][j].t_firstbuffer_dist, 0, sizeof(stat_datas[i][j].t_firstbuffer_dist));
            memset(&stat_datas[i][j].speed_dist, 0, sizeof(stat_datas[i][j].speed_dist));
            memset(&stat_datas[i][j].t_speed_dist, 0, sizeof(stat_datas[i][j].t_speed_dist));
        }
    }
//    const size_t FILE_COUNT = 24;
//    const std::string TIME_FILENAME_PREFIX = "vod_stat.log.";
//    const char* TIME_FILENAME_SUFFIX[FILE_COUNT]  = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
//    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" };
//        std::string data_file = TIME_FILENAME_PREFIX + time + TIME_FILENAME_SUFFIX[i];
//        ifstream fin(data_file.c_str());

//        if (!fin) {
//            cerr << "load the data file[" << data_file << "] error.\n" << endl;
//            break;
//        }
    // TODO

    stat_all_func(stat_datas);

	return 0;
}
int tool_trait_stat_values(int argc, char* argv[]) {
    const size_t TRAIT_STAT_FIELD_COUNT = 6;
    vector<string> firstbuffer_values;
    vector<string> t_firstbuffer_values;
    vector<string> interrupt_values;
    vector<string> t_interrupt_values;
    vector<string> playspeed_values;
    vector<string> t_playspeed_values;
	const std::string FUNC_FLAG_FIRST_BUFFER = "firstbuffer=";
	const std::string T_FUNC_FLAG_FIRST_BUFFER = "t_firstbuffer=";
	const std::string INTERRUPT_RATE_FLAG = "interrupt_rate0=";
	const std::string T_INTERRUPT_RATE_FLAG = "interrupt_rate1=";
	const std::string FUNC_FLAG_PLAYSPEED = "playspeed=";
	const std::string T_FUNC_FLAG_PLAYSPEED = "trialPlayspeed=";
	const std::string SPLIT_FLAG = " ";
	vector<string>* stat_values_array[TRAIT_STAT_FIELD_COUNT] = {&firstbuffer_values, &t_firstbuffer_values, &interrupt_values, &t_interrupt_values, &playspeed_values, &t_playspeed_values};
	const std::string* stat_values_flag_array[TRAIT_STAT_FIELD_COUNT] = {&FUNC_FLAG_FIRST_BUFFER, &T_FUNC_FLAG_FIRST_BUFFER, &INTERRUPT_RATE_FLAG, &T_INTERRUPT_RATE_FLAG, &FUNC_FLAG_PLAYSPEED, &T_FUNC_FLAG_PLAYSPEED};
    if (argc < 2) {
		cerr << "Parameters error.\n" << endl;
		cerr << "Usage: trait_stat_values.exe [in_file] [out_file](defaule './trait_stat_values_result.txt'). \n" << endl;
		return -1;
    }
    const char* in_file = argv[1];
    const char* out_file = NULL;
    if (argc == 2)
        out_file = "trait_stat_values_result.txt";
    else
        out_file = argv[2];

	ifstream fin(in_file);
    if (!fin) {
        cerr << "load the data file[" << in_file << "] error.\n" << endl;
        return -1;
    }

    const size_t DEFAULT_BUFFER_SIZE = 1 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	while (getline(fin, buffer)) {
		//cout << "line buffer:" << buffer << "\n";
		if (buffer.empty())
            continue;

		size_t find_pos = 0;
        for (size_t i = 0; i < TRAIT_STAT_FIELD_COUNT; ++i) {
            find_pos = buffer.find(*(stat_values_flag_array[i]), find_pos);
            if (find_pos == string::npos) {
                break;
            }
            find_pos += stat_values_flag_array[i]->size();
            size_t split_pos = buffer.find(SPLIT_FLAG, find_pos);
            stat_values_array[i]->push_back(buffer.substr(find_pos, split_pos-find_pos));
            find_pos = split_pos + SPLIT_FLAG.size();
            //cout << "flag:" << *(stat_values_flag_array[i]) << stat_values_array[i]->back() << "\n";
        }
	}
    std::ofstream fout(out_file, ios::app);
    assert (fout);
    for (size_t i = 0; i < TRAIT_STAT_FIELD_COUNT; ++i) {
        fout << *stat_values_flag_array[i];
        const vector<string>& stat_values_vec = *stat_values_array[i];
        for (size_t j = 0; j < stat_values_vec.size(); ++j) {
            fout << stat_values_vec[j] << "\t";
        }
        fout << "\n";
    }
    fout << std::endl;
    fout.close();
    fin.close();
    cout << "Output:[file]" << out_file << ".\n";
}

int main(int argc, char* argv[])
{
    assert (s_fout && s_fout_buffer);
    cout << "Running " << argv[0] << ".\n";
    //stat_single_func(argc, argv);
    stat_all_func(argc, argv);
    //tool_trait_stat_values(argc, argv);
	//s_fout_buffer << endl;
    s_fout << s_fout_buffer.str() << endl;
	//cout << "\n input any char to end.\n" << endl;
	//getch();
	return 0;
}
