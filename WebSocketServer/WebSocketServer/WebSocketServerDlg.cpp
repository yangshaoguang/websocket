
// WebSocketServerDlg.cpp : 实现文件
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "WebSocketServer.h"
#include "WebSocketServerDlg.h"
#include "afxconv.h"
#include "string"
#include "char_trans_coding.h"

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


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IPC_TEST  159753

typedef struct __MyStruct //改结构体用于进程间通讯传递字符串
{
	char msg[2048];
}MyStruct;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#pragma comment(lib,"ws2_32.lib")
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/******** yangshaoguang 2018/05/02 17:42:59 START ↓ ********/
//作用
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;
typedef unsigned char BYTE;
inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x - 10 + 'A' : x + '0';
}
inline BYTE fromHex(const BYTE &x)
{
	return isdigit(x) ? x - '0' : x - 'A' + 10;
}
inline string URLEncode1(const string &sIn)
{
	string sOut;
	for (size_t ix = 0; ix < sIn.size(); ix++)
	{
		BYTE buf[4];
		memset(buf, 0, 4);
		if (isalnum((BYTE)sIn[ix]))
		{
			buf[0] = sIn[ix];
		}
		else
		{
			buf[0] = '%';
			buf[1] = toHex((BYTE)sIn[ix] >> 4);
			buf[2] = toHex((BYTE)sIn[ix] % 16);
		}
		sOut += (char *)buf;
	}
	return sOut;
};
inline string URLDecode1(const string &sIn)
{
	string sOut;
	for (size_t ix = 0; ix < sIn.size(); ix++)
	{
		BYTE ch = 0;
		if (sIn[ix] == '%')
		{
			ch = (fromHex(sIn[ix + 1]) << 4);
			ch |= fromHex(sIn[ix + 2]);
			ix += 2;
		}
		else if (sIn[ix] == '+')
		{
			ch = ' ';
		}
		else
		{
			ch = sIn[ix];
		}
		sOut += (char)ch;
	}
	return sOut;
}

/******** yangshaoguang 2018/05/02 17:42:59  END  ↑ ********/


/******** yangshaoguang 2018/04/25 13:17:25 START ↓ ********/
//作用  utf8 -->GB2312
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
std::string GBToUTF8(const char* str)
{
	std::string result;
	WCHAR *strSrc;
	TCHAR *szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i + 1];
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

// CWebSocketServerDlg 对话框

CWebSocketServerDlg::CWebSocketServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebSocketServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	netState = Close;
	Count = 0;
	m_TimerFlag = FALSE;
	m_strLastLongText = "";

	char name[50] = {0};
	int timecount = GetTickCount();
	sprintf(name,"./log/WebSocket%d.log",timecount);
	log = new LogWriter(name);
}

void CWebSocketServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPCtl);
	DDX_Control(pDX, IDC_EDIT1, m_PortEdit);
	DDX_Control(pDX, IDC_EDIT2, m_RecvEdit);
	DDX_Control(pDX, IDC_EDIT3, m_SendEdit);
	DDX_Control(pDX, IDC_BUTTON2, m_sendBtn);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_BUTTON1, m_ConnectBtn);
}

BEGIN_MESSAGE_MAP(CWebSocketServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CWebSocketServerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CWebSocketServerDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_SOCKET,OnMySocket)
	ON_MESSAGE(WM_NEW_CONNECT,MySend)
	ON_WM_COPYDATA()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWebSocketServerDlg 消息处理程序

BOOL CWebSocketServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	log->write("启动WebSocket服务器");
	char pFileName[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pFileName);
	CString csFullPath(pFileName);
	m_filePath = csFullPath + _T("\\Record");
	m_filePath += _T("\\");
	m_filePath += _T("KFCallout");
	m_filePath = _T("C:\\KFCallout");
	DeleteFile(m_filePath);
	SetWindowText("__53__WebSocketServer__53__");
 	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
 	SetWindowPos(NULL, 0, 0, 0, 0, NULL);
 	ShowWindow(SW_HIDE); 
	m_IPCtl.SetWindowText(_T("127.0.0.1"));
	m_PortEdit.SetWindowText(_T("8356"));
	OnBnClickedButton1();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWebSocketServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebSocketServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebSocketServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


