#include <algorithm>
#include <string>

#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_stream_resource_handler.h"

const char kMessageName[] = "WindowState";

// define window state.
#define WINDOW_MAXIMIZED "MAXIMIZED"
#define WINDOW_MINIMIZED "MINIMIZED"
#define WINDOW_NORMAL "NORMAL"
#define WINDOW_UNKNOWN_MSG "Unknown State"

// JS��C++�첽ͨ�ţ���02����
// ʵ��һ��CefMessageRouterBrowserSide::Handler���࣬��дOnQuery����
// Handle messages in the browser process.
class WindowStateHandler : public CefMessageRouterBrowserSide::Handler 
{
public:
	WindowStateHandler() {}

	virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id,
		const CefString& request,
		bool persistent,
		CefRefPtr<Callback> callback) OVERRIDE 
	{
		const std::string& message_name = request;
 		if (message_name.find(kMessageName) == 0) {
			// �õ���ǰ����
			
			return true;
 		}
		else if (request == "HelloCefQuery")
		{
			callback->Success("HelloCefQuery Ok");
			return true;
		}
		else if (request == "GiveMeMoney")
		{
			callback->Failure(404, "There are none thus query!");
			return true;
		}
		// let other handlers to process.
		return false;
	}

	virtual void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id) OVERRIDE
	{
		//LOG(INFO) << "Query request [Id: " << query_id << "] was cancelled.";

		// cancel async query task...
	}
};