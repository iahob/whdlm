#include "cryptk_wrapper.h"
#include <assert.h>
#include <algorithm>
#include <fstream>

#if _HAS_MD5
#include "md5.h"
#endif

#if _HAS_MD6
#include "md6.h"
#endif

#if _HAS_LIBB64
#include "libb64.h"
#endif

#if _HAS_OPENSSL
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#if defined(_WIN32)
#pragma comment(lib, "libcrypto.lib")
#endif
#endif

#define HASH_FILE_BUFF_SIZE 1024

#if !defined(_WIN32)
#define _fseeki64 fseeko
#define _ftelli64 ftello
#endif

static long long get_file_size(FILE *fp) {
  _fseeki64(fp, 0, SEEK_END);
  auto length = _ftelli64(fp);
  if (length != 0) {
    _fseeki64(fp, 0, SEEK_SET);
  }
  return length;
}

#if _HAS_MD5
std::string cryptk::hash::md5(cxx17::string_view plaintext)
{
    char digest[16];
    md5_state_t pms;
    md5_init(&pms);
    md5_append(&pms, (const md5_byte_t*)plaintext.data(), plaintext.size());
    md5_finish(&pms, (md5_byte_t*)digest);

    std::string strHex(32, '\0');
    cryptk::bin2hex(digest, sizeof(digest), &strHex.front(), strHex.length());
    return strHex;
}

std::string cryptk::hash::md5raw(cxx17::string_view plaintext)
{
    std::string digest(16, '\0');
    md5_state_t pms;
    md5_init(&pms);
    md5_append(&pms, (const md5_byte_t*)plaintext.data(), plaintext.size());
    md5_finish(&pms, (md5_byte_t*)&digest.front());
    return digest;
}

std::string cryptk::hash::fmd5(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    // std::ifstream fin(filename, std::ios_base::binary);
    if (fp == NULL)
    {
        // std::cout << "Transmission Client: open failed!\n";
        return "";
    }

    //fin.seekg(0, std::ios_base::end);
    //std::streamsize bytes_left = fin.tellg(); // filesize initialized
    //fin.seekg(0, std::ios_base::beg);
    auto bytes_left = get_file_size(fp);

    char buffer[HASH_FILE_BUFF_SIZE];

    md5_state_t state;

    char hash[16] = { 0 };
    std::string result(32, '\0');

    md5_init(&state);

    while (bytes_left > 0) {
        auto n = (std::min)((std::streamsize)sizeof(buffer), (std::streamsize)bytes_left);
        // fin.read(buffer, bytes_read);
        auto bytes_read = fread(buffer, 1, n, fp);
        md5_append(&state, (unsigned char*)buffer, bytes_read);
        bytes_left -= bytes_read;
    }

    fclose(fp);
    md5_finish(&state, (unsigned char*)hash);

    cryptk::bin2hex(hash, sizeof(hash), &result.front(), result.size());

    return result;
}
#endif

#if _HAS_MD6
std::string cryptk::hash::md6(cxx17::string_view plaintext, size_t hashByteLen)
{
    assert(hashByteLen <= 64);

    char buffer[64] = { 0 };
    memset(buffer, 0, sizeof(buffer));

    md6_state state;
    md6_init(&state, hashByteLen << 3);
    md6_update(&state, (unsigned char*)plaintext.data(), plaintext.length() << 3);
    md6_final(&state, (unsigned char*)buffer);

    std::string strHex(hashByteLen << 1, '\0');
    cryptk::bin2hex(buffer, hashByteLen, &strHex.front(), strHex.length());
    return strHex;
}

std::string cryptk::hash::md6raw(cxx17::string_view plaintext, size_t hashByteLen)
{
    std::string result(hashByteLen, '\0');

    md6_state state;
    md6_init(&state, hashByteLen << 3);
    md6_update(&state, (unsigned char*)plaintext.data(), plaintext.length() << 3);
    md6_final(&state, (unsigned char*)&result.front());

    return result;
}