const char* _host = "127.0.0.1";
void CWebSocketServerDlg::OnBnClickedButton1() //连接网络
{
	// TODO: 在此添加控件通知处理程序代码
	if (netState == Close)
	{
		int err;
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			PostQuitMessage(0);
			return;
		}
		//创建Socket
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
		{
			PostQuitMessage(0);
			return;
		}
		sockaddr_in service;
		hostent *localHost;
		char *localIP;
		DWORD ipaddr;
		BYTE *ip;
		localHost = gethostbyname("");
		localIP = inet_ntoa(*(in_addr*)*localHost->h_addr_list);
		ipaddr = inet_addr(localIP);
		ip = (BYTE*)&ipaddr;
		service.sin_family = AF_INET;
		service.sin_addr.S_un.S_addr = inet_addr(_host);
		service.sin_port = htons(8356);
		err = bind(sock, (sockaddr*)&service, sizeof(service));
		if (err != 0)
		{
			log->write("Bind Socket failed");
			PostQuitMessage(0);
			return;
		}
		err = WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);
		if (err == SOCKET_ERROR)
		{
			log->write("Select failed");
			PostQuitMessage(0);
			return;
		}
		if (listen(sock, 300) != 0)
		{
			log->write("Listen failed");
			PostQuitMessage(0);
			return;
		}
		m_ConnectBtn.SetWindowText(_T("断开连接"));
		netState = Open;
	}
	else
	{
		closesocket(sock);
		m_ConnectBtn.SetWindowText(_T("网络连接"));
		netState = Close;
		m_IPCtl.ClearAddress();
		m_PortEdit.SetWindowText(_T(""));
	}
}

void CWebSocketServerDlg::OnBnClickedButton2() //发送消息
{
	// TODO: 在此添加控件通知处理程序代码
// 	HWND hWnd = ::FindWindow(NULL,_T("AAAAAAAAAAAAAAAAAAAAAAAAA"));
// 	if (hWnd)
// 	{
// 		MyStruct ms;
// 		strcpy(ms.msg,"123我是aaa");
// 		COPYDATASTRUCT cds;
// 		ZeroMemory(&cds, sizeof(cds));
// 		cds.dwData = IPC_TEST;
// 		cds.cbData = sizeof(ms);
// 		cds.lpData = &ms;
// 		::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
// 	}
	CString ss;
	m_SendEdit.GetWindowText(ss);
	m_SendEdit.SetWindowText("");

	if(ss == "") 
		ss = "Defult Message 123";
	std::string strHead = ss;
	char sendLen = (char)(strHead.length());
	strHead = sendLen + strHead;
	strHead = (char)0x81 + strHead;
	std::string str = strHead.c_str();
	send(client, str.c_str(), str.length(), 0);
}



