#include "pch.h"
#include "Globalptr.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD Globalptr(_In_ PBYTE Data, _In_ DWORD Size)
/*
�д����ԡ�


*/
{
    DWORD ret = ERROR_SUCCESS;

    if (!IsValidPE(Data, Size)) {
        return ret;
    }

    IMAGE_DATA_DIRECTORY DataDirectory = {0};
    GetDataDirectory(Data, Size, IMAGE_DIRECTORY_ENTRY_GLOBALPTR, &DataDirectory);

    if (0 == DataDirectory.VirtualAddress) {
        printf("���ļ�û��Globalptr.\r\n");
        return ret;
    }

    DebugBreak();

    ULONG size = 0;
    PIMAGE_SECTION_HEADER FoundHeader = NULL;
    LONG_PTR GlobalptrDirectory = (LONG_PTR)
        ImageDirectoryEntryToDataEx(Data,
                                    FALSE,//ӳ�䣨MapViewOfFile������FALSE��ԭʼ��ȡ(�磺ReadFile)����TRUE�� 
                                    IMAGE_DIRECTORY_ENTRY_GLOBALPTR,
                                    &size, &FoundHeader);

    printf("Architecture Directory Information:\r\n");
    printf("VirtualAddress:%#010X.\r\n", DataDirectory.VirtualAddress);
    printf("Size:%#010X.\r\n", DataDirectory.Size);
    printf("\r\n");





    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD Globalptr(_In_ LPCWSTR FileName)
{
    return MapFile(FileName, Globalptr);
}
