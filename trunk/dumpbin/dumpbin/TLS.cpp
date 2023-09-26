#include "pch.h"
#include "TLS.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD TLS(_In_ PBYTE Data, _In_ DWORD Size)
//�д����ԡ�
{
    DWORD ret = ERROR_SUCCESS;

    if (!IsValidPE(Data, Size)) {
        return ret;
    }

    IMAGE_DATA_DIRECTORY DataDirectory = {0};
    GetDataDirectory(Data, Size, IMAGE_DIRECTORY_ENTRY_TLS, &DataDirectory);

    if (0 == DataDirectory.VirtualAddress) {
        printf("���ļ�û��TLS.\r\n");
        return ret;
    }

    ULONG size = 0;
    PIMAGE_SECTION_HEADER FoundHeader = NULL;
    PIMAGE_TLS_DIRECTORY TLSDirectory = (PIMAGE_TLS_DIRECTORY)
        ImageDirectoryEntryToDataEx(Data,
                                    FALSE,//ӳ�䣨MapViewOfFile������FALSE��ԭʼ��ȡ(�磺ReadFile)����TRUE�� 
                                    IMAGE_DIRECTORY_ENTRY_TLS,
                                    &size, &FoundHeader);

    printf("TLS Directory Information:\r\n");
    printf("VirtualAddress:%#010X.\r\n", DataDirectory.VirtualAddress);
    printf("Size:%#010X.\r\n", DataDirectory.Size);
    printf("\r\n");

    //�м���IMAGE_TLS_DIRECTORY�ṹ�أ�

    if (IsPE32Ex(Data, Size)) {
        PIMAGE_TLS_DIRECTORY64 TLSDirectory64 = (PIMAGE_TLS_DIRECTORY64)TLSDirectory;

        printf("StartAddressOfRawData:%#016llX.\r\n", TLSDirectory64->StartAddressOfRawData);
        printf("EndAddressOfRawData:%#016llX.\r\n", TLSDirectory64->EndAddressOfRawData);
        printf("AddressOfIndex:%#016llX.\r\n", TLSDirectory64->AddressOfIndex);
        printf("AddressOfCallBacks:%#016llX.\r\n", TLSDirectory64->AddressOfCallBacks);

        PIMAGE_TLS_CALLBACK * TlsCallback = (PIMAGE_TLS_CALLBACK *)TLSDirectory64->AddressOfCallBacks;
        //�ص������ĵ�ַ������Ҳ��ô�ӡ������

        printf("SizeOfZeroFill:%#010X.\r\n", TLSDirectory64->SizeOfZeroFill);

        printf("ReserCharacteristicsved0:%#010X.\r\n", TLSDirectory64->Characteristics);

        printf("Reserved0:%#010X.\r\n", TLSDirectory64->Reserved0);
        printf("Alignment:%#010X.\r\n", TLSDirectory64->Alignment);
        printf("Reserved1:%#010X.\r\n", TLSDirectory64->Reserved1);
    } else {
        PIMAGE_TLS_DIRECTORY32 TLSDirectory32 = (PIMAGE_TLS_DIRECTORY32)TLSDirectory;

        printf("StartAddressOfRawData:%#010X.\r\n", TLSDirectory32->StartAddressOfRawData);
        printf("EndAddressOfRawData:%#010X.\r\n", TLSDirectory32->EndAddressOfRawData);
        printf("AddressOfIndex:%#010X.\r\n", TLSDirectory32->AddressOfIndex);
        printf("AddressOfCallBacks:%#010X.\r\n", TLSDirectory32->AddressOfCallBacks);

        PIMAGE_TLS_CALLBACK * TlsCallback = (PIMAGE_TLS_CALLBACK *)ULongToHandle(TLSDirectory32->AddressOfCallBacks);
        //�ص������ĵ�ַ������Ҳ��ô�ӡ������

        printf("SizeOfZeroFill:%#010X.\r\n", TLSDirectory32->SizeOfZeroFill);

        printf("ReserCharacteristicsved0:%#010X.\r\n", TLSDirectory32->Characteristics);

        printf("Reserved0:%#010X.\r\n", TLSDirectory32->Reserved0);
        printf("Alignment:%#010X.\r\n", TLSDirectory32->Alignment);
        printf("Reserved1:%#010X.\r\n", TLSDirectory32->Reserved1);
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD TLS(_In_ LPCWSTR FileName)
{
    return MapFile(FileName, TLS);
}
