#pragma once

#include <set>
#include <string>

namespace V8WindowFunciotn
{
#define DECLARE_WINDOWS_FUNCTION(name, function) static std::string name = insert(function);

	static std::set<std::string> Functions;
	static auto insert = [&](const char* function)
	{ 
		Functions.insert(function);

		return function;
	};

	// js����window.FormatRequest(,,,)
	DECLARE_WINDOWS_FUNCTION(loginFunc, "FormatRequest");		//��¼��֤
	DECLARE_WINDOWS_FUNCTION(maxFunc, "MaximizeWnd");		//��󻯴���
	DECLARE_WINDOWS_FUNCTION(restoreFunc, "RestoreWnd");		//�ָ����ڴ�С
	DECLARE_WINDOWS_FUNCTION(minFunc, "MinimizeWnd");		//��С������
	DECLARE_WINDOWS_FUNCTION(ExitFunc, "CloseWnd");					//�˳�����
	DECLARE_WINDOWS_FUNCTION(domTest, "DomVisitTest");
}