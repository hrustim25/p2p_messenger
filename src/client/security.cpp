#include "security.h"

#include <fstream>
#include <memory>
#include <random>

#include <openssl/x509.h>
#include <openssl/pem.h>

namespace msgr {

std::string Security::LoadFile(const std::string &path) {
    std::ifstream fstream(path);
    std::string result;
    if (!fstream.is_open()) {
        return result;
    }
    while (fstream.good()) {
        char ch;
        fstream.get(ch);
        if (fstream.eof()) {
            break;
        }
        result += ch;
    }
    return result;
}

void Security::GenerateAndSaveCerts() {
    std::unique_ptr<EVP_PKEY, void (*)(EVP_PKEY *)> pkey(EVP_PKEY_new(), EVP_PKEY_free);
    std::unique_ptr<BIGNUM, void (*)(BIGNUM *)> bne(BN_new(), BN_free);
    BN_set_word(bne.get(), RSA_F4);
    std::unique_ptr<RSA, void (*)(RSA *)> rsa(RSA_new(), RSA_free);
    RSA_generate_key_ex(rsa.get(), 2048, bne.get(), NULL);
    if (!EVP_PKEY_assign_RSA(pkey.get(), rsa.get())) {
        return;
    }
    rsa.release();

    std::unique_ptr<X509, void (*)(X509 *)> cert(X509_new(), X509_free);
    ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), 1);
    X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(cert.get()), 365 * 24 * 60 * 60);
    X509_set_pubkey(cert.get(), pkey.get());

    X509_NAME *name = X509_get_subject_name(cert.get());
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"RU", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"TNN Ltd.", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);
    X509_set_issuer_name(cert.get(), name);
    X509_sign(cert.get(), pkey.get(), EVP_sha256());

    std::unique_ptr<FILE, int (*)(FILE *)> fpriv(fopen("clientkey.pem", "wb"), fclose);
    PEM_write_PrivateKey(fpriv.get(), pkey.get(), NULL, NULL, 0, NULL, NULL);
    std::unique_ptr<FILE, int (*)(FILE *)> fcert(fopen("clientcert.pem", "wb"), fclose);
    PEM_write_X509(fcert.get(), cert.get());
}
}  // namespace msgr
