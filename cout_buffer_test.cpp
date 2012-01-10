#include <iostream>
using namespace std;

#include "cout_buffer.h"

int main()
{
    {
    using namespace test_namespace;
    /// 0. testing ...
    cout_buffer << 1 << 2 << 3 << 4 << 5;
    unsigned ui1, ui2, ui3, ui4, ui5;
    cout_buffer >> ui1 >> ui2 >> ui3 >> ui4 >> ui5;
    cout << ui1 << ", " << ui2 << ", " << ui3 << ", " << ui4 << ", " << ui5 << "\n";
    /// 1. testing ...
    cout_buffer.reset();
    cout_buffer << "hello";
    cout_buffer << " world! \n";
    string res;
    cout_buffer >> res;
    std::cout << " res: " << res << "\n";
    cout_buffer >> res;
    std::cout << " res: " << res << "\n";

    std::cout << " " << cout_buffer.length() << ", " << cout_buffer.capability() << "\n";
    /// 2. testing ...
    cout_buffer.reset();
    CTestCoutBuffer test_obj;
    cout_buffer << test_obj;
    std::cout << " " << cout_buffer.to_string() << "\n";
    //std::cout << " " << cout_buffer.buffer() << "\n";// runtime error ...
    /// 3. testing ...
    cout_buffer.reset();
    vector<string> str_vec1, str_vec2;
    str_vec1.push_back("1111");
    str_vec1.push_back("2222");
    str_vec1.push_back("3333");
    cout_buffer << str_vec1;

    cout_buffer >> str_vec2;
    for (size_t i=0; i < str_vec2.size(); i++)
        std::cout << " " << str_vec2[i] << "\n";

    /// 4. testing ...
    cout_buffer.reset();
    vector<CTestCoutBuffer> test_vec1, test_vec2;
    test_obj.setValue("aaaaa");
    test_vec1.push_back(test_obj);
    test_obj.setValue("bbbbb");
    test_vec1.push_back(test_obj);
    test_obj.setValue("ccccc");
    test_vec1.push_back(test_obj);
    cout_buffer << test_vec1;

    cout_buffer >> test_vec2;
    for (size_t i=0; i < test_vec2.size(); i++)
        std::cout << " " << test_vec2[i].value() << "\n";

    /// 5. testing ...
    wcout_buffer.reset();
    vector<wstring> wstr_vec1, wstr_vec2;
    wstr_vec1.push_back(L"1111");
    wstr_vec1.push_back(L"2222");
    wstr_vec1.push_back(L"3333");
    wcout_buffer << wstr_vec1;

    wcout_buffer >> wstr_vec2;
    std::wcout << " " << wstr_vec2.size() << "\n";

    for (size_t i=0; i < wstr_vec2.size(); i++)
    {
        std::wcout << L" " << wstr_vec1[i] << L", "<< wstr_vec2[i] << L"\n";
        //std::wcout << L" " << wstr_vec2[i].size() << L"\n";
    }

    std::cout << " " << sizeof(unsigned) << ", " << sizeof(wchar_t) << "\n";
    }

    /**  =   =   =   =   =   =   =   */
    cout << "Hello world!" << endl;
    return 0;
}
