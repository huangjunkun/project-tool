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


static const size_t FIRSTBUFFER_DIST_COUNT = 10;
static const size_t MAP_CC2COUNT_COUNT = 2;
static const size_t SPEED_DIST_COUNT = 6;

typedef std::map<unsigned long long, unsigned> map_cc2count;
struct stat_data_bundle {
    string time;//lable
    string machine;
    unsigned long long first_buffer_noraml_count; // 非试播
    unsigned long long first_buffer_trial_count; // 试播
    size_t noraml_count; // 非试播
    size_t trial_count; // 试播

    size_t firstbuffer_dist[FIRSTBUFFER_DIST_COUNT];// 分布0(0-1000), 1(1000-2000), 2(2000-3000), 3(3000-4000), 4(4000-5000), 5(5000-6000) ....
    size_t t_firstbuffer_dist[FIRSTBUFFER_DIST_COUNT];//

    map_cc2count map_cc2count_e2[MAP_CC2COUNT_COUNT]; //[0] 点播统计, [1]试播统计
    map_cc2count map_cc2count_e3[MAP_CC2COUNT_COUNT];
    double playspeed_value_count;
    size_t playspeed_count; //
    size_t speed_dist[SPEED_DIST_COUNT];// 速度分布0(0-100), 1(100-200), 2(200-300), 3(300-400), 4(400-500), 5(500-)
    double t_playspeed_value_count;
    size_t t_playspeed_count; //
    size_t t_speed_dist[SPEED_DIST_COUNT];//
};

long GetTickCount() {
    time_t timer;
    time(&timer);
    return timer;
}

void stat_first_buffer(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	unsigned long long first_buffer_trial_count = 0; // 试播
	unsigned long long first_buffer_noraml_count = 0; // 非试播
	size_t trial_count = 0; // 试播
	size_t noraml_count = 0; // 非试播
	const std::string FIRST_BUFFER_FLAG = "f=firstbuffer";
	const std::string FIRST_BUFFER_TIME_FLAG = "time=";
	const std::string FIRST_BUFFER_PLAYTYPE_FLAG = "playtype=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
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
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	// %I64u、%I64o、%I64x %I64d /// lld，ll，llu，llx，这几个都是输出64位的，
	printf("first_buffer_trial_count=%llu, first_buffer_noraml_count=%llu\n", first_buffer_trial_count, first_buffer_noraml_count);
	printf("trial_count=%u, noraml_count=%u\n", trial_count, noraml_count);
	printf("first_buffer_trial=%llu, first_buffer_noraml=%llu\n", first_buffer_trial_count/trial_count,
		first_buffer_noraml_count/noraml_count);

    s_fout_buffer << " first_buffer_trial=" << first_buffer_trial_count/trial_count << " first_buffer_noraml="
        << first_buffer_noraml_count/noraml_count;
	//cout << first_buffer_trial_count/trial_count << ", " << first_buffer_noraml_count/noraml_count << "\n";
}


void stat_interrupt_rate(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	const std::string BUFFER_FLAG = "f=buffer";
	const std::string BUFFER_E_FLAG = "e=";
	const std::string BUFFER_CC_FLAG = "cc=";
	const std::string BUFFER_PLAYTYPE_FLAG = "playtype=";
	const char SPLIT_FLAG = '&';
	map_cc2count map_cc2count_e2[2]; //[0] 点播统计, [1]试播统计
	map_cc2count map_cc2count_e3[2];

	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
		size_t find_pos = buffer.find(BUFFER_FLAG);
		assert (find_pos != string::npos);
		find_pos += BUFFER_FLAG.size();
		size_t playtype_pos = buffer.find(BUFFER_PLAYTYPE_FLAG, find_pos);
		int playtype = 0;// 默认为点播（播放方式）
		if (playtype_pos != string::npos) {
            playtype_pos = playtype_pos + BUFFER_PLAYTYPE_FLAG.size();
			size_t next_split_pos = buffer.find(SPLIT_FLAG, playtype_pos);
            playtype = atoi(buffer.substr(playtype_pos, next_split_pos - playtype_pos).c_str());
            assert (playtype < 2);
            find_pos = playtype_pos;
		} else continue;

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
	double playspeed_value_count = 0;
	size_t playspeed_count = 0; //
	const std::string PLAYSPEED_FLAG = "f=playspeed";
	const std::string PLAYSPEED_VAULE_FLAG = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
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
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	printf("playspeed_count=%u, playspeed_value_count=%f\n", playspeed_count, playspeed_value_count);
	printf("playspeed=%f\n", playspeed_value_count / playspeed_count);
	s_fout_buffer << " playspeed=" << playspeed_value_count / playspeed_count;
}