LRESULT CWebSocketServerDlg::OnMySocket(WPARAM wParam, LPARAM lParam)
{
	SOCKET s = (SOCKET)wParam;
	int Len = sizeof(addr_client);
	CString strTip;
	DWORD reclen;
	char msgRcv[512] = { 0 };
	char msgShow[506] = { 0 };
	int i = sizeof(msgRcv);
	int j = sizeof(msgShow);
	if (WSAGETSELECTERROR(lParam))
	{
		closesocket(s);
		return 0;
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		client = accept(s, (sockaddr*)&addr_client, &Len);
		SetWindowText("__53__WebSocketServer__53__");
		if (m_TimerFlag == FALSE)
		{
			m_TimerFlag = TRUE;
			SetTimer(NewConnectTimer,400,NULL);
		}
		strTip.Format(_T("有%d个客户已经连接上"), ++Count);
		m_ListBox.ResetContent();
		m_ListBox.AddString(strTip);
		break;
	case FD_READ:
		memset(msgRcv, 0, sizeof(msgRcv));
		memset(msgShow, 0, sizeof(msgShow));
		/*
		*当 接收到的字节长度reclen超过128的时候，会引起后续解析乱码 
		*到时候需要客户端将数据分段发送然后由我们自己拼装
		*方案可以参考本地发送长json位置的代码
		*/


		reclen = recv(client, msgRcv, 512, 0);
		if (reclen > 7 && reclen <600)
		{
			std::string buffer(msgRcv);
			char *p = GetKey((char*)buffer.c_str());
			if (p == NULL)
			{

				for (DWORD i = 0; i != reclen - 6; ++i)
				{
					msgShow[i] = msgRcv[i % 4 + 2] ^ msgRcv[i + 6];
				}
				CString ss1;
				m_strMsgFromWeb = msgShow;

				m_strMsgFromWeb = U2G(msgShow);  //yangshg 2018/06/15 10:29:26 UTF8--->GB2312
				{//GetJson
					int pos = m_strMsgFromWeb.find_first_of('}');
					m_strMsgFromWeb = m_strMsgFromWeb.substr(0,pos + 1);
				}
				log->write("收到的内容msgShow:%s",m_strMsgFromWeb.c_str());
				m_RecvEdit.SetWindowText(CString(m_strMsgFromWeb.c_str()));
				if (m_strMsgFromWeb != "")
				{
					log->write("调用ProcessRecvMsg去处理非空串:m_strMsgFromWeb--%s",m_strMsgFromWeb.c_str());
					ProcessRecvMsg(m_strMsgFromWeb);
				}
				
// 				ss1 = ss1 + CString(U2G(msgShow));
// 				m_RecvEdit.SetWindowText(ss1);
				
				break;
			}
			std::string input(p);
			std::string result = DecodeKey(input);
			std::string handShakeResp = "HTTP/1.1 101 Switching Protocols\r\n";
			handShakeResp += "Upgrade: websocket\r\n";
			handShakeResp += "Connection: Upgrade\r\n";
			handShakeResp += "Sec-WebSocket-Accept:";
			handShakeResp += result;
			handShakeResp += "\r\n\r\n";
			send(client, handShakeResp.c_str(), handShakeResp.length(), 0);
		}
		break;
	case FD_WRITE:
		break;
	case FD_CLOSE:
		log->write("客户端断开了");
		closesocket(s);
		strTip.Format(_T("有%d个客户已经连接上"), --Count);
		m_ListBox.ResetContent();
		m_ListBox.AddString(strTip);
		break;
	case FD_CONNECT:
		{
		}
		break;
	default:
		break;
	}
	return 0;
}
// #if 0
// 	int i = 0;
// #else if 1
// 	int i = 0;
// #endif

char* CWebSocketServerDlg::GetKey(char* handShakeHeader)
{
	char* result = NULL;
	std::string sec_key = "Sec-WebSocket-Key";
	const char* delim = "\r\n";
	char *ret[20] = { NULL };
	char *p = NULL, *q = NULL;
	unsigned size = 0;
	p = strtok_s(handShakeHeader, delim, &q);

	while (p)
	{
		ret[size] = p;
		p = strtok_s(NULL, delim, &q);
		size++;
	}

	for (unsigned i = 0; i < size; ++i)
	{
		if (strstr(ret[i], sec_key.c_str()) != NULL)
		{
			//Sec-WebSocket-Key: NvxdeWLLsLXkt5DirLJ1yA==除去:和空格
			result = strchr(ret[i], ':');
			result += 2;
			break;
		}
	}

	return result;
}

std::string CWebSocketServerDlg::DecodeKey(std::string key)
{
	std::string secWebSocket;
	std::string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	key += guid;
	secWebSocket.empty();

	SHA1 sha;
	unsigned message_digest[5];
	sha.Reset();
	sha << key.c_str();
	sha.Result(message_digest);

	for (int i = 0; i < 5; ++i)
		message_digest[i] = htonl(message_digest[i]);

	secWebSocket = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
	return secWebSocket;
}


void CWebSocketServerDlg::ProcessRecvMsg(std::string & str)
{
	
	std::string strMsg = str;
	str = "";
	log->write("进入ProcessRecvMsg函数，参数%s:",strMsg.c_str());
	//解析并打印出来  
	Document document;
	document.Parse<0>(strMsg.c_str());
	if (document.IsObject())
	{
		ProcessJsonObjRequest(document);
	}
	else
	{
		log->write("json解析失败网页发送的内容失败");
	}
}

