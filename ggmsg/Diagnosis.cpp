#include "stdafx.h"
#include "Diagnosis.h"


Diagnosis::Diagnosis()
{
}


Diagnosis::~Diagnosis()
{
}

void Diagnosis::Trace(_In_z_ _Printf_format_string_ LPCWSTR pszFormat, ...)
{
	TCHAR msg[1024] = { 0 };
	va_list args1;
	va_start(args1, pszFormat);
	_vstprintf_s(msg, pszFormat, args1);
	OutputDebugString(msg);
}
