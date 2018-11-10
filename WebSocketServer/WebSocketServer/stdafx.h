
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#define WM_NEW_CONNECT              WM_USER + 0x100   //新建立的连接
// #define WM_MSG_PUSH_DATA            WM_USER + 0x518 /////yangshg 2018/01/31 Java传来的push数据
// #define WM_MSG_CALLOUT_CALL         WM_USER + 0x510 //yangshg 2018/04/25 16:55:49给呼叫中心发送拨打消息
// #define WM_MSG_FROM_CALLDLG         WM_USER + 0x511
// #define WM_MSG_CALL_HUNGUP_BY_HTML  WM_USER + 0x519 // 将网页发送的挂断消息发送到指定窗口
// #define WM_MSG_CALL_ACCEPT_BY_HTML  WM_USER + 0x520 // 将网页发送的接听消息发送到指定窗口
// #define WM_MSG_CALL_REFUSE_BY_HTML  WM_USER + 0x521 //yangshg  将网页发送的拒接消息发送到指定窗口
#define   IPC_CALLSTATE				12138

#include <iostream>
using namespace std;
typedef struct _tagCallout_Call
{
	std::string m_strPhoneNum;
	std::string m_strName;
	std::string m_strAddress;
}Callout_Call;
typedef struct _tagCallin_Refuse 
{
	std::string m_strPeerNumber;
	int m_nCallID;
}Callin_Refuse;
typedef struct _tagCallin_Accept
{
	std::string m_strPeerNumber;
	int	m_nCallID;
}Callin_Accept;
typedef struct _tagCallHangup
{
	std::string m_strPeerNumber;
	int m_nCallID;
	std::string m_strType;
}Call_Hangup;


#include <afxsock.h>            // MFC 套接字扩展




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



#include <iostream>  
#include <string>  
#include <fstream>  
//包含rapidjson必要头文件,rapidjson文件夹拷贝到工程目录，或者设置include路径，或者加入到工程树  
#include "rapidjson/document.h"  
#include "rapidjson/filestream.h"  
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h"  
using namespace std;  
using namespace rapidjson;  //引入rapidjson命名空间  