void CWebSocketServerDlg::ProcessJsonObjRequest(const rapidjson::Value& data)
{
	assert(data.IsObject());
	if (data.HasMember("cmd"))
	{
		static std::string phoneName;  //保存号码的名字
		static std::string phoneNum;   //保存号码的名字
		HWND hWnd = ::FindWindow(NULL, _T("53KF_KFCaaSClient_53KF"));
		std::string strCmd = data["cmd"].GetString();
		if ("callout_call" == strCmd)  //yangshg 2018/04/25 15:48:36外呼
		{
			if (data.HasMember("phoneNum"))
			{
// 				{//判断id6D是否一致
// 					string id6d = data["id6d"].GetString();
// 					if (id6d != m_strId6d)
// 					{
// 						//yangshg 2018/09/29 13:29:33
// 						log->write("用户ID：%s 与 登录ID不同.",id6d.c_str(),m_strId6d.c_str());
// 						return ;
// 					}
// 				}
				/******** yangshaoguang 2018/02/28 14:17:04 START ↓ ********/
				//作用 CRM未知原因会多次发送通话信息，再此只处理第一次
				static int ttmp = 0;
				time1 = CTime::GetCurrentTime();
				if (ttmp == 0)
				{
					ttmp++;
				}
				else
				{
					CTimeSpan timespan = time1 - time2;
					int nTSeconds = timespan.GetTotalSeconds();
					if (nTSeconds < 5)
					{
						return;
					}
				}
				time2 = time1;
				/******** yangshaoguang 2018/02/28 14:17:04  END  ↑ ********/
				Callout_Call info;
				ZeroMemory(&info, sizeof(info));
				info.m_strPhoneNum = data["phoneNum"].GetString();
				info.m_strPhoneNum = RemoveFirstZero(info.m_strPhoneNum);
				MyStruct mystruct;

				char contect[256] = {0};
				sprintf(mystruct.msg,"{\"cmd\":\"callout_call\",\"name\":\"%s\",\"phoneNum\":\"%s\",\"address\":\"%s\"}",info.m_strName.c_str(),info.m_strPhoneNum.c_str(),info.m_strAddress.c_str());

				COPYDATASTRUCT cds;
				ZeroMemory(&cds, sizeof(cds));
				cds.dwData = IPC_CALLSTATE;
				cds.cbData = sizeof(MyStruct);
				cds.lpData = &mystruct;
				theApp.m_zhuangtai = theApp_waihu; //标记为外呼
				log->write("进程间通讯:%s",mystruct.msg);
				::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
			}
		}
		else if ("hangup" == strCmd)  //yangshg 2018/04/25 15:48:47 挂断
		{
			std::string strID, strNumber, strType;
			if (data.HasMember("type") && data.HasMember("call_id") && data.HasMember("call_number"))
			{
				strID = data["call_id"].GetString();
				strNumber = data["call_number"].GetString();
				strType = data["type"].GetString();
			}

			MyStruct mystruct;
			sprintf(mystruct.msg, "{\"cmd\":\"hangup\",\"call_id\":\"%s\",\"call_number\":\"%s\",\"type\":\"%s\"}", strID.c_str(), strNumber.c_str(), strType.c_str());

			COPYDATASTRUCT cds;
			ZeroMemory(&cds, sizeof(cds));
			cds.dwData = IPC_CALLSTATE;
			cds.cbData = sizeof(MyStruct);
			cds.lpData = &mystruct;
			theApp.m_zhuangtai = theApp_waihu; //标记为外呼
			log->write("进程间通讯:%s",mystruct.msg);
			::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
		}
		else if ("callin_accept" == strCmd)//yangshg 2018/04/25 15:48:52 接听
		{
			std::string strID, strNumber;
			{
				MyStruct mystruct;
				sprintf(mystruct.msg, "{\"cmd\":\"callin_accept\"}");
				COPYDATASTRUCT cds;
				ZeroMemory(&cds, sizeof(cds));
				cds.dwData = IPC_CALLSTATE;
				cds.cbData = sizeof(mystruct);
				cds.lpData = &mystruct;
				theApp.m_zhuangtai = theApp_jietong; //标记为呼入接听
				log->write("进程间通讯:%s",mystruct.msg);
				::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
			}
		}
		else if ("callin_refuse" == strCmd) //yangshg 2018/04/25 15:48:59 拒接
		{
			std::string strID, strNumber;
			if (data.HasMember("call_id") && data.HasMember("call_number"))
			{
				strID = data["call_id"].GetString();
				strNumber = data["call_number"].GetString();

				MyStruct mystruct;
				sprintf(mystruct.msg, "{\"cmd\":\"callin_refuse\",\"call_id\":\"%s\",\"call_number\":\"%s\"}", strID.c_str(), strNumber.c_str());

				COPYDATASTRUCT cds;
				ZeroMemory(&cds, sizeof(cds));
				cds.dwData = IPC_CALLSTATE;
				cds.cbData = sizeof(mystruct);
				cds.lpData = &mystruct;
				theApp.m_zhuangtai = theApp_jujie; //标记为呼入拒接
				log->write("进程间通讯:%s",mystruct.msg);
				::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
			}

		}
		else if ("guest_name" == strCmd) //yangshg 2018/04/25 15:49:05 外呼姓名
		{
			phoneNum = data["phoneNum"].GetString();
			if (data.HasMember("names"))
			{
				const rapidjson::Value &tmp_names = data["names"];
				if (tmp_names.IsArray())
				{
					int i = 0;
					if (tmp_names.Size() > 0)
					{
						const rapidjson::Value &One = tmp_names[i];
						phoneName = tmp_names[i].GetString(); //yangshg 2018/02/26 11:06:05 获取第一个名字
					}
					else
					{
						phoneName = "来自CRM"; //yangshg 2018/02/26 11:06:05 获取第一个名字
					}
				}
			}
		}
		else
		{
			//yangshg 2018/04/26 13:33:50 忽略掉的消息
		}
	}
}

