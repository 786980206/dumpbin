/*
�ļ���֤��Ľ����м���˼·��
1.��PE�ļ���ʽ�Ľ����������κ�API��
  ����������뷨����ϧ���ڻ����˽�֤�����صĽṹ�͹淶��
  ����ASN����CryptDecodeObjectEx��API��
2.ֱ���û�ȡ֤������API����WinTrust����غ�����
3.ImageHlp��ص�API������ȽϷ�ǳ���������Ƴ�֤�顣
4.�����㷨��CNG�����API�Ľ����������Ҫ���㷨��CNG����������˽⣬�������������ʹ�á�
5.�����������⣬
  �磺openssl���磺d2i_PKCS7��OBJ_obj2nid��sk_X509_num��sk_X509_value��X509_get_issuer_name���ȡ�
*/

#pragma once

#include "pch.h"
#include "Public.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


DWORD Security(_In_ LPCWSTR FileName);
