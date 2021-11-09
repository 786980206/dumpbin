#include "pch.h"
#include "openssl.h"


#pragma warning(disable:4996)


//////////////////////////////////////////////////////////////////////////////////////////////////


const char * GetLnByNid(int nid)
{
    const char * ln = "�д�����";

    switch (nid) {
    case NID_rsaEncryption:
        ln = LN_rsaEncryption;
        break;
    case NID_md5WithRSAEncryption:
        ln = LN_md5WithRSAEncryption;
        break;
    case NID_sha256WithRSAEncryption:
        ln = LN_sha256WithRSAEncryption;
        break;
    case NID_sha1WithRSAEncryption:
        ln = LN_sha1WithRSAEncryption;
        break;
    case NID_sha256:
        ln = LN_sha256;
        break;
    case NID_sha1:
        ln = LN_sha1;
        break;
    default:
        break;
    }

    return ln;
}


void DumpX509(X509 * x509)
{
    long version = X509_get_version(x509);
    printf("�汾:V%d.\n", version + 1);

    ASN1_INTEGER * serialNumber = X509_get_serialNumber(x509);
    printf("���к�:");
    for (int i = 0; i < serialNumber->length; i++) {
        printf("%02x", serialNumber->data[i]);
    }
    printf("\n");

    X509_NAME * issuer_name = X509_get_issuer_name(x509);
    int issuer_count = X509_NAME_entry_count(issuer_name);
    string name;
    for (int i = 0; i < issuer_count; i++) {
        X509_NAME_ENTRY * entry = X509_NAME_get_entry(issuer_name, i);
        ASN1_STRING * data = X509_NAME_ENTRY_get_data(entry);
        if (name.length()) {
            name += ", ";
        }
        name += (char *)data->data;
    }
    printf("�䷢��:%s.\n", name.c_str());
    name.clear();

    X509_NAME * subject_name = X509_get_subject_name(x509);
    int subject_count = X509_NAME_entry_count(subject_name);
    for (int i = 0; i < subject_count; i++) {
        X509_NAME_ENTRY * entry = X509_NAME_get_entry(subject_name, i);
        ASN1_STRING * data = X509_NAME_ENTRY_get_data(entry);
        if (name.length()) {
            name += ", ";
        }
        name += (char *)data->data;
    }
    printf("ʹ����:%s.\n", name.c_str());

    const ASN1_TIME * notBefore = X509_get0_notBefore(x509);
    struct tm tm;
    int ret = ASN1_TIME_to_tm(notBefore, &tm);
    printf("��Ч�ڴӣ�%04d��%02d��%02d�� %02d:%02d:%02d.\n",
           tm.tm_year + 1900,
           tm.tm_mon + 1,
           tm.tm_mday,
           tm.tm_hour,
           tm.tm_min,
           tm.tm_sec);

    const ASN1_TIME * notAfter = X509_get0_notAfter(x509);
    ret = ASN1_TIME_to_tm(notAfter, &tm);
    printf("����%04d��%02d��%02d�� %02d:%02d:%02d.\n",
           tm.tm_year + 1900,
           tm.tm_mon + 1,
           tm.tm_mday,
           tm.tm_hour,
           tm.tm_min,
           tm.tm_sec);

    //////////////////////////////////////////////////////////////////////////////////////////////

    int signature_type = X509_get_signature_type(x509);

    int secbits;
    int nid; //ȡֵ���磺NID_md5WithRSAEncryption
    int pknid;//ȡֵ���磺NID_rsaEncryption 
    X509_get_signature_info(x509, &nid, &pknid, &secbits, NULL);

    printf("ǩ���㷨:%s.\n", GetLnByNid(pknid));
    printf("ǩ����ϣ�㷨:%s.\n", GetLnByNid(nid));

    nid = X509_get_signature_nid(x509);//����õ��ľ�Ȼ������Ĳ�һ����
    //printf("ǩ����ϣ�㷨:%s.\n", GetLnByNid(nid));//���Ǻ������һ����    

    const X509_ALGOR * sig_alg;
    const ASN1_BIT_STRING * sig;
    X509_get0_signature(&sig, &sig_alg, x509);

    char oid[128] = {0};//���磺1.2.840.113549.1.1.11
    OBJ_obj2txt(oid, 128, sig_alg->algorithm, 1);

    nid = OBJ_obj2nid(sig_alg->algorithm);//NID_md5WithRSAEncryption
    printf("ǩ���㷨(Signature Algorithm):%s.\n", GetLnByNid(nid));

    //printf("��Կ����:%d.\n", sig_alg->parameter->type);//���ֵ���ֽ�����񲻶ԡ�

    //////////////////////////////////////////////////////////////////////////////////////////////

    int crit = 0;
    AUTHORITY_KEYID * akeyid = NULL;
    akeyid = (AUTHORITY_KEYID *)X509_get_ext_d2i(x509, NID_authority_key_identifier, &crit, NULL);
    if (akeyid) {
        printf("��Ȩ��Կ��ʶ��:");
        for (int i = 0; i < akeyid->keyid->length; i++) {
            printf("%02x", akeyid->keyid->data[i]);
        }
        printf("\n");
    }

    ASN1_OCTET_STRING * skid = NULL;
    skid = (ASN1_OCTET_STRING *)X509_get_ext_d2i(x509, NID_subject_key_identifier, &crit, NULL);
    printf("ʹ������Կ��ʶ��:");
    for (int i = 0; i < skid->length; i++) {
        printf("%02x", skid->data[i]);
    }
    printf("\n");

    BASIC_CONSTRAINTS * bc;
    bc = (BASIC_CONSTRAINTS *)X509_get_ext_d2i(x509, NID_basic_constraints, NULL, NULL);
    if (bc) {
        printf("����Լ����Subject Type=%d.", bc->ca);//������������ɶ���壬�д����롣
        if (bc->pathlen) {
            printf("Basic Constraints:");
            for (int i = 0; i < bc->pathlen->length; i++) {
                printf("%02x", bc->pathlen->data[i]);
            }
            printf("\n");
        } else {
            printf("Path Length Constraint=None.\n");
        }
    }

    //NID_key_usage.��Կ��;��
    //NID_subject_alt_name.������

    //////////////////////////////////////////////////////////////////////////////////////////////

    const EVP_MD * fprint_type = EVP_sha1();
    unsigned char fprint[EVP_MAX_MD_SIZE] = {0};
    unsigned int fprint_size = 0;

    X509_digest(x509, fprint_type, fprint, &fprint_size);

    printf("ָ��:");
    for (unsigned int i = 0; i < fprint_size; i++) {
        printf("%02x", fprint[i]);
    }
    printf("\n");

    //////////////////////////////////////////////////////////////////////////////////////////////

    X509_PUBKEY * PUBKEY = X509_get_X509_PUBKEY(x509);
    //EVP_PKEY * X509_get_pubkey(X509 * x509);
    ASN1_BIT_STRING * pubkey_bitstr = X509_get0_pubkey_bitstr(x509);

    size_t          publen = 0;
    unsigned char * pub = NULL;
    EVP_PKEY * pkey = X509_get0_pubkey(x509);
    int keyid = EVP_PKEY_id(pkey); //EVP_PKEY_RSA

    switch (keyid) {
    case EVP_PKEY_RSA:
        printf("��Կ����:RSA\n");
        break;
    case EVP_PKEY_DSA:
        printf("��Կ����:DSA\n");
        break;
    default:
        printf("��Կ����:�д�����\n");
        break;
    }

    printf("��Կ����:%d bits\n", EVP_PKEY_bits(pkey));

    int len = i2d_X509_PUBKEY(PUBKEY, NULL);
    //char * key = (char *)HeapAlloc(GetProcessHeap(), 0, len);
    //char * key = new char[len]();
    char * key = NULL;//�����ȡ�����ݺ��񲻶ԡ�
    len = i2d_X509_PUBKEY(PUBKEY, (unsigned char **)&key);

    const unsigned char * pp = NULL;
    int pklen;
    //EC_KEY * eckey = NULL;
    X509_ALGOR * palg;
    const void * pval;
    int ptype;

    X509_PUBKEY_get0_param(NULL, &pp, &pklen, &palg, PUBKEY);
    X509_ALGOR_get0(NULL, &ptype, &pval, palg);
    //d2i_X509_ALGOR(&palg, &pp, pklen);//����ᵼ�½����˳��쳣��

    //printf("��Կ����:%d.\n", ptype);//���ֵ���ֽ�����񲻶ԡ�

    //HeapFree(GetProcessHeap(), 0, key);
    //delete [] key;
    OPENSSL_free(key);

    //////////////////////////////////////////////////////////////////////////////////////////////    

    EVP_PKEY * pubkey = X509_get_pubkey(x509);
    unsigned char tem[1024] = {0};
    unsigned char * p = tem;
    len = i2d_PublicKey(pubkey, &p);

    printf("��Կ:");
    for (int i = 0; i < len; i++) {
        unsigned char t = tem[i];
        printf("%02x", t);
    }
    printf("\n");

    RSA * rsa = EVP_PKEY_get1_RSA(pubkey);
    char * Modulus = BN_bn2hex(RSA_get0_n(rsa));
    char * Exponent = BN_bn2hex(RSA_get0_e(rsa));

    printf("Modulus:%s\n", Modulus);
    printf("Exponent:%s\n", Exponent);

    //printf("��Կ����:%d bits\n", RSA_size(rsa) * 8);

    OPENSSL_free(Modulus);
    OPENSSL_free(Exponent);
    RSA_free(rsa);
}