std::string cryptk::hash::fmd6(const char* filename, int hashByteLen)
{
    std::ifstream fin;
    fin.open(filename, std::ios_base::binary);
    if (!fin.is_open())
    {
        // std::cout << "Transmission Client: open failed!\n";
        return "";
    }

    fin.seekg(0, std::ios_base::end);
    std::streamsize bytes_left = fin.tellg(); // filesize initialized
    fin.seekg(0, std::ios_base::beg);

    char buffer[HASH_FILE_BUFF_SIZE];

    md6_state state;

    if (hashByteLen > 64)
        hashByteLen = 64;
    // int hashByteLen = 64;

    // assert(hashByteLen <= 64);

    char hash[64] = { 0 };
    std::string result(hashByteLen << 1, '\0');

    md6_init(&state, hashByteLen << 3);

    while (bytes_left > 0) {
        int bytes_read = (std::min)((std::streamsize)sizeof(buffer), bytes_left);
        fin.read(buffer, bytes_read);
        md6_update(&state, (unsigned char*)buffer, bytes_read << 3);
        bytes_left -= bytes_read;
    }

    fin.close();
    md6_final(&state, (unsigned char*)hash);

    cryptk::bin2hex(hash, hashByteLen, &result.front(), result.size());

    return result;
}
#endif

#if _HAS_OPENSSL
std::string cryptk::hash::hmac_sha1(const std::string& message, const std::string& key)
{
    unsigned char result[64] = { 0 };
    unsigned int outlen = sizeof(result);
    unsigned char* digest = HMAC(EVP_sha1(), key.c_str(), key.size(), (const unsigned char*)message.c_str(), message.size(), result, &outlen);

    return std::string((const char*)result, outlen);
}
std::string cryptk::hash::compute_digest(cxx17::string_view data, cxx17::string_view algorithm)
{
    const EVP_MD* md = nullptr;
    unsigned char mdValue[EVP_MAX_MD_SIZE] = { 0 };
    unsigned int mdLen = 0;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname(algorithm.data());
    if (!md)
    {
        return "";
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, data.data(), data.size());
    EVP_DigestFinal_ex(mdctx, mdValue, &mdLen);
    EVP_MD_CTX_destroy(mdctx);

    std::string result(mdLen << 1, '\0');
    cryptk::bin2hex(mdValue, mdLen, &result.front(), result.size());
    return result;
}
#endif

#if _HAS_LIBB64
std::string cryptk::http::b64dec(cxx17::string_view ciphertext)
{
    std::string plaintext( ciphertext.length(), '\0' );

    base64_decodestate state;
    base64_init_decodestate(&state);
    int r1 = base64_decode_block(ciphertext.data(), ciphertext.length(), &plaintext.front(), &state);

    plaintext.resize(r1);
    return plaintext;
}

std::string cryptk::http::b64enc(cxx17::string_view  plaintext)
{
    std::string ciphertext( (plaintext.length() * 2), '\0' );
    char* wrptr = &ciphertext.front();
    base64_encodestate state;
    base64_init_encodestate(&state);
    int r1 = base64_encode_block(plaintext.data(), plaintext.length(), wrptr, &state);
    int r2 = base64_encode_blockend(wrptr + r1, &state);

    ciphertext.resize(r1 + r2);
    return ciphertext;
}
#endif

std::string cryptk::http::urlencode(cxx17::string_view input)
{
    std::string output;
    for( size_t ix = 0; ix < input.size(); ix++ )
    {      
        uint8_t buf[4];
        memset( buf, 0, 4 );
        if( isalnum( (uint8_t)input[ix] ) )
        {      
            buf[0] = input[ix];
        }
        else if ( isspace( (uint8_t)input[ix] ) )
        {
            buf[0] = '+';
        }
        else
        {
            buf[0] = '%';
            buf[1] = cryptk::hex2uchr( (uint8_t)input[ix] >> 4 );
            buf[2] = cryptk::hex2uchr( (uint8_t)input[ix] % 16);
        }
        output += (char *)buf;
    }
    return output;
};

