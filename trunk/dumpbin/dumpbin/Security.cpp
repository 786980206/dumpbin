#include "pch.h"
#include "Security.h"
#include "Public.h"
#include "openssl.h"
#include "Public.h"


#pragma warning(disable:6386)
#pragma warning(disable:6387)


//////////////////////////////////////////////////////////////////////////////////////////////////


PCSTR GetCertificateType(_In_ WORD CertificateType)
{
    PCSTR CertificateTypeStr = NULL;

    switch (CertificateType) {
    case WIN_CERT_TYPE_X509:
        CertificateTypeStr = "X509";
        break;
    case WIN_CERT_TYPE_PKCS_SIGNED_DATA:
        CertificateTypeStr = "PKCS_SIGNED_DATA";
        break;
    case WIN_CERT_TYPE_RESERVED_1:
        CertificateTypeStr = "RESERVED_1";
        break;
    case WIN_CERT_TYPE_TS_STACK_SIGNED:
        CertificateTypeStr = "TS_STACK_SIGNED";
        break;
    default:
        CertificateTypeStr = "δ֪";
        break;
    }

    return CertificateTypeStr;
}


PCSTR GetCertRevision(_In_ WORD wRevision)
{
    PCSTR string = NULL;

    switch (wRevision) {
    case WIN_CERT_REVISION_1_0:
        string = "1_0";
        break;
    case WIN_CERT_REVISION_2_0:
        string = "2_0";
        break;
    default:
        string = "δ֪";
        break;
    }

    return string;
}


BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext)
{
    BOOL fReturn = FALSE;
    LPTSTR szName = NULL;
    DWORD dwData;

    __try {
        //�ɿ�����openssl�ķ�ʽ����pCertContext->cbCertEncoded + pCertContext->pbCertEncoded��

        _tprintf(_T("CertEncodingType:%d.\n"), pCertContext->dwCertEncodingType);

        _tprintf(_T("Version:%d.\n"), pCertContext->pCertInfo->dwVersion + 1);

        _tprintf(_T("ObjId:%hs.\n"), pCertContext->pCertInfo->SignatureAlgorithm.pszObjId);

        _tprintf(_T("Parameters: "));
        dwData = pCertContext->pCertInfo->SignatureAlgorithm.Parameters.cbData;
        for (DWORD n = 0; n < dwData; n++) {
            _tprintf(_T("%02x "), pCertContext->pCertInfo->SignatureAlgorithm.Parameters.pbData[n]);
        }
        _tprintf(_T("\n"));

        // Print Serial Number.
        _tprintf(_T("Serial Number: "));
        dwData = pCertContext->pCertInfo->SerialNumber.cbData;
        for (DWORD n = 0; n < dwData; n++) {
            _tprintf(_T("%02x "), pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
        }
        _tprintf(_T("\n"));

        char NotBefore[MAX_PATH] = {0};
        FileTimeToLocalTimeA(&pCertContext->pCertInfo->NotBefore, NotBefore);
        printf("��Ч�ڴ�:%s\t", NotBefore);

        char NotAfter[MAX_PATH] = {0};
        FileTimeToLocalTimeA(&pCertContext->pCertInfo->NotAfter, NotAfter);
        printf("��:%s\n", NotAfter);

        _tprintf(_T("SubjectPublicKey Algorithm ObjId:%hs.\n"),
                 pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId);

        _tprintf(_T("SubjectPublicKey Algorithm Parameters: "));
        dwData = pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.cbData;
        for (DWORD n = 0; n < dwData; n++) {
            _tprintf(_T("%02x "),
                     pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.pbData[dwData - (n + 1)]);
        }
        _tprintf(_T("\n"));

        //pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData
        //pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData
        //pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey.cUnusedBits

        //����IssuerUniqueId��SubjectUniqueId��cExtension����Ϣ��

        _tprintf(_T("cExtension:%d.\n"), pCertContext->pCertInfo->cExtension);
        _tprintf(_T("Extension ObjId:%hs.\n"), pCertContext->pCertInfo->rgExtension->pszObjId);
        _tprintf(_T("Extension fCritical:%d.\n"), pCertContext->pCertInfo->rgExtension->fCritical);
        _tprintf(_T("Extension Value: "));
        dwData = pCertContext->pCertInfo->rgExtension->Value.cbData;
        for (DWORD n = 0; n < dwData; n++) {
            _tprintf(_T("%02x "), pCertContext->pCertInfo->rgExtension->Value.pbData[n]);
        }
        _tprintf(_T("\n"));

        // Get Issuer name size.
        if (!(dwData = CertGetNameString(pCertContext,
                                         CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                         CERT_NAME_ISSUER_FLAG,
                                         NULL,
                                         NULL,
                                         0))) {
            _tprintf(_T("CertGetNameString failed.\n"));
            __leave;
        }

        // Allocate memory for Issuer name.
        szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
        if (!szName) {
            _tprintf(_T("Unable to allocate memory for issuer name.\n"));
            __leave;
        }

        // Get Issuer name.
        if (!(CertGetNameString(pCertContext,
                                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                CERT_NAME_ISSUER_FLAG,
                                NULL,
                                szName,
                                dwData))) {
            _tprintf(_T("CertGetNameString failed.\n"));
            __leave;
        }

        // print Issuer name.
        _tprintf(_T("Issuer Name: %s\n"), szName);
        LocalFree(szName);
        szName = NULL;

        // Get Subject name size.
        if (!(dwData = CertGetNameString(pCertContext,
                                         CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                         0,
                                         NULL,
                                         NULL,
                                         0))) {
            _tprintf(_T("CertGetNameString failed.\n"));
            __leave;
        }

        // Allocate memory for subject name.
        szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
        if (!szName) {
            _tprintf(_T("Unable to allocate memory for subject name.\n"));
            __leave;
        }

        // Get subject name.
        if (!(CertGetNameString(pCertContext,
                                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                0,
                                NULL,
                                szName,
                                dwData))) {
            _tprintf(_T("CertGetNameString failed.\n"));
            __leave;
        }

        // Print Subject Name.
        _tprintf(_T("Subject Name: %s\n"), szName);

        DWORD dwStrType = CERT_X500_NAME_STR;
        DWORD dwCount = CertGetNameString(pCertContext,
                                          CERT_NAME_RDN_TYPE,
                                          0,
                                          &dwStrType,
                                          NULL,
                                          0);
        if (dwCount) {
            LPTSTR szSubjectRDN = (LPTSTR)LocalAlloc(0, dwCount * sizeof(TCHAR));
            dwCount = CertGetNameString(pCertContext,
                                        CERT_NAME_RDN_TYPE,
                                        0,
                                        &dwStrType,
                                        szSubjectRDN,
                                        dwCount);
            if (dwCount) {
                _tprintf(_T("Certificate Subject = %s\n"), szSubjectRDN);
            }

            LocalFree(szSubjectRDN);
        }

        fReturn = TRUE;
    } __finally {
        if (szName != NULL)
            LocalFree(szName);
    }

    return fReturn;
}


void DecodeCertificate(PBYTE Certificate, DWORD Length)
/*
���ܣ���CryptDecodeObjectEx����PKCS#7 SignedData��ASN1�ṹ��
*/
{
    //  Get the length needed for the decoded buffer.
    DWORD cbDecoded = NULL;
    if (CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                            PKCS_CONTENT_INFO,//X509_NAME
                            Certificate,     // the buffer to be decoded
                            Length,
                            CRYPT_DECODE_NOCOPY_FLAG,
                            NULL,
                            NULL,
                            &cbDecoded)) {
        //printf("The needed buffer length is %d\n", cbDecoded);
    } else {
        _ASSERTE(false);
    }

    // Allocate memory for the decoded information.
    BYTE * pbDecoded;
    if (!(pbDecoded = (BYTE *)malloc(cbDecoded))) {
        _ASSERTE(false);
    }

    // Decode the encoded buffer.
    if (CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                            PKCS_CONTENT_INFO,//X509_NAME
                            Certificate,     // the buffer to be decoded
                            Length,
                            CRYPT_DECODE_NOCOPY_FLAG,
                            NULL,
                            pbDecoded,
                            &cbDecoded)) {
        CRYPT_CONTENT_INFO * content_info = (CRYPT_CONTENT_INFO *)pbDecoded;
        if (content_info) {
            printf("ObjId:%s\n", content_info->pszObjId);

            //WCHAR szSubject[1024] = {0};//d2i_X509_NAME  +  X509_NAME_oneline
            //DWORD cbSize = CertNameToStr(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            //                             &content_info->Content,
            //                             CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG,
            //                             szSubject,
            //                             sizeof(szSubject));
            //if (cbSize > 1) {//  If it returns one, the name is an empty string.
            //    ///printf("szSubject��%ls\n", szSubject);//����Ϊ�ա�
            //} else {
            //    _ASSERTE(false);
            //}

            HCERTSTORE CertStore = NULL;
            HCRYPTMSG Msg = NULL;
            CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &content_info->Content,
                             CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED,
                             CERT_QUERY_FORMAT_FLAG_BINARY,
                             0,
                             NULL,
                             NULL,
                             NULL,
                             &CertStore,
                             &Msg,
                             NULL);

            PCCERT_CONTEXT PrevCertContext = NULL;
            while ((PrevCertContext = CertEnumCertificatesInStore(CertStore, PrevCertContext)) != NULL) {
                printf("\n");
                PrintCertificateInfo(PrevCertContext);
            }
        } else {
            _ASSERTE(false);
        }
    } else {
        _ASSERTE(false);
    }
}


