#include <iostream>
#include <tchar.h>
using namespace std;

#include "win_timer_class.h"


HWND GetConsoleHwnd(void)
{
    //#define MY_BUFSIZE 1024 // Buffer size for console window titles.
    HWND hwndFound;         // This is what is returned to the caller.
    TCHAR pszNewWindowTitle[MAX_PATH]; // Contains fabricated
    // WindowTitle.
    TCHAR pszOldWindowTitle[MAX_PATH]; // Contains original
    // WindowTitle.

    // Fetch current window title.

    GetConsoleTitle(pszOldWindowTitle, MAX_PATH);

    // Format a "unique" NewWindowTitle.

    wsprintf(pszNewWindowTitle, _T("%d/%d"),
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


class test_timer : public test_namespace::win_timer
{
public:
    test_timer()
        : win_timer() {}
    test_timer(const std::string & v)
        : win_timer(), _value(v) {}

    test_timer(HWND hWnd)
        : win_timer(hWnd) {}
    test_timer(HWND hWnd, const std::string & v)
        : win_timer(hWnd), _value(v) {}
public:
    virtual void handle_timeout(unsigned timer_type)
    {
        unsigned now_tick = ::GetTickCount();
        std::cout << " handle_timeout ...\n";
        std::cout << " " << now_tick - _last_timeout << "\n";
        std::cout << " value:" << _value << "\n";
        _last_timeout = now_tick;
    }
private:
    std::string _value;
    unsigned _last_timeout;
};

class test_timer2 : public test_namespace::win_timer
{
    static const unsigned DEFAULT_INTERVAL_TICK = 500;
public:
    test_timer2()
        : win_timer(DEFAULT_INTERVAL_TICK) {}
    test_timer2(const std::string & v)
        : win_timer(DEFAULT_INTERVAL_TICK), _value(v) {}

    test_timer2(HWND hWnd)
        : win_timer(hWnd, DEFAULT_INTERVAL_TICK) {}
    test_timer2(HWND hWnd, const std::string & v)
        : win_timer(hWnd, DEFAULT_INTERVAL_TICK), _value(v) {}
public:
    virtual void handle_timeout(unsigned timer_type)
    {
        unsigned now_tick = ::GetTickCount();
        std::cout << " handle_timeout ...\n";
        std::cout << " " << now_tick - _last_timeout << "\n";
        std::cout << " value:" << _value << "\n";
        _last_timeout = now_tick;
    }
private:
    std::string _value;
    unsigned _last_timeout;
};

LRESULT CALLBACK asyn_frame_wnd_proc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    std::cout << " asyn_frame_wnd_proc ...\n";
    switch ( uMsg )
    {
    case WM_USER+1:
        std::cout << "a WM_USER* to asyn_frame_wnd_proc ...\n";
        break;
    case WM_TIMER:
    {
        std::cout << "a WM_TIMER to asyn_frame_wnd_proc ...\n";
        test_namespace::win_timer* this_timer = (test_namespace::win_timer*) wParam;
        assert (this_timer);
        this_timer->handle_timeout(wParam);
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

TCHAR* WINDOW_CLASS_NAME= _T("win_timer_class_test");

HWND MyCreateWindow()
{
    HWND _hwnd = NULL;
// 	GUID guid = GUID_NULL;
// 	CoCreateGuid( &guid );
// 	USES_CONVERSION;
// 	LPOLESTR guid_ole_str = NULL;
// 	StringFromIID( guid, &guid_ole_str );
// 	//_wnd_class_name = OLE2A(guid_ole_str);
// 	CoTaskMemFree( guid_ole_str );

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style			= NULL;//CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= (WNDPROC)asyn_frame_wnd_proc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= GetModuleHandleW(NULL);
    wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32APP);
    wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= WINDOW_CLASS_NAME;//error -- _T("");
    wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    RegisterClassEx(&wcex);

    _hwnd = CreateWindow(wcex.lpszClassName, NULL, WS_POPUP,
                         0, 0, 0, 0, NULL, NULL, wcex.hInstance, NULL);

    assert( _hwnd ); // 如果窗口都创建失败，除了让程序崩溃别无他法。

    //SetWindowLong( _hwnd, GWL_USERDATA, (LONG)this );

    ShowWindow(_hwnd, SW_HIDE);
    return _hwnd;
}
void MyDestroyWindow(HWND& _hwnd)
{
    assert( _hwnd );
    BOOL ret = DestroyWindow( _hwnd );
    assert(ret);
    ret = UnregisterClass( WINDOW_CLASS_NAME, GetModuleHandleW(NULL) );
    assert(ret);
    _hwnd = NULL;
}

int main()
{
///*
    {
        using namespace std;
        using namespace test_namespace;
        //**
        ///HWND this_app_handle = GetConsoleHwnd();
        //std::cout << " " << this_app_handle << "\n";
        HWND new_handle = MyCreateWindow();
        using namespace std;
        using namespace test_namespace;
        test_timer a_test_timer(new_handle, "a_test_timer"); //1000
        test_timer2 b_test_timer(new_handle, "b_test_timer"); //
		std::cout << a_test_timer.interval_tick() << " " << b_test_timer.interval_tick() << "\n";
        MSG msg;
        while (GetMessage(&msg, NULL, NULL, NULL) != 0 && GetMessage(&msg, NULL, NULL, NULL) != -1)
        {
            if (msg.message == WM_TIMER)
            {
                //std::cout << " get a message is WM_TIMER.\n";
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
// 		while (true)
// 		{
// 			std::cout << " run and sleep 1s...\n";
// 			Sleep(1000);			
// 		}
        MyDestroyWindow(new_handle);
    }
    /** =   =   =   =   =   =   =   =   =   =*/
    cout << " Hello world!" << endl;
    return 0;
}
