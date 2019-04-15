// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "UIlib.h"

using namespace DuiLib;
using namespace Gdiplus;

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "base/control//UICefBrowser.h"

typedef CCefBrowserUI CCefUI;
#define ENABLE_ENCODE

#ifdef _DEBUG
#ifdef _USED_CHROME_69_3497
    #pragma comment(lib, "libcef_dll_wrapper_69_3497/libcef_dll_wrapper_d.lib")
#else
    #pragma comment(lib, "libcef_dll_wrapper_49_2623/libcef_dll_wrapper_d.lib")
#endif
    #pragma comment(lib, "encrypt/encrypt_d.lib")
    #pragma comment(lib, "util/util_d.lib")
#else
#ifdef _USED_CHROME_69_3497
    #pragma comment(lib, "libcef_dll_wrapper_69_3497/libcef_dll_wrapper.lib")
#else
    #pragma comment(lib, "libcef_dll_wrapper_49_2623/libcef_dll_wrapper.lib")
#endif
    #pragma comment(lib, "encrypt/encrypt.lib")
    #pragma comment(lib, "util/util.lib")
#endif 

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