void PrintSecurity(LPWIN_CERTIFICATE SecurityDirectory)
{
    printf("Length:%d.\r\n", SecurityDirectory->dwLength);
    printf("Revision:%d(%s).\r\n",
           SecurityDirectory->wRevision,
           GetCertRevision(SecurityDirectory->wRevision));
    printf("CertificateType:%d(%s).\r\n",
           SecurityDirectory->wCertificateType,
           GetCertificateType(SecurityDirectory->wCertificateType));

    switch (SecurityDirectory->wCertificateType) {
    case WIN_CERT_TYPE_X509://bCertificate �������� X.509 ֤��

        break;
    case WIN_CERT_TYPE_PKCS_SIGNED_DATA://bCertificate �������� PKCS#7 SignedData �ṹ
    {
        //���������ɶ�ṹ�أ�
        //��ǰ�ľ�����������utf8���롣
        PBYTE Certificate = SecurityDirectory->bCertificate;

        /*
        ��������ݿ���CryptDecodeObjectEx��������
        ��Ӧ����ASN��ʽ�ġ�
        ע�⣺���ݽṹCERT_ALT_NAME_ENTRY��

        ��������ݿ�����openssl�ĺ����������磺d2i_PKCS7�ȡ�
        �ο���https://github.com/ajkhoury/CertDump.git
        */

        DecodeCertificate(Certificate, SecurityDirectory->dwLength);

        break;
    }
    case WIN_CERT_TYPE_RESERVED_1://������ 

        break;
    case WIN_CERT_TYPE_TS_STACK_SIGNED://�ն˷�����Э��ջ֤��ǩ��

        break;
    default:

        break;
    }

    printf("\r\n");
}


BOOL WINAPI DigestFunction(DIGEST_HANDLE refdata, PBYTE pData, DWORD dwLength)
//����ᱻ���ö�Ρ�
{

    return true;
}


BOOL VerifyEmbeddedSignature(IN LPCTSTR filename, OUT wchar_t * signer_file)
{
    HCATADMIN cat_admin_handle = NULL;
    if (!CryptCATAdminAcquireContext(&cat_admin_handle, NULL, 0)) {
        printf("FileName:%ls, GetLastError:%#x", filename, GetLastError());
        return FALSE;
    }

    HANDLE hFile = CreateFileW(filename, 
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               NULL, 
                               OPEN_EXISTING,
                               0, 
                               NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        printf("FileName:%ls, GetLastError:%#x", filename, GetLastError());
        CryptCATAdminReleaseContext(cat_admin_handle, 0);
        return FALSE;
    }

    DWORD hash_count = 100;
    BYTE hash_data[100];
    CryptCATAdminCalcHashFromFileHandle(hFile, &hash_count, hash_data, 0);
    CloseHandle(hFile);

    wstring member_tag;
    for (DWORD dw = 0; dw < hash_count; dw++) {
        member_tag += L"0123456789ABCDEF"[(hash_data[dw] >> 4) & 0x0f];
        member_tag += L"0123456789ABCDEF"[hash_data[dw] & 0x0f];
    }

    WINTRUST_DATA wd = {0};
    HCATINFO cat_admin_info = CryptCATAdminEnumCatalogFromHash(cat_admin_handle,
                                                               hash_data,
                                                               hash_count,
                                                               0,
                                                               NULL);
    if (NULL == cat_admin_info) {
        WINTRUST_FILE_INFO wfi = {0};
        wfi.cbStruct = sizeof(WINTRUST_FILE_INFO);
        wfi.pcwszFilePath = filename;

        wd.cbStruct = sizeof(WINTRUST_DATA);
        wd.dwUnionChoice = WTD_CHOICE_FILE;
        wd.pFile = &wfi;
        wd.dwUIChoice = WTD_UI_NONE;
        wd.dwProvFlags = WTD_SAFER_FLAG;

        lstrcpy(signer_file, filename);
    } else {
        CATALOG_INFO ci = {0};
        CryptCATCatalogInfoFromContext(cat_admin_info, &ci, 0);

        WINTRUST_CATALOG_INFO wci = {0};
        wci.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
        wci.pcwszCatalogFilePath = ci.wszCatalogFile;
        wci.pcwszMemberFilePath = filename;
        wci.pcwszMemberTag = member_tag.c_str();
        wci.pbCalculatedFileHash = hash_data;
        wci.cbCalculatedFileHash = hash_count;

        wd.cbStruct = sizeof(WINTRUST_DATA);
        wd.dwUnionChoice = WTD_CHOICE_CATALOG;
        wd.pCatalog = &wci;
        wd.dwUIChoice = WTD_UI_NONE;
        wd.fdwRevocationChecks = WTD_REVOKE_WHOLECHAIN;

        lstrcpy(signer_file, ci.wszCatalogFile);
    }

    GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    HRESULT hr = WinVerifyTrust(NULL, &action, &wd);
    Sleep(1);
    BOOL retval = SUCCEEDED(hr);

    if (NULL != cat_admin_info) {
        CryptCATAdminReleaseCatalogContext(cat_admin_handle, cat_admin_info, 0);
    }
    CryptCATAdminReleaseContext(cat_admin_handle, 0);
    return retval;
}


