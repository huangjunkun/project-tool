#include <iostream>

using namespace std;

#include "timer_class.h"


HWND GetConsoleHwnd(void)
{
    //#define MY_BUFSIZE 1024 // Buffer size for console window titles.
    HWND hwndFound;         // This is what is returned to the caller.
    char pszNewWindowTitle[MAX_PATH]; // Contains fabricated
    // WindowTitle.
    char pszOldWindowTitle[MAX_PATH]; // Contains original
    // WindowTitle.

    // Fetch current window title.

    GetConsoleTitle(pszOldWindowTitle, MAX_PATH);

    // Format a "unique" NewWindowTitle.

    wsprintf(pszNewWindowTitle,"%d/%d",
             GetTickCount(),
             GetCurrentProcessId());

    // Change current window title.

    SetConsoleTitle(pszNewWindowTitle);

    // Ensure window title has been updated.

    Sleep(40);

    // Look for NewWindowTitle.

    hwndFound=FindWindow(NULL, pszNewWindowTitle);

    // Restore original window title.

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}


class test_timer : public test_namespace::timer
{
public:
    test_timer() : timer() {}
    test_timer(unsigned time_tick) : timer(time_tick) {}
    test_timer(HWND hWnd) : timer(hWnd) {}
public:
    virtual void handle_timeout(unsigned timer_type)
    {
        std::cout << " handle_timeout \n";
    }
};


int main()
{

///*
    {
    using namespace std;
	using namespace test_namespace;
    //**
    HWND this_app_handle = GetConsoleHwnd();
    std::cout << " " << this_app_handle << "\n";
    using namespace std;
    using namespace test_namespace;
    test_timer a_test_timer(100);//test_timer
    MSG msg;
    while (GetMessage(&msg, NULL, NULL, NULL) != 0 && GetMessage(&msg, NULL, NULL, NULL) != -1)
    {
    	if (msg.message == WM_TIMER)
    	{
    		std::cout << "get a message \n";
    		TranslateMessage(&msg);
    		DispatchMessage(&msg);
    	}
    }
    }
    /** =   =   =   =   =   =   =   =   =   =*/
    cout << "Hello world!" << endl;
    return 0;
}
