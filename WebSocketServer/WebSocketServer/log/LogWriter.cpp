//LogWriter.cpp
#include "stdafx.h"
#include "LogWriter.h"
#include <assert.h>
#include <share.h>
#include <stdarg.h>
#include <time.h>

LogWriter::LogWriter(const char * filepath)
{
	fp = _fsopen(filepath, "a+", _SH_DENYNO);
	if (fp == NULL)
	{
		int i = 0;
	}
	assert(fp != NULL);
}

LogWriter::~LogWriter(void)
{
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
}

bool LogWriter::write(const char * format, ...)
{
	if (fp == NULL)
		return false;

	int nsize = 0;
	int prelen = 0;
	char * str = m_buffer;
	prelen = preMark(str);
	str += prelen;

	va_list args;
	va_start(args, format);
	nsize = _vsnprintf(str, LOG_BUFFER_SIZE - prelen, format, args);
	va_end(args);

	fprintf(fp, "%s\n", m_buffer);
	fflush(fp);
	return true;
}

int LogWriter::preMark(char * buffer)
{
	time_t now;
	now = time(&now);
	struct tm vtm = *localtime(&now);
	return _snprintf(buffer, LOG_BUFFER_SIZE, "%04d-%02d-%02d %02d:%02d:%02d ",
		vtm.tm_year + 1900, vtm.tm_mon, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
}