BOOL GetSignerInfo(IN WCHAR * FileName)
{
    HCERTSTORE hStore = NULL;
    HCRYPTMSG hMsg = NULL;
    PCCERT_CONTEXT pCertContext = NULL;
    BOOL fResult = FALSE;
    PCMSG_SIGNER_INFO pSignerInfo = NULL;

    __try {
        // Get message handle and store handle from the signed file.
        DWORD dwEncoding, dwContentType, dwFormatType;
        fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
                                   FileName,
                                   CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
                                   CERT_QUERY_FORMAT_FLAG_BINARY,
                                   0,
                                   &dwEncoding,
                                   &dwContentType,
                                   &dwFormatType,
                                   &hStore,
                                   &hMsg,
                                   NULL);
        if (!fResult) {
            printf("FileName:%ls, GetLastError:%#x", FileName, GetLastError());
            __leave;
        }

        // Get signer information size.
        DWORD dwSignerInfo = NULL;
        fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
        if (!fResult) {
            printf("FileName:%ls, GetLastError:%#x", FileName, GetLastError());
            __leave;
        }

        // Allocate memory for signer information.
        pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
        if (!pSignerInfo) {
            printf("FileName:%ls, GetLastError:%#x", FileName, GetLastError());
            __leave;
        }

        // Get Signer Information.
        fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
        if (!fResult) {
            printf("FileName:%ls, GetLastError:%#x", FileName, GetLastError());
            __leave;
        }

        // Search for the signer certificate in the temporary certificate store.
        CERT_INFO CertInfo;
        CertInfo.Issuer = pSignerInfo->Issuer;
        CertInfo.SerialNumber = pSignerInfo->SerialNumber;
        pCertContext = CertFindCertificateInStore(hStore,
                                                  (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING),
                                                  0,
                                                  CERT_FIND_SUBJECT_CERT,
                                                  (PVOID)&CertInfo,
                                                  NULL);
        if (!pCertContext) {
            printf("FileName:%ls, GetLastError:%#x", FileName, GetLastError());
            fResult = FALSE;
            __leave;
        }

        PrintCertificateInfo(pCertContext);

        fResult = TRUE;
    } __finally {
        if (pSignerInfo != NULL)
            LocalFree(pSignerInfo);

        if (pCertContext != NULL)
            CertFreeCertificateContext(pCertContext);

        if (hStore != NULL)
            CertCloseStore(hStore, 0);

        if (hMsg != NULL)
            CryptMsgClose(hMsg);
    }

    return fResult;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void ParseCertificateInfo1()
{
    int Args;
    LPWSTR * Arglist = CommandLineToArgvW(GetCommandLineW(), &Args);
    LPCWSTR FileName = Arglist[2];

    wchar_t signer_file[MAX_PATH] = {0};
    if (VerifyEmbeddedSignature(FileName, signer_file)) {
        printf("%ls��ǩ����Ϣ���ڵ��ļ���:%ls\n", FileName, signer_file);
        if (GetSignerInfo(signer_file)) {
            printf("%ls���кϷ���ǩ��\n", FileName);
        }
    }

    LocalFree(Arglist);
}


