#include "pch.h"
#include "log.h"

#pragma warning(disable:26812)


/*
��LOG_LEVEL��Ӧ�������١�
����δ�����Ƿ�ֹԽ�硣
*/
const wchar_t* g_log_level_w[MAX_LEVEL + 1] = {
    L"������Ϣ��",
    L"������Ϣ��",
    L"��Ҫ��Ϣ��",
    L"��ͨ��Ϣ��",
    L"�߳���Ϣ��",
    L"������Ϣ��",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺",
    L"δ���壺"
};


CRITICAL_SECTION g_log_cs;//ͬ����־�ļ��Ķ���


ULONG g_log_level = DEFAULT_LOG_LEVEL;//��־���أ��������ļ����ơ�


//////////////////////////////////////////////////////////////////////////////////////////////////


void LogA(IN LOG_LEVEL Level, IN char const * Format, ...)
{
    if (!BitTest((const LONG*)&g_log_level, Level)) {
        return;
    }

    if (Level >= MAX_LEVEL) {
        return;
    }

    setlocale(0, "chs");//֧��д���֡�

    EnterCriticalSection(&g_log_cs);

    va_list args;
    va_start(args, Format);

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t time[MAX_PATH] = {0};//��ʽ��2016-07-11 17:35:54 
    int written = wsprintfW(time, L"%04d-%02d-%02d %02d:%02d:%02d:%03d\t",
                            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    written = printf("%ls", time);

#pragma prefast(push)
#pragma prefast(disable: 33010, "warning C33010: Unchecked lower bound for enum Level used as index..")
    written = printf("%ls", g_log_level_w[Level]);
#pragma prefast(pop)       

    written = vprintf(Format, args);

    va_end(args);

    LeaveCriticalSection(&g_log_cs);
}


void LogW(IN LOG_LEVEL Level, IN wchar_t const * Format, ...)
{


}


void LogApiErrMsg(PCSTR Api)
/*
���ܣ�ר�����ڼ�¼API����ʧ�ܵ���Ϣ��

�����ж���
1.����APIʧ��ԭ�����ϸ�������о��÷��е��Ť��
2.֧�ֲ���������
3.
*/
{
    LPWSTR lpvMessageBuffer;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPWSTR)&lpvMessageBuffer,//�ر�ע�⣺���ݺ��лس����У����һ����������ݡ�
                  0,
                  NULL);

    //ȥ���س�����
    int x = lstrlenW((LPWSTR)lpvMessageBuffer);
    lpvMessageBuffer[x - 1] = 0;
    lpvMessageBuffer[x - 2] = 0;

    LOGA(ERROR_LEVEL, "API:%s, LastError:%#x, Message:%ls", Api, GetLastError(), lpvMessageBuffer);

    LocalFree(lpvMessageBuffer);
}
