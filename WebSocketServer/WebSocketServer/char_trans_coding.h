#pragma once
#include "stdafx.h"
char * websocket_parity(CString code);
char * standardize(char * recvBuf);
//BOOL achr2wchr( const char *src, wchar_t *dst, int dst_len );
//BOOL wch2ach( const wchar_t* src, char *dst, int dst_len );
//bool MBToUTF8(char* pu8, const char* pmb, int u_len);
//bool UTF8ToMB(char* pmb, const char* pu8, int b_len);
char * AnalyticData(char * recBytes, int recByteLength);
char * PackData(CString message);
void ConvertGBKToUtf8(CString&amp,char * strGBK);
void ConvertUtf8ToGBK(CString&amp, char *strUtf8);
void Purify(char * recvBuf);
CString ShowChar(unsigned char * recvBuf,int radix,int len);