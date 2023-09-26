#include "pch.h"
#include "BoundImport.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD BoundImport(_In_ PBYTE Data, _In_ DWORD Size)
//�д����ԡ�
{
    DWORD ret = ERROR_SUCCESS;

    if (!IsValidPE(Data, Size)) {
        return ret;
    }

    IMAGE_DATA_DIRECTORY DataDirectory = {0};
    GetDataDirectory(Data, Size, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, &DataDirectory);

    if (0 == DataDirectory.VirtualAddress) {
        printf("���ļ�û��BoundImport.\r\n");
        return ret;
    }

    ULONG size = 0;
    PIMAGE_SECTION_HEADER FoundHeader = NULL;
    PIMAGE_BOUND_IMPORT_DESCRIPTOR BoundImportDirectory = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)
        ImageDirectoryEntryToDataEx(Data,
                                    FALSE,//ӳ�䣨MapViewOfFile������FALSE��ԭʼ��ȡ(�磺ReadFile)����TRUE�� 
                                    IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT,
                                    &size, &FoundHeader);

    printf("TLS Directory Information:\r\n");
    printf("VirtualAddress:%#010X.\r\n", DataDirectory.VirtualAddress);
    printf("Size:%#010X.\r\n", DataDirectory.Size);
    printf("\r\n");

    //�м���IMAGE_BOUND_IMPORT_DESCRIPTOR�ṹ�أ�

    CHAR TimeDateStamp[MAX_PATH] = {0};
    GetTimeDateStamp(BoundImportDirectory->TimeDateStamp, TimeDateStamp);
    printf("TimeDateStamp:%d(%#010X), ʱ�����%s.\r\n",
           BoundImportDirectory->TimeDateStamp,
           BoundImportDirectory->TimeDateStamp,
           TimeDateStamp);

    printf("OffsetModuleName:%#06X.\r\n", BoundImportDirectory->OffsetModuleName);//���Ӧ�����ַ�����

    BoundImportDirectory++;
    PIMAGE_BOUND_FORWARDER_REF BoundForwarderRef = (PIMAGE_BOUND_FORWARDER_REF)BoundImportDirectory;

    for (WORD i = 0; i < BoundImportDirectory->NumberOfModuleForwarderRefs; i++) {
        CHAR TimeDateStamp[MAX_PATH] = {0};
        GetTimeDateStamp(BoundForwarderRef->TimeDateStamp, TimeDateStamp);
        printf("\tTimeDateStamp:%d(%#010X), ʱ�����%s.\r\n",
               BoundForwarderRef->TimeDateStamp,
               BoundForwarderRef->TimeDateStamp,
               TimeDateStamp);

        printf("\tSize:%#06X.\r\n", BoundForwarderRef->OffsetModuleName);//���Ӧ�����ַ�����
        printf("\tReserved:%#06X.\r\n", BoundForwarderRef->Reserved);

        printf("\r\n");

        BoundForwarderRef++;
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD BoundImport(_In_ LPCWSTR FileName)
{
    return MapFile(FileName, BoundImport);
}