void stat_trial_playspeed(istream& ins) {
	const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
	std::string buffer;
	buffer.reserve(DEFAULT_BUFFER_SIZE);
	double t_playspeed_value_count = 0;
	size_t t_playspeed_count = 0; //
	const std::string TRIAL_PLAYSPEED_FLAG = "f=trialPlayspeed";
	const std::string TRIAL_PLAYSPEED_VAULE_FLAG = "s=";
	const char SPLIT_FLAG = '&';
	int start_tick_count = ::GetTickCount();
	while (getline(ins, buffer)) {
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
			t_playspeed_value_count += speed;
			++t_playspeed_count;
		}
	}
	printf("cost time=%i\n", GetTickCount() - start_tick_count);
	printf("trialPlayspeed_count=%u, trialPlayspeed_value_count=%f\n", t_playspeed_count, t_playspeed_value_count);
	printf("trialPlayspeed=%f\n", t_playspeed_value_count / t_playspeed_count);
	s_fout_buffer << " trialPlayspeed=" << t_playspeed_value_count / t_playspeed_count;
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


int index_of_machine(int func_num, const vector<stat_data_bundle>& stats, const string& buffer, size_t find_offset = 0) {
	if (func_num == 0) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = "vod=" + stats[i].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }
	} else if (func_num == 1) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }

	} else if (func_num == 2) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }

	} else if (func_num == 3) {
        for (size_t i = 0; i < stats.size(); ++i) {
            const string flag = stats[i].machine;
            if (buffer.find(flag, find_offset) != string::npos)
                return i;
        }
	}
    return -1;
}

