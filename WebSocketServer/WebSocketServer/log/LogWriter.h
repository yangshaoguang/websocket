//LogWriter.h
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define LOG_BUFFER_SIZE 1024
class LogWriter
{
public:
	LogWriter(){}
	LogWriter(const char * filepath);
	~LogWriter(void);
	bool write(const char * format, ...);
private:
	static int preMark(char * buffer);
private:
	FILE * fp;
	char   m_buffer[LOG_BUFFER_SIZE];
};