void ParseCertificateInfo2()
/*
��ϵͳ��API������֤�����Ϣ���Ա���Լ������ĶԱȡ�

���ַ�ʽû�з���PE�ļ���DataDirectory��
*/
{
    int Args;
    LPWSTR * Arglist = CommandLineToArgvW(GetCommandLineW(), &Args);

    LPCWSTR FileName = Arglist[2];

    HANDLE hfile = INVALID_HANDLE_VALUE;
    LPWIN_CERTIFICATE buffer = NULL;

    __try {
        hfile = CreateFile(FileName,
                           FILE_READ_DATA,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
        if (hfile == INVALID_HANDLE_VALUE) {
            int x = GetLastError();
            __leave;
        }

        DWORD CertificateCount = 0;
        DWORD Indices[9] = {0};
        DWORD  IndexCount = ARRAYSIZE(Indices);
        BOOL ret = ImageEnumerateCertificates(hfile,
                                              CERT_SECTION_TYPE_ANY,
                                              &CertificateCount,
                                              Indices,
                                              IndexCount);
        if (!ret) {
            int x = GetLastError();
            __leave;
        }

        for (DWORD i = 0; i < CertificateCount; i++) {
            WIN_CERTIFICATE Certificateheader = {0};
            ret = ImageGetCertificateHeader(hfile, i, &Certificateheader);
            if (!ret) {
                int x = GetLastError();
            }

            WIN_CERTIFICATE Certificate = {0};
            DWORD RequiredLength = sizeof(WIN_CERTIFICATE);
            ret = ImageGetCertificateData(hfile, i, &Certificate, &RequiredLength);
            if (!ret) {
                int x = GetLastError();
            }

            buffer = (LPWIN_CERTIFICATE)HeapAlloc(GetProcessHeap(), 0, RequiredLength);
            _ASSERTE(buffer);

            ret = ImageGetCertificateData(hfile, i, buffer, &RequiredLength);
            _ASSERTE(ret);

            CRYPT_DATA_BLOB p7Data;
            p7Data.cbData = RequiredLength - sizeof(DWORD) - sizeof(WORD) - sizeof(WORD);
            p7Data.pbData = buffer->bCertificate;
            HCERTSTORE hStore = CertOpenStore(CERT_STORE_PROV_PKCS7,
                                              X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                              NULL,
                                              0,
                                              &p7Data);
            if (hStore) {
                int count = 0;
                char szCodeSigningOID[] = szOID_PKIX_KP_CODE_SIGNING;
                // populate the key usage structure with the Code Signing OID
                CERT_ENHKEY_USAGE keyUsage;
                keyUsage.cUsageIdentifier = 1;
                keyUsage.rgpszUsageIdentifier = (LPSTR *)LocalAlloc(0, sizeof(LPSTR));
                keyUsage.rgpszUsageIdentifier[0] = &szCodeSigningOID[0];

                // Find certificates that contain the Code Signing Enhanced Key Usage
                PCCERT_CONTEXT  pCertContext = NULL;
                do {
                    pCertContext = CertFindCertificateInStore(hStore,
                                                              X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                                              CERT_FIND_EXT_ONLY_ENHKEY_USAGE_FLAG,
                                                              CERT_FIND_ENHKEY_USAGE,
                                                              &keyUsage,
                                                              pCertContext);
                    if (pCertContext) {
                        count++;
                        PrintCertificateInfo(pCertContext);
                    }
                } while (pCertContext);

                if (count == 0) {
                    _tprintf(_T("No Code Signing certificates found\n"));
                }

                LocalFree(keyUsage.rgpszUsageIdentifier);
                CertCloseStore(hStore, CERT_CLOSE_STORE_FORCE_FLAG);
            } else {
                _tprintf(_T("CertOpenStore failed with error 0x%.8X\n"), GetLastError());
            }

            DIGEST_HANDLE DigestHandle = NULL;
            ret = ImageGetDigestStream(hfile, i, DigestFunction, DigestHandle);
            if (!ret) {
                int x = GetLastError();
            }

            HeapFree(GetProcessHeap(), 0, buffer);
        }
    } __finally {
        if (INVALID_HANDLE_VALUE != hfile) {
            CloseHandle(hfile);
        }
    }

    LocalFree(Arglist);
}


void ParseCertificateInfo3(PIMAGE_DATA_DIRECTORY DataDirectory, LPWIN_CERTIFICATE SecurityDirectory)
/*

�˷�ʽ��Ч�����ƣ�signtool.exe verify /pa /a /v c:\windows\notepad.exe�����������ǿ���ࡣ

�ӷ���PE�ļ���DataDirectory��ʼ��һ��һ���Ľ�����
*/
{
    for (DWORD i = 0; i < DataDirectory->Size; ) {
        SecurityDirectory = LPWIN_CERTIFICATE((PBYTE)SecurityDirectory + i);

        printf("index:%d.\r\n", i + 1);

        PrintSecurity(SecurityDirectory);

        DWORD dwLength = SecurityDirectory->dwLength / 8;

        if (SecurityDirectory->dwLength % 8) {
            dwLength++;
        }

        i += dwLength * 8;
    }
}


void ParseCertificateInfo4(PIMAGE_DATA_DIRECTORY DataDirectory, LPWIN_CERTIFICATE SecurityDirectory)
/*
��openssl����PE��֤�顣
*/
{
    PKCS7 * pkcs7;
    unsigned char * CertData;
    long CertDataLength;

    CertData = (unsigned char *)SecurityDirectory->bCertificate;
    CertDataLength = SecurityDirectory->dwLength - FIELD_OFFSET(WIN_CERTIFICATE, bCertificate);

    pkcs7 = d2i_PKCS7(NULL, (const unsigned char **)&CertData, CertDataLength);
    if (NULL == pkcs7) {
        return;
    }

    DumpPKCS7(pkcs7);
}


DWORD Security(_In_ PBYTE Data, _In_ DWORD Size)
/*
�ο���\win2k\trunk\private\sdktools\imagehlp\dice.cxx��FindCertificate������
*/
{
    DWORD ret = ERROR_SUCCESS;

    if (!IsValidPE(Data, Size)) {
        return ret;
    }

    IMAGE_DATA_DIRECTORY DataDirectory = {0};
    GetDataDirectory(Data, Size, IMAGE_DIRECTORY_ENTRY_SECURITY, &DataDirectory);

    if (0 == DataDirectory.VirtualAddress) {
        printf("���ļ�û��Security.\r\n");
        return ret;
    }

    printf("Security Directory Information:\r\n");
    printf("VirtualAddress:%#010X.\r\n", DataDirectory.VirtualAddress);
    printf("Size:%#010X.\r\n", DataDirectory.Size);
    printf("\r\n");

    //////////////////////////////////////////////////////////////////////////////////////////////

    //ULONG size = 0;
    //PIMAGE_SECTION_HEADER FoundHeader = NULL;
    //LPWIN_CERTIFICATE SecurityDirectory = (LPWIN_CERTIFICATE)
    //    ImageDirectoryEntryToDataEx(Data,
    //                                FALSE,//�Լ�ӳ�����FALSE������ϵͳ���ص���TRUE�� 
    //                                IMAGE_DIRECTORY_ENTRY_SECURITY,
    //                                &size,
    //                                &FoundHeader);

    LPWIN_CERTIFICATE SecurityDirectory = (LPWIN_CERTIFICATE)(Data + DataDirectory.VirtualAddress);

    PIMAGE_NT_HEADERS NtHeaders = ImageNtHeader(Data);
    _ASSERTE(NtHeaders);

    //////////////////////////////////////////////////////////////////////////////////////////////

    printf("----------------------------------------------------------------------------------\n");
    printf("������ʽһ��\n");

    ParseCertificateInfo1();

    printf("----------------------------------------------------------------------------------\n");
    printf("������ʽ����\n");

    ParseCertificateInfo2();

    printf("----------------------------------------------------------------------------------\n");
    printf("������ʽ����\n");

    ParseCertificateInfo3(&DataDirectory, SecurityDirectory);

    printf("----------------------------------------------------------------------------------\n");
    printf("������ʽ�ģ�\n");

    ParseCertificateInfo4(&DataDirectory, SecurityDirectory);

    //////////////////////////////////////////////////////////////////////////////////////////////

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD Security(_In_ LPCWSTR FileName)
{
    return MapFile(FileName, Security);
}