void stat_all_func(vector<stat_data_bundle>& stats) {
    const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
    std::string buffer;
    buffer.reserve(DEFAULT_BUFFER_SIZE);

    const std::string FIRST_BUFFER_FLAG = "firstbuffer";
    const std::string FIRST_BUFFER_TIME_FLAG = "time=";
    const std::string FIRST_BUFFER_PLAYTYPE_FLAG = "playtype=";
    const char SPLIT_FLAG = '&';
    const std::string BUFFER_FLAG = "buffer";
    const std::string BUFFER_E_FLAG = "e=";
    const std::string BUFFER_CC_FLAG = "cc=";
    const std::string BUFFER_PLAYTYPE_FLAG = "playtype=";
    const std::string PLAYSPEED_FLAG = "playspeed";
    const std::string PLAYSPEED_VAULE_FLAG = "s=";
	const std::string TRIAL_PLAYSPEED_FLAG = "trialPlayspeed";
	const std::string TRIAL_PLAYSPEED_VAULE_FLAG = "s=";
	const std::string FUNC_FLAG = "f=";

    int start_tick_count = GetTickCount();
    while (getline(std::cin, buffer)) {
        //cout << "line buffer:" << buffer << "\n";
        size_t func_flah_pos = string::npos;
        if ((func_flah_pos = buffer.find(FUNC_FLAG)) == string::npos) {
            continue;
        }
        func_flah_pos += FUNC_FLAG.size();
        stat_data_bundle* stat_ptr = NULL;
        size_t find_pos = string::npos;
        if ((find_pos = buffer.find(FIRST_BUFFER_FLAG, func_flah_pos)) != string::npos) {
            find_pos += FIRST_BUFFER_FLAG.size();
            int index = index_of_machine(0, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            stat_ptr = &stats[index];
            find_pos = buffer.find(FIRST_BUFFER_TIME_FLAG, find_pos);
            if (find_pos != string::npos) {
                find_pos += FIRST_BUFFER_TIME_FLAG.size();
                size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                int time = atoi(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                find_pos = next_split_pos;
                find_pos = buffer.find(FIRST_BUFFER_PLAYTYPE_FLAG, find_pos);
                find_pos += FIRST_BUFFER_PLAYTYPE_FLAG.size();
                next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                const std::string playtype = buffer.substr(find_pos, next_split_pos - find_pos);
                if (playtype == "1") {
                    stat_ptr->first_buffer_trial_count += time;
                    ++stat_ptr->trial_count;
                    int dist_i = time/1000;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)FIRSTBUFFER_DIST_COUNT-1);
                    ++(stat_ptr->t_firstbuffer_dist[dist_i]);
                } else {
                    stat_ptr->first_buffer_noraml_count += time;
                    ++stat_ptr->noraml_count;
                    int dist_i = time/1000;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)FIRSTBUFFER_DIST_COUNT-1);
                    ++(stat_ptr->firstbuffer_dist[dist_i]);
                }
            }
        } else if ((find_pos = buffer.find(BUFFER_FLAG, func_flah_pos)) != string::npos) {
            find_pos += BUFFER_FLAG.size();
            int index = index_of_machine(1, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            stat_ptr = &stats[index];
            size_t playtype_pos = buffer.find(BUFFER_PLAYTYPE_FLAG, find_pos);
            int playtype = 0;// 默认为点播（播放方式）
            if (playtype_pos != string::npos) {
                playtype_pos = playtype_pos + BUFFER_PLAYTYPE_FLAG.size();
                size_t next_split_pos = buffer.find(SPLIT_FLAG, playtype_pos);
                playtype = atoi(buffer.substr(playtype_pos, next_split_pos - playtype_pos).c_str());
                assert (playtype < 2);
                find_pos = playtype_pos;
            } else continue;

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
        } else if ((find_pos = buffer.find(PLAYSPEED_FLAG, func_flah_pos)) != string::npos) {
            find_pos += PLAYSPEED_FLAG.size();
            int index = index_of_machine(2, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            stat_ptr = &stats[index];
            find_pos = buffer.find(PLAYSPEED_VAULE_FLAG, find_pos);
            if (find_pos != string::npos) {
                find_pos += PLAYSPEED_VAULE_FLAG.size();
                size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                if (speed > 0 && speed < 10 * 1024 * 1024) {
                    stat_ptr->playspeed_value_count += speed;
                    ++(stat_ptr->playspeed_count);
                    int dist_i = ((int)speed)/100;
                    dist_i = max(dist_i, 0);
                    dist_i = min(dist_i, (int)SPEED_DIST_COUNT-1);
                    ++(stat_ptr->speed_dist[dist_i]);
                }

            }
        } else if ((find_pos = buffer.find(TRIAL_PLAYSPEED_FLAG, func_flah_pos)) != string::npos) {
            find_pos += TRIAL_PLAYSPEED_FLAG.size();
            int index = index_of_machine(3, stats, buffer, find_pos);
            if (index == -1) {
                continue;
            }
            stat_ptr = &stats[index];
            find_pos = buffer.find(TRIAL_PLAYSPEED_VAULE_FLAG, find_pos);
            if (find_pos != string::npos) {
                find_pos += TRIAL_PLAYSPEED_VAULE_FLAG.size();
                size_t next_split_pos = buffer.find(SPLIT_FLAG, find_pos);
                double speed = atof(buffer.substr(find_pos, next_split_pos - find_pos).c_str());
                if (speed > 0 && speed < 10 * 1024 * 1024) {
                    stat_ptr->t_playspeed_value_count += speed;
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
        // %I64u、%I64o、%I64x %I64d /// lld，ll，llu，llx，这几个都是输出64位的，
        const string fout_prefix = "[time]" + stats[i].time + "[machine]" + stats[i].machine;
        printf("==== %s ====\n", fout_prefix.c_str());
        long first_buffer0 = stats[i].first_buffer_noraml_count/stats[i].noraml_count;
        long first_buffer1 = stats[i].first_buffer_trial_count/stats[i].trial_count;
        printf("first_buffer_noraml_count=%llu, first_buffer_trial_count=%llu, noraml_count=%u, trial_count=%u, first_buffer_noraml=%u, first_buffer_trial=%u\n",
               stats[i].first_buffer_noraml_count, stats[i].first_buffer_trial_count,
               stats[i].noraml_count, stats[i].trial_count,
               first_buffer0, first_buffer1);
        printf("firstbuffer_dist distribution:");
        for (size_t j = 0; j < FIRSTBUFFER_DIST_COUNT; ++j) {
            printf(" [%u-%u)(%u)", j*1000, j*1000+1000, stats[i].firstbuffer_dist[j]);
        }
        printf("\n");
        printf("t_firstbuffer_dist distribution:");
        for (size_t j = 0; j < FIRSTBUFFER_DIST_COUNT; ++j) {
            printf(" [%u-%u)(%u)", j*1000, j*1000+1000, stats[i].t_firstbuffer_dist[j]);
        }
        printf("\n");

        s_fout_buffer << fout_prefix << " first_buffer_noraml=" << first_buffer0 << " first_buffer_trial=" << first_buffer1;

        for (size_t j = 0; j < MAP_CC2COUNT_COUNT; ++j) {
            //printf("[WARN] map_cc2count_e2[%u][0]=%u, map_cc2count_e3[%u][0]=%u\n", j, stats[i].map_cc2count_e2[j][0], j, stats[i].map_cc2count_e3[j][0]);
            printf("cc2count_e2[%u]=%u, map_cc2count_e3[%u]=%u, ", i, stats[i].map_cc2count_e2[j].size(), j, stats[i].map_cc2count_e3[j].size());
            double interrupt_rate = (double)stats[i].map_cc2count_e2[j].size() / (stats[i].map_cc2count_e3[j].size()+1);
            printf("interrupt_rate%u=%f\n", j, interrupt_rate);
            s_fout_buffer << " interrupt_rate" << j << "=" << interrupt_rate;
        }

        double playspeed = stats[i].playspeed_value_count / (stats[i].playspeed_count+1);
        printf("playspeed_count=%u, playspeed_value_count=%f, playspeed=%f\n",
               stats[i].playspeed_count, stats[i].playspeed_value_count, playspeed);
        printf("playspeed distribution:");
        for (size_t j = 0; j < SPEED_DIST_COUNT; ++j) {
            printf(" [%u-%u)(%u)", j*100, j*100+100, stats[i].speed_dist[j]);
        }
        printf("\n");
        s_fout_buffer << " playspeed=" << playspeed;

        double trialPlayspeed = stats[i].t_playspeed_value_count / (stats[i].t_playspeed_count+1);
        printf("trialPlayspeed_count=%u, trialPlayspeed_value_count=%f, trialPlayspeed=%f\n",
               stats[i].t_playspeed_count, stats[i].t_playspeed_value_count, trialPlayspeed);
        printf("trialPlayspeed distribution:");
        for (size_t j = 0; j < SPEED_DIST_COUNT; ++j) {
            printf(" [%u-%u)(%u)", j*100, j*100+100, stats[i].t_speed_dist[j]);
        }
        printf("\n");
        s_fout_buffer << " trialPlayspeed=" << trialPlayspeed << "\n";
        printf("=============================================\n");
    }
}

int stat_all_func(int argc, char* argv[]) {
    if (argc < 3) {
		cerr << "Parameters error.\n" << endl;
		cerr << "Usage: stat.exe [time](e.g 2013110520) [machines](e.g 221.235.189.167 ). \n" << endl;
		return -1;
    }
    //std::string time = argv[1];
    vector<stat_data_bundle> stat_datas;
    stat_datas.resize(argc-2);
    for (size_t i = 0; i < stat_datas.size(); ++i) {
        stat_datas[i].time = argv[1];
        stat_datas[i].machine = argv[i + 2];
        stat_datas[i].first_buffer_trial_count = 0;
        stat_datas[i].first_buffer_noraml_count = 0;
        stat_datas[i].trial_count = 0;
        stat_datas[i].noraml_count = 0;
        stat_datas[i].playspeed_value_count = 0;
        stat_datas[i].playspeed_count = 0;
        stat_datas[i].t_playspeed_value_count = 0;
        stat_datas[i].t_playspeed_count = 0;
        memset(&stat_datas[i].speed_dist, 0, sizeof(stat_datas[i].speed_dist));
        memset(&stat_datas[i].t_speed_dist, 0, sizeof(stat_datas[i].t_speed_dist));
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

int main(int argc, char* argv[])
{
    assert (s_fout && s_fout_buffer);
    cout << "Running " << argv[0] << ".\n";
    //stat_single_func(argc, argv);
    stat_all_func(argc, argv);
	//s_fout_buffer << endl;
    s_fout << s_fout_buffer.str() << endl;
	//cout << "\n input any char to end.\n" << endl;
	//getch();
	return 0;
}
