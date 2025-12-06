#pragma once
#include <atltrace.h>

class Diagnosis
{
public:
	Diagnosis();
	~Diagnosis();

	static void Trace(_In_z_ _Printf_format_string_ LPCWSTR pszFormat, ...);
};

#ifdef _DEBUG
#define DiagnosisTrace Diagnosis::Trace
#else
#define DiagnosisTrace __noop
#endif // DEBUG
//TRACE()