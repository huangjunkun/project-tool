
#include <cstring>
#include "c_single_file.h"

using namespace std;

int main()
{
	{
	using namespace junkun;
	c_single_file file(".\\data.txt", "r");
	const string_t str = file.get_str();
	wcout << str.size() << str << "\n";	
	}
/**
    {
    using namespace junkun;
    c_single_file file("write.txt");
    char str[1024*10] = {'0'};
    memset(str, str[0], sizeof(str));
    file.write(str, sizeof(str));
    file.close();

    file.open("write.txt", "rb+");
    char to_str[1024*10] = {'1'};
    memset(str, to_str[0], sizeof(str));
    std::cout << " " << to_str[0] << "\n";
    file.read(to_str, sizeof(to_str));
    std::cout << " " << to_str[0] << "\n";

    }

    /** =========================*/
    cout << "Hello world!" << endl;
    return 0;
}

