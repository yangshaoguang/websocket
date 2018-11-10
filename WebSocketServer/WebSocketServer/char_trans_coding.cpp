#include "char_trans_coding.h"
#include "stdafx.h"
#include <string.h>
// #include "mybase64.h"
// #include "SHA1ACT.h"
CString ShowChar(unsigned char * recvBuf,int radix,int len)
{
//	char * result=(char *)malloc(1024);
//	memset(result,0,1024);
	CString result;
	char  format[4];
	switch(radix)
	{
	case(10):
		memcpy(format,"%d ",4);
		break;
	case 16:
		memcpy(format,"%x ",4);
		break;
	}
	CString temp;
	for(int i=0;i<len;i++)
	{
		temp.Format(format,recvBuf[i]);
		result+=temp;
	}
	return result;
}
char * websocket_parity(CString code)//请在调用后free
{
	char *result=(char *)malloc(1024);
	memset(result,0,1024);
// 	int start,end;
// 	start=code.Find((TCHAR*)"Key: ");
// 	start+=5;
// 	code=code.Mid(start,24);
// 	code+="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
// 	unsigned long len=41;
// 	char *_result=BitSHA1(code);
// 	_result=gEncBase64(_result,20,&len);
// 	sprintf(result,"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",_result);
	return result;
}

char * AnalyticData(char * recBytes, int recByteLength)//请在调用后free掉返回值
        {
            if (recByteLength < 2)  return NULL; 

            bool fin = (recBytes[0] & 0x80) == 0x80; // 1bit，1表示最后一帧  
            if (!fin) return NULL;// 超过一帧暂不处理 
           

            bool mask_flag = (recBytes[1] & 0x80) == 0x80; // 是否包含掩码  
            if (!mask_flag) return NULL;

            int payload_len = recBytes[1] & 0x7F; // 数据长度  

            char * masks = (char  *)malloc(4);
            char * payload_data=(char *)malloc(1024);
			memset(payload_data,0,1024);
            if (payload_len == 126)
			{
				memcpy(masks,recBytes+4,4);
                payload_len = (unsigned short)(recBytes[2] << 8 | recBytes[3]);
            //    payload_data =(char *)malloc(payload_len);
                memcpy(payload_data,recBytes+8,payload_len);

            }
			else if (payload_len == 127)
			{
                memcpy(masks,recBytes+10,4);
                char * uInt64Bytes = (char *)malloc(8);
                for (int i = 0; i < 8; i++)
				{
                    uInt64Bytes[i] = recBytes[9 - i];
                }
                long long len = *(long long *)uInt64Bytes;

            //    payload_data = (char *)malloc(len);
                for (long long i = 0; i < len; i++)
				{
                    payload_data[i] = recBytes[i + 14];
                }
				free(uInt64Bytes);
            }
			else
			{
                memcpy(masks,recBytes+2,4);
             //   payload_data =(char *)malloc(payload_len);
                memcpy(payload_data,recBytes+6,payload_len);

            }

            for (int i = 0; i < payload_len; i++)
			{
                payload_data[i] = (byte)(payload_data[i] ^ masks[i % 4]);
            }
			free(masks);
            return payload_data;
        }

extern CDialog * MainWindow;

void ConvertGBKToUtf8(CString&amp,char * strGBK)
{
amp="";
int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0);
unsigned short * wszUtf8 = new unsigned short[len+1];
memset(wszUtf8, 0, len * 2 + 2);
MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, (LPWSTR)wszUtf8, len);
len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
char *szUtf8=new char[len + 1];
memset(szUtf8, 0, len + 1);
WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
amp = szUtf8;
delete[] szUtf8;
delete[] wszUtf8;
}


char * PackData(CString message)
        {
            unsigned char * contentBytes = NULL;
            CString temp;
			ConvertGBKToUtf8(temp,(char *)message.GetString());
			int len = temp.GetLength();
			if (temp.GetLength() < 126)
			{
				contentBytes =(unsigned char *)malloc(temp.GetLength()+2);
                contentBytes[0] = 0x81;
				contentBytes[1] = (unsigned char)temp.GetLength();
				strcpy((char *)contentBytes+2,temp.GetString());
			}
			else if (temp.GetLength() > 125 && temp.GetLength() < 0xFFFF)
			{
				contentBytes = (unsigned char *)malloc(temp.GetLength()+4);
                contentBytes[0] = (unsigned char)0x81;
                contentBytes[1] =(unsigned char) 126;
				contentBytes[2] = (unsigned char)((unsigned char)temp.GetLength() & 0xFF);
				contentBytes[3] = (unsigned char)((unsigned char)temp.GetLength() >> 8 & 0xFF);
				strcpy((char *)contentBytes+4,temp.GetString());
            }else{
                // 暂不处理超长内容  
            }

            return (char *)contentBytes;
        }  


void ConvertUtf8ToGBK(CString&amp, char *strUtf8) 
{
int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0);
unsigned short * wszGBK = new unsigned short[len+1];
memset(wszGBK, 0, len * 2 + 2);
MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, (LPWSTR)wszGBK, len);
len = WideCharToMultiByte(CP_ACP, 0,(LPWSTR) wszGBK, -1, NULL, 0, NULL, NULL);
char *szGBK=new char[len + 1];
memset(szGBK, 0, len + 1);
WideCharToMultiByte (CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL,NULL);
amp = szGBK;
delete[] szGBK;
delete[] wszGBK;
}
void Purify(char * recvBuf)
{
	for(int i=0;i<1024;i++)
	if((unsigned char)recvBuf[i]==0xfd)
	{
		recvBuf[i]=0;
		return ;
	}
}

