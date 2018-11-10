
// WebSocketServer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CWebSocketServerApp:
// 有关此类的实现，请参阅 WebSocketServer.cpp
//

typedef enum MyEnum
{
	theApp_kongxian,  //空闲
	theApp_waihu,     //外呼  
	theApp_huru,      //呼入
	theApp_jietong,   //接通
	theApp_guaduan,   //挂断
	theApp_jujie,     //拒接
};
class CWebSocketServerApp : public CWinAppEx
{
public:
	CWebSocketServerApp();

// 重写
	public:
	virtual BOOL InitInstance();

	MyEnum m_zhuangtai;

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CWebSocketServerApp theApp;