BOOL CWebSocketServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN &&
		(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
BOOL CWebSocketServerDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (pCopyDataStruct != NULL)
	{
		int nCmd = pCopyDataStruct->dwData;
		if (IPC_CALLSTATE == nCmd)
		{
			MyStruct mystruct;
			memcpy(&mystruct,pCopyDataStruct->lpData,sizeof(MyStruct));
			string ss = mystruct.msg;
			{//yangshg 2018/09/29 13:15:10  解析出来id6d
				Document document;  
				document.Parse<0>(ss.c_str()); 
				if(document.IsObject())
				{
					if (document.HasMember("id6d"))
					{
						m_strId6d = document["id6d"].GetString();
					}
				}
			}
			log->write("将要发送到客户端的数据：%s",ss.c_str());
//			send(client,ss.c_str(),ss.length(),0);
			SendToClient(ss);
		}
		else if (IPC_TEST == nCmd)
		{
// 			MyStruct mystruct;
// 			memcpy(&mystruct,pCopyDataStruct->lpData,sizeof(MyStruct));
// 			string ss = mystruct.msg;
// 			int i = 0;
		}
	}
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CWebSocketServerDlg::MySend(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
void CWebSocketServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == NewConnectTimer)
	{
		MySend(0,0);
		KillTimer(NewConnectTimer);
		m_TimerFlag = FALSE;
	}
	CDialog::OnTimer(nIDEvent);
}
#include "rapidjson/document.h"  
#include "rapidjson/filestream.h"  
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h" 
void CWebSocketServerDlg::SendToClient(string &strMsg)
{
	int nSendSuccess = 0;
	string Msg = strMsg;
	if (Msg.length() > 200)
	{//将json解出来一层再发出去
		Document document;  
		document.Parse<0>(Msg.c_str());  
		if(document.HasMember("cmd")
			&&document.HasMember("msg"))
		{
			string ss1 ;
			ss1 = document["msg"].GetString();
			Document node1;  
			node1.Parse<0>(ss1.c_str());
//			Value &node1=document["msg"];
			if(node1.HasMember("cmd"))
			{
				string strCmd = node1["cmd"].GetString();
				if (strCmd == "push") //需要发送的数据
				{
					Msg = ss1;
				}
				else if(strCmd == "callinfo")
				{
					return ;
				}
			}
		}
	}
	int len = Msg.length();

	if (len >=200)
	{
		{//将推送信息中的中文去掉
			string strtemp = strMsg;
			if (strtemp.find("history") != -1) //是历史记录的长json
			{
				int npos = strtemp.find("area");
				string ss1 = MyGetFirstNChar(strtemp, npos - 1);
				int len = strtemp.length();
				string ss2 = MyGetLastNChar(strtemp, len - npos);
				npos = ss2.find("},");
				int len2 = ss2.length();
				string ss3 = MyGetLastNChar(ss2,len2 - npos -3);
				Msg = ss1 + ss3;	
				{//去除notifyFromServer ，就是先将json串解开一层
					Document document;
					document.Parse<0>(Msg.c_str());
					if (document.IsObject())
					{
						if(document.HasMember("cmd")&&document.HasMember("msg"))
						{
							string strCmd = document["cmd"].GetString();
							if (strCmd == "notifyFromServer")
							{
								string Node1str = document["msg"].GetString();
								Msg = Node1str;
							}
						}
					}

				}
				log->write("要发送到websocket的去除地区的数据是:%s",Msg.c_str());
			}
		}
		{//发送 infostart
			std::string strHead = "infostart";
			char sendLen = (char)(strHead.length());
			strHead = sendLen + strHead;
			strHead = (char)0x81 + strHead;
			std::string str = strHead.c_str();
			nSendSuccess = send(client, str.c_str(), str.length(), 0);
			log->write("infostart");
		}
		{//发长JSON
// 			int len = Msg.length();
// 			for (int i = 0; i < len; i += 30)
// 			{
// 				string see = /*URLEncode1*/(Msg.substr(i, 30));
// 				int lensee = see.length();
// 				CString cs = CString(see.c_str());
// 				char *_temp = PackData(cs);
// 				std::string str = _temp;
// 				nSendSuccess = send(client, str.c_str(),str.length(), 0);
// 				log->write("共%d字符,第%d字符:%s,nSendSuccess = %d",len,i+1,see.c_str(),nSendSuccess);
// 			}

			int len = Msg.length();
			string strarr[32];
			int index = 0;
			for (int i = 0; i < len; i += 100,index++)
			{
				strarr[index] = Msg.substr(i, 100);
				//string see = /*URLEncode1*/(Msg.substr(i, 30));
			}
			for (int i = 0; i < 32;i++)
			{
				CString cs = CString(strarr[i].c_str());
				if (cs != _T(""))
				{
					char *_temp = PackData(cs);
					std::string str = _temp;
					nSendSuccess = send(client, str.c_str(),str.length(), 0);
					log->write("共%d字符串,第%d字符串:%s,nSendSuccess = %d",index+1,i+1,strarr[i].c_str(),nSendSuccess);
				}
			}
		}
		{ // infoend
			std::string strEnd = "infoend";
			char sendLen = (char)(strEnd.length());
			strEnd = sendLen + strEnd;
			strEnd = (char)0x81 + strEnd;
			std::string str = strEnd.c_str();
			nSendSuccess = send(client, str.c_str(), str.length(), 0);
			log->write("infoend");
		}
	}
	else
	{
		std::string strHead = Msg;
		char sendLen = (char)(strHead.length());
		strHead = sendLen + strHead;
		strHead = (char)0x81 + strHead;
		std::string str = strHead.c_str();
		nSendSuccess = send(client, str.c_str(), str.length(), 0);
	}
	if (nSendSuccess <= 0)
	{ //发送消息失败 {"cmd":"callout_refused"}
		std::string strID, strNumber;
		HWND hWnd = ::FindWindow(NULL, _T("53KF_KFCaaSClient_53KF"));
		if(hWnd && theApp.m_zhuangtai == theApp_huru)
		{
			MyStruct mystruct;
			sprintf(mystruct.msg, "{\"cmd\":\"callout_refused\"}");
			COPYDATASTRUCT cds;
			ZeroMemory(&cds, sizeof(cds));
			cds.dwData = IPC_CALLSTATE;
			cds.cbData = sizeof(mystruct);
			cds.lpData = &mystruct;
			theApp.m_zhuangtai = theApp_jujie; //标记为拒接
			log->write("呼入消息为发送成功，直接拒接来电");
			log->write("进程间通讯:%s",mystruct.msg);
			::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
		}
	}
}