std::string cryptk::http::urldecode(cxx17::string_view ciphertext)
{
    std::string result = "";

    for( size_t ix = 0; ix < ciphertext.size(); ix++ )
    {
        uint8_t ch = 0;
        if(ciphertext[ix]=='%')
        {
            ch = (cryptk::uchr2hex(ciphertext[ix+1])<<4);
            ch |= cryptk::uchr2hex(ciphertext[ix+2]);
            ix += 2;
        }
        else if(ciphertext[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = ciphertext[ix];
        }
        result += (char)ch;
    }

    return result;
}

/// ----------------- rsa wrappers ---------------------------
#if _HAS_OPENSSL
namespace cryptk {

    namespace rsa {
        struct RSA_Key {
            RSA* p_rsa;
            void* p_io;
        };

        typedef bool(*load_key_func)(const char* key, int /*length*/, RSA_Key* k);
        typedef void(*process_padding_func)(int paddingMode, int ilen, int& flen);
        typedef int(*RSA_crypto_func)(int flen, const unsigned char *from,
            unsigned char *to, RSA *rsa, int padding);
        typedef void(*close_key_func)(RSA_Key* k);

        static bool load_public_key_from_mem(const char* key, int length, RSA_Key* k)
        {
            BIO *bio = NULL;
            if (length <= 0) {
                perror("The public key is empty!");
                return false;
            }

            if ((bio = BIO_new_mem_buf((char*)key, length)) == NULL)
            {
                perror("BIO_new_mem_buf failed!");
                return false;
            }

            k->p_io = bio;
            k->p_rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
            if (k->p_rsa == nullptr) {
                BIO_free_all(bio);
                return false;
            }

            return true;
        }

        static bool load_private_key_from_mem(const char* key, int length, RSA_Key* k)
        {
            BIO *bio = NULL;
            if (length <= 0) {
                perror("The public key is empty!");
                return false;
            }

            if ((bio = BIO_new_mem_buf(key, length)) == NULL)
            {
                perror("BIO_new_mem_buf failed!");
                return false;
            }

            k->p_io = bio;
            k->p_rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

            if (k->p_rsa == nullptr) {
                BIO_free_all(bio);
                return false;
            }
            return true;
        }

        static bool load_public_key_from_file(const char* key, int /*length*/, RSA_Key* k)
        {
            /* equal as follow:
            FILE* fp = nullptr;
            if ((fp = fopen(key, "r")) == NULL) {
                perror("open key file error");
                return false;
            }
            PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
            */
            BIO *bio = NULL;
            if ((bio = BIO_new_file(key, "r")) == NULL)
            {
                perror("BIO_new_file failed!");
                return false;
            }

            k->p_io = bio;
            k->p_rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL); 
            if (k->p_rsa == nullptr) {
                BIO_free_all(bio);
                return false;
            }

            return true;
        }

        static bool load_private_key_from_file(const char* key, int /*length*/, RSA_Key* k)
        {
            BIO *bio = NULL;
            if ((bio = BIO_new_file(key, "r")) == NULL)
            {
                perror("BIO_new_file failed!");
                return false;
            }
            k->p_io = bio;
            k->p_rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
            if (k->p_rsa == nullptr) {
                BIO_free_all(bio);
                return false;
            }

            return true;
        }

        static void process_public_padding(int paddingMode, int ilen, int& flen)
        {
            // pitfall: private encrypt supported PaddingModes: RSA_PKCS1_PADDING, RSA_PKCS1_OAEP_PADDING, RSA_SSLV23_PADDING, RSA_NO_PADDING
            switch (paddingMode) {
            case RSA_PKCS1_OAEP_PADDING:
                flen -= (2 * SHA_DIGEST_LENGTH + 2); // pitfall: many blogs from internet said: it's 41, actually, it must be 42, phpseclib does correct.
                break;
            case RSA_PKCS1_PADDING:
            case RSA_SSLV23_PADDING:
                flen -= 11;
                break;
            case RSA_NO_PADDING:
                assert(ilen % flen == 0);
                break;
            }
        }

        static void process_private_padding(int paddingMode, int ilen, int& flen)
        {
            // pitfall: private encrypt supported PaddingModes: RSA_PKCS1_PADDING, RSA_X931_PADDING, RSA_NO_PADDING
            switch (paddingMode) {
            case RSA_PKCS1_PADDING:
                flen -= 11;
                break;
            case RSA_X931_PADDING:
                flen -= 2;
                break;
            case RSA_NO_PADDING:
                assert(ilen % flen == 0);
                break;
            };
        }

        static void close_keybio(RSA_Key* k)
        {
            if (k->p_rsa != nullptr) {
                BIO_free_all((BIO*)k->p_io);
            }
        }

        //static void close_keyfile(RSA_Key* k)
        //{
        //    if (k->p_rsa != nullptr) {
        //        fclose((FILE*)k->p_io);
        //    }
        //}

        struct encrypt_helper {
            load_key_func load_key;
            process_padding_func process_padding;
            RSA_crypto_func crypto_func;
            close_key_func close_key;
        };

        struct decrypt_helper {
            load_key_func load_key;
            RSA_crypto_func crypto_func;
            close_key_func close_key;
        };

        static
            std::string common_encrypt(cxx17::string_view plaintext, cxx17::string_view key, const encrypt_helper& helper, int paddingMode)
        {
            RSA_Key k;

            int keylen = static_cast<int>(key.length());
            if (!helper.load_key(key.data(), keylen, &k))
                return "";

            int flen = RSA_size(k.p_rsa);
            auto ilen = plaintext.length();
            auto buffer = (unsigned char*)malloc(flen);
            int iret = -1, grab;
            size_t offset = 0;

#if defined(_DEBUG)
            const char* errormsg = nullptr;
            auto error_handler = [](const char *str, size_t len, void *u) ->int {
                *((const char**)u) = str;
                return 0;
            };
#endif
            std::string result;

            helper.process_padding(paddingMode, ilen, flen);

            do {
                grab = ilen - offset;
                if (grab > flen)
                    grab = flen;
                iret = helper.crypto_func(grab, (const unsigned char *)plaintext.data() + offset, buffer, k.p_rsa, paddingMode);
                if (iret > 0) {
                    result.insert(result.end(), buffer, buffer + iret);
                    offset += grab;
                }
                else {
#if defined(_DEBUG)
                    ERR_print_errors_cb(error_handler, &errormsg);
#endif
                    break;
                }
            } while (offset < ilen);

            free(buffer);

            helper.close_key(&k);

            return result;
        }

        static
            std::string common_decrypt(cxx17::string_view cipertext, cxx17::string_view key, const decrypt_helper& helper, int paddingMode)
        {
            RSA_Key k;

            int keylen = static_cast<int>(key.length());
            if (!helper.load_key(key.data(), keylen, &k))
                return "";

            std::string result;

#if defined(_DEBUG)
            const char* errormsg = nullptr;
            auto error_handler = [](const char *str, size_t len, void *u) ->int {
                *((const char**)u) = str;
                return 0;
            };
#endif
            auto flen = RSA_size(k.p_rsa);
            auto buffer = (unsigned char*)malloc(flen);
            auto ilen = cipertext.length();
            int iret = -1;
            size_t offset = 0;
            do {
                iret = helper.crypto_func(flen, (const unsigned char *)cipertext.data() + offset, buffer, k.p_rsa, paddingMode);
                if (iret > 0) {
                    result.insert(result.end(), buffer, buffer + iret);
                    offset += flen;
                }
                else {
#if defined(_DEBUG)
                    ERR_print_errors_cb(error_handler, &errormsg);
#endif
                    break;
                }
            } while (offset < ilen);

            free(buffer);

            helper.close_key(&k);

            return result;
        }

        namespace pub {
            std::string encrypt(cxx17::string_view plaintext, cxx17::string_view key, int paddingMode)
            {
                encrypt_helper helper = { load_public_key_from_mem, process_public_padding, RSA_public_encrypt,  close_keybio };
                return common_encrypt(plaintext, key, helper, paddingMode);
            }
            std::string decrypt(cxx17::string_view ciphertext, cxx17::string_view key, int paddingMode)
            {
                decrypt_helper helper = { load_public_key_from_mem, RSA_public_decrypt,  close_keybio };
                return common_decrypt(ciphertext, key, helper, paddingMode);
            }

            std::string encrypt2(cxx17::string_view plaintext, cxx17::string_view keyfile, int paddingMode)
            {
                encrypt_helper helper = { load_public_key_from_file, process_public_padding, RSA_public_encrypt,  close_keybio };
                return common_encrypt(plaintext, keyfile, helper, paddingMode);
            }
            std::string decrypt2(cxx17::string_view ciphertext, cxx17::string_view keyfile, int paddingMode)
            {
                decrypt_helper helper = { load_public_key_from_file, RSA_public_decrypt,  close_keybio };
                return common_decrypt(ciphertext, keyfile, helper, paddingMode);
            }
        }

        namespace pri {
            std::string encrypt(cxx17::string_view plaintext, cxx17::string_view key, int paddingMode)
            {
                encrypt_helper helper = { load_private_key_from_mem, process_private_padding, (RSA_crypto_func)&RSA_private_encrypt,  close_keybio };
                return common_encrypt(plaintext, key, helper, paddingMode);
            }
            std::string decrypt(cxx17::string_view ciphertext, cxx17::string_view key, int paddingMode)
            {
                decrypt_helper helper = { load_private_key_from_mem, RSA_private_decrypt,  close_keybio };
                return common_decrypt(ciphertext, key, helper, paddingMode);
            }

            std::string encrypt2(cxx17::string_view plaintext, cxx17::string_view keyfile, int paddingMode)
            {
                encrypt_helper helper = { load_private_key_from_file, process_private_padding, (RSA_crypto_func)&RSA_private_encrypt,  close_keybio/*close_keyfile*/ };
                return common_encrypt(plaintext, keyfile, helper, paddingMode);
            }
            std::string decrypt2(cxx17::string_view ciphertext, cxx17::string_view keyfile, int paddingMode)
            {
                decrypt_helper helper = { load_private_key_from_file, RSA_private_decrypt, close_keybio};
                return common_decrypt(ciphertext, keyfile, helper, paddingMode);
            }
        }
    }
};
#endif