void DumpPKCS7(PKCS7 * pkcs7)
{
    char       name[10000];
    int ret = OBJ_obj2txt(name, 1000, pkcs7->type, 0);
    printf("type : %s \n", name);

    int type = OBJ_obj2nid(pkcs7->type);

    STACK_OF(X509) * X509Certs = NULL;
    STACK_OF(X509_CRL) * X509Crls = NULL;

    switch (type) {
    case NID_pkcs7_signed:
        if (pkcs7->d.sign != NULL) {
            X509Certs = pkcs7->d.sign->cert;
            X509Crls = pkcs7->d.sign->crl;
        }
        break;
    case NID_pkcs7_signedAndEnveloped:
        if (pkcs7->d.signed_and_enveloped != NULL) {
            X509Certs = pkcs7->d.signed_and_enveloped->cert;
            X509Crls = pkcs7->d.signed_and_enveloped->crl;
        }
        break;
    default:
        return;
        break;
    }

    for (int CertIndex = 0; CertIndex < sk_X509_num(X509Certs); CertIndex++) {
        X509 * X509Cert = sk_X509_value(X509Certs, CertIndex);

        printf("��:%d��֤�����Ϣ��\n", CertIndex + 1);

        DumpX509(X509Cert);

        printf("\n\n\n");
    }
}


//int test()
//{
//    OPENSSL_die("Voluntary abort", __FILE__, __LINE__);
//    return 0;
//}


//////////////////////////////////////////////////////////////////////////////////////////////////
