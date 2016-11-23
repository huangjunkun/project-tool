

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <set>
#include <stdio.h>

using namespace std;

int stat_uuid_rate(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Parameters error.\n" << endl;
        cerr << "Usage: stat.exe [uuid.txt](e:\\txt\\uuid.txt) [uuid2ch.txt](e:\\txt\\uuid2ch.txt). \n" << endl;
        return -1;
    }
    const char* str_uuid_file = argv[1];
    const char* str_uuid2ch_file = argv[2];
    ifstream fout_all_uuid(str_uuid_file), fout_uuid2ch(str_uuid2ch_file);
    map<string, pair<size_t, size_t> > map_ch2uuid_info;
    set<string> all_uuid;
    string uuid;
    while (fout_all_uuid >> uuid)
    {
        if (!uuid.empty())
        {
            all_uuid.insert(uuid);
            uuid.clear();
        }
    }
    cout << "all_uuid size=" << all_uuid.size() << "\n";

    string channel;
    while ((fout_uuid2ch >> uuid && fout_uuid2ch >> channel))
    {
        if (!uuid.empty())
        {
            size_t is_exist_in_all_uuid = (all_uuid.find(uuid) != all_uuid.end() ? 1 : 0);
            map<string, pair<size_t, size_t> >::iterator iter = map_ch2uuid_info.find(channel);
            if (iter != map_ch2uuid_info.end())
            {
                pair<size_t, size_t>& uuid_info = iter->second;
                uuid_info.first += 1;
                uuid_info.second += is_exist_in_all_uuid;
            }
            else
            {
                map_ch2uuid_info.insert(map<string, pair<size_t, size_t> >::value_type(channel, pair<size_t, size_t>(1, is_exist_in_all_uuid)));
            }

            uuid.clear();
            channel.clear();
        }
    }
    cout << "map_ch2uuid_info size=" << map_ch2uuid_info.size() << "\n";

    for (map<string, pair<size_t, size_t> >::const_iterator citer = map_ch2uuid_info.begin();
            citer != map_ch2uuid_info.end(); ++citer)
    {
        channel = citer->first;
        pair<size_t, size_t> uuid_info = citer->second;
        //cout << "channel=" << channel << ", count1=" << uuid_info.first << ", count2=" << uuid_info.second << "\n" ;
        printf("ch=%s rate=%0.2f%% count1=%i count2=%i\n", channel.c_str(), (uuid_info.second*100.0/uuid_info.first), uuid_info.first, uuid_info.second);
    }
    return 0;
}

