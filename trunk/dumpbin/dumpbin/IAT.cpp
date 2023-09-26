#include "pch.h"
#include "IAT.h"
#include "Public.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD IAT(_In_ PBYTE Data, _In_ DWORD Size)
{
    DWORD ret = ERROR_SUCCESS;

    if (!IsValidPE(Data, Size)) {
        return ret;
    }

    IMAGE_DATA_DIRECTORY DataDirectory = {0};
    GetDataDirectory(Data, Size, IMAGE_DIRECTORY_ENTRY_IAT, &DataDirectory);

    if (0 == DataDirectory.VirtualAddress) {
        printf("���ļ�û��IAT.\r\n");
        return ret;
    }

    ULONG size = 0;
    PIMAGE_SECTION_HEADER FoundHeader = NULL;
    PULONG_PTR IATBase = (PULONG_PTR)
        ImageDirectoryEntryToDataEx(Data,
                                    FALSE,//ӳ�䣨MapViewOfFile������FALSE��ԭʼ��ȡ(�磺ReadFile)����TRUE�� 
                                    IMAGE_DIRECTORY_ENTRY_IAT,
                                    &size, &FoundHeader);

    PIMAGE_NT_HEADERS NtHeaders = ImageNtHeader(Data);
    _ASSERTE(NtHeaders);

    printf("IAT Directory Information:\r\n");

    printf("IAT Directory Numbers:%zd.\r\n", DataDirectory.Size/ sizeof(ULONG));

    //��Щ���ݵ������д���һ���Ŀ��졣

    for (DWORD i = 0; i * sizeof(ULONG) < DataDirectory.Size; i++) {
        ULONG_PTR ImportThunk = IATBase[i];

        printf("ImportThunk:%zd.\r\n", ImportThunk);

        i++;//����һ��DWORD��0.
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD IAT(_In_ LPCWSTR FileName)
{
    return MapFile(FileName, IAT);
}
