
// WebSocketServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "sha1.h"
#include "base64.h"
#include "./log/LogWriter.h"


/******** yangshaoguang 2018/04/25 15:30:19 START ↓ ********/
//作用
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
/******** yangshaoguang 2018/04/25 15:30:19  END  ↑ ********/

#define WM_SOCKET (WM_USER+100)
#define CLIENT_COUNT 20
enum Connect_State
{
	Open = 0,
	Close
};
typedef struct _SocketStruct
{
	SOCKET socket;
	bool b_connect;
	string m_id6d;
	_SocketStruct()
	{
		b_connect = false;
		m_id6d = "";
	}
}MySocketClient;

enum TimerType
{
	NewConnectTimer,
};
// CWebSocketServerDlg 对话框
class CWebSocketServerDlg : public CDialog
{
// 构造
public:
	CWebSocketServerDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CWebSocketServerDlg()
	{
		if (log)
		{
			delete log;
			log = NULL;
		}
	}
// 对话框数据
	enum { IDD = IDD_WEBSOCKETSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	BOOL  m_TimerFlag;  //TRUE  标识定时器已经开启

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_IPCtl;
	CEdit m_PortEdit;
	CEdit m_RecvEdit;
	CEdit m_SendEdit;
	CButton m_sendBtn;
	CListBox m_ListBox;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	CString m_filePath;
	Connect_State netState;
	SOCKET sock; //服务器
	SOCKET client; //客户端
	MySocketClient clientarr[CLIENT_COUNT];//客户端数组
	sockaddr_in addr_client;
	int Count;
	CButton m_ConnectBtn;
	afx_msg LRESULT OnMySocket(WPARAM wParam, LPARAM lParam);
	std::string m_strMsgFromWeb; //yangshg 2018/04/25 15:00:14来自网页的消息
	void ProcessRecvMsg(std::string & str);
	void ProcessJsonObjRequest(const rapidjson::Value& data);
	CTime time1, time2;
	afx_msg LRESULT MySend(WPARAM wParam, LPARAM lParam);
	char* GetKey(char* handShakeHeader);
	std::string DecodeKey(std::string key);
	std::string m_strLastLongText;
	LogWriter *log;
	
	/******** yangshaoguang 2018/02/13 START ↓ ********/
	//作用
public:
	std::string CStringToString(LPCWSTR pwszSrc)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
		if (nLen <= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen - 1] = 0;
		std::string strTemp(pszDst);
		delete[] pszDst;
		return strTemp;
	}
	std::string MyGetFirstNChar(std::string stringTmp, int n) /////yangshg 2018/02/13获取一个字符串的前n个字符，返回字符串
	{
		std::string str;
		for (int i = 0; i <n; i++)
		{
			str += stringTmp[i];
		}
		return str;
	}
	std::string MyGetLastNChar(std::string stringTmp, int n) /////yangshg 2018/02/13获取一个字符串的后n个字符，返回字符串
	{
		std::string str;
		int len = stringTmp.length();
		for (int i = len - n; i < len; i++)
		{
			str += stringTmp[i];
		}
		return str;
	}
	/*******************************************
	*函数名称：U2G
	*函数功能：UTF8 ----> GB2312
	*函数参数：
	*返 回 值：
	*日    期：2018/02/27 17:34:02
	*  Name  : yangshaoguang
	********************************************/
	char* U2G(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return str;
	}
	/******** yangshaoguang 2018/02/13  END  ↑ ********/
	/*******************************************
	*函数名称：
	*函数功能：去掉传过来的号码的"-",以及号码区号的第一个0  +860571 ==>  +86571
	*函数参数：
	*返 回 值：
	*日    期：2018/03/13 09:58:45
	*  Name  : yangshaoguang
	********************************************/
	std::string RemoveFirstZero(std::string stringTmp)
	{
		std::string strNum = stringTmp;
		string tmp;
		size_t pos = strNum.find_first_of('-');
		int i = (int)pos;
		if (i != -1)
		{
			tmp = MyGetFirstNChar(strNum, i);// + MyGetLastNChar(strNum,strNum.length() - i -1);
			tmp += MyGetLastNChar(strNum, strNum.length() - i - 1);
			strNum = tmp;
		}
		if (strNum[3] == '0')
		{
			i = 3;
			tmp = MyGetFirstNChar(strNum, i);// + MyGetLastNChar(strNum,strNum.length() - i -1);
			tmp += MyGetLastNChar(strNum, strNum.length() - i - 1);
			strNum = tmp;
		}
		return strNum;
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SendToClient(string &strMsg);
	string m_strId6d; //存储连接上华为账号的id6d
};
