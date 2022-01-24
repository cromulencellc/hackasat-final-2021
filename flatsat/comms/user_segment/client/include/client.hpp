#ifndef _USR_SEG_CLIENT
#define _USR_SEG_CLIENT

/**
 * HAS 2 Crappy Crypto Challenge
 * Written by: MeenMachine, Tormund with special thanks to Pedant
 **/

// Local Crypto Includes
#include "internal/deprecated.h"
#include "internal/cryptlib.h"
#include "prov/providercommon.h"
#include "rsa_local.h"

#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/self_test.h>

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <errno.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// Cpp Includes
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Other Includes
#include "argparse/argparse.hpp"

// Debugging
#define DEBUG 0
#if DEBUG
    #define LOG printf
#else
    #define LOG(...) while(0){}
#endif

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)

#define KEY_BITS 1372
#define M_BIG_NUM 3930574699

class RSAKeyGenCrypto{
    public:
    RSAKeyGenCrypto(){
        this->rsa_obj = RSA_new();

        this->stdlib_rand_meth = {
            .seed       = RSAKeyGenCrypto::stdlib_rand_seed,
            .bytes      = RSAKeyGenCrypto::stdlib_rand_bytes,
            .cleanup    = NULL,
            .add        = RSAKeyGenCrypto::stdlib_rand_add,
            .pseudorand = RSAKeyGenCrypto::stdlib_rand_bytes,
            .status     = RSAKeyGenCrypto::stdlib_rand_status
        };

        RAND_set_rand_method(RAND_stdlib());
        this->RSAGenerateKey();
        free_all_rsa_goodies();
    }
    int RSAGenerateKey(){
        int ret = 0;

        if(!secure_getenv("TEAM_RANDOM_NUM")){
            return 0;
        }

        if((ret = RSAMultiPrimeKeygen(rsa_obj)) != 1){
            LOG_ERR("(0) Error: Generating key. Got ret: (%d)\n", ret);
            free_all_rsa_goodies();
            exit(-1);
        }

        this->bp_public = BIO_new_file("public.pem", "w+");

        if(this->bp_public == NULL){
            LOG_ERR("(1) Error: Creating new file. Does the path to the folder exist?\n");
            free_all_rsa_goodies();
            exit(-1);
        }

        if((ret = PEM_write_bio_RSAPublicKey(this->bp_public, this->rsa_obj)) != 1){
            LOG_ERR("(2) Error: Writing public key to a file.\n");
            free_all_rsa_goodies();
            exit(-1);
        }

        this->bp_private = BIO_new_file("private.pem", "w+");

        if(this->bp_private == NULL){
            LOG_ERR("(3) Error: Creating new file. Does the path to the folder exist?\n");
            free_all_rsa_goodies();
            exit(-1);
        }

        if((ret = PEM_write_bio_RSAPrivateKey(this->bp_private, this->rsa_obj, NULL, NULL, 0, NULL, NULL)) != 1){
            LOG_ERR("(4) Error: Writing the private key to a file.\n");
            free_all_rsa_goodies();
            exit(-1);
        }

        free_all_rsa_goodies();

        return 1;
    }
    void free_all_rsa_goodies(){
        RSA_free(this->rsa_obj);
        BIO_free_all(this->bp_public);
        BIO_free_all(this->bp_private);

        this->rsa_obj = NULL;
        this->bp_public = NULL;
        this->bp_private = NULL;
    }
    int randNumGen(){
        return 0;
    }
    int RSAMultiPrimeKeygen(RSA *rsa){

        BN_GENCB *cb = NULL;

        BIGNUM *r0 = NULL, *r1 = NULL, *r2 = NULL, *e_value = NULL, *tmp, *prime;
        int n = 0, bitsr[RSA_MAX_PRIME_NUM], bitse = 0;
        int i = 0, adj = 0, retries = 0;
        RSA_PRIME_INFO *pinfo = NULL;
        STACK_OF(RSA_PRIME_INFO) *prime_infos = NULL;
        BN_CTX *ctx = NULL;
        BN_ULONG bitst = 0;
        unsigned long error = 0, e = RSA_F4;
        int ok = -1, primes = 0;

        primes = 3;

        e_value = BN_new();
        // RSA_F4 65537

        if(BN_set_word(e_value, e) != 1){
            BN_free(e_value);
            return -1;
        }

        // Modify
        char *random_num = secure_getenv("TEAM_RANDOM_NUM");
        if(!random_num){
            LOG_ERR("Error (env): TEAM_RANDOM_NUM enviroment var doesn't exist\n");
            exit(-1);
        }

        errno = 0;
        uint64_t rnd_seed = strtoull(random_num, NULL, 10);
        if(errno != 0){
            perror("Error strtoull");
            exit(-1);
        }

        // LOG("random_num: %s | rnd_seed: %lu \n", random_num, rnd_seed);
        rnd_seed = RSAKeyGenCrypto::fast_exp_mod(rnd_seed, 6, M_BIG_NUM);

        ctx = BN_CTX_new_ex(rsa->libctx);
        if (ctx == NULL)
            goto err;
        BN_CTX_start(ctx);
        r0 = BN_CTX_get(ctx);
        r1 = BN_CTX_get(ctx);
        r2 = BN_CTX_get(ctx);
        if (r2 == NULL)
            goto err;

        bitsr[0] = 172;
        bitsr[1] = 172;
        bitsr[2] = 1028;

        rsa->dirty_cnt++;

        /* We need the RSA components non-NULL */
        if (!rsa->n && ((rsa->n = BN_new()) == NULL))
            goto err;
        if (!rsa->d && ((rsa->d = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->d, BN_FLG_CONSTTIME);
        if (!rsa->e && ((rsa->e = BN_new()) == NULL))
            goto err;
        if (!rsa->p && ((rsa->p = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->p, BN_FLG_CONSTTIME);
        if (!rsa->q && ((rsa->q = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->q, BN_FLG_CONSTTIME);
        if (!rsa->dmp1 && ((rsa->dmp1 = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->dmp1, BN_FLG_CONSTTIME);
        if (!rsa->dmq1 && ((rsa->dmq1 = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->dmq1, BN_FLG_CONSTTIME);
        if (!rsa->iqmp && ((rsa->iqmp = BN_secure_new()) == NULL))
            goto err;
        BN_set_flags(rsa->iqmp, BN_FLG_CONSTTIME);

        /* initialize multi-prime components */
        if (primes > RSA_DEFAULT_PRIME_NUM) {
            rsa->version = RSA_ASN1_VERSION_MULTI;
            prime_infos = sk_RSA_PRIME_INFO_new_reserve(NULL, primes - 2);
            if (prime_infos == NULL)
                goto err;
            if (rsa->prime_infos != NULL) {
                /* could this happen? */
                sk_RSA_PRIME_INFO_pop_free(rsa->prime_infos, ossl_rsa_multip_info_free);
            }
            rsa->prime_infos = prime_infos;

            /* prime_info from 2 to |primes| -1 */
            for (i = 2; i < primes; i++) {
                pinfo = ossl_rsa_multip_info_new();
                if (pinfo == NULL)
                    goto err;
                (void)sk_RSA_PRIME_INFO_push(prime_infos, pinfo);
            }
        }

        if (BN_copy(rsa->e, e_value) == NULL)
            goto err;

        /* generate p, q and other primes (if any) */
        for (i = 0; i < primes; i++) {
            adj = 0;
            retries = 0;

            if (i == 0) {
                prime = rsa->p;
                RAND_seed(&rnd_seed, sizeof(rnd_seed));
            } else if (i == 1) {
                prime = rsa->q;
                RAND_seed(&rnd_seed, sizeof(rnd_seed));
            } else {
                pinfo = sk_RSA_PRIME_INFO_value(prime_infos, i - 2);
                prime = pinfo->r;
                RAND_seed(&rnd_seed, sizeof(rnd_seed));
            }
            BN_set_flags(prime, BN_FLG_CONSTTIME);

            for (;;) {
    redo:
                if (!BN_generate_prime_ex2(prime, bitsr[i] + adj, 0, NULL, NULL,
                                        cb, ctx))
                    goto err;
                /*
                * prime should not be equal to p, q, r_3...
                * (those primes prior to this one)
                */
                {
                    int j;

                    for (j = 0; j < i; j++) {
                        BIGNUM *prev_prime;

                        if (j == 0)
                            prev_prime = rsa->p;
                        else if (j == 1)
                            prev_prime = rsa->q;
                        else
                            prev_prime = sk_RSA_PRIME_INFO_value(prime_infos,
                                                                j - 2)->r;

                        if (!BN_cmp(prime, prev_prime)) {
                            goto redo;
                        }
                    }
                }
                if (!BN_sub(r2, prime, BN_value_one()))
                    goto err;
                ERR_set_mark();
                BN_set_flags(r2, BN_FLG_CONSTTIME);
                if (BN_mod_inverse(r1, r2, rsa->e, ctx) != NULL) {
                /* GCD == 1 since inverse exists */
                    break;
                }
                error = ERR_peek_last_error();
                if (ERR_GET_LIB(error) == ERR_LIB_BN
                    && ERR_GET_REASON(error) == BN_R_NO_INVERSE) {
                    /* GCD != 1 */
                    ERR_pop_to_mark();
                } else {
                    goto err;
                }
                if (!BN_GENCB_call(cb, 2, n++))
                    goto err;
            }

            bitse += bitsr[i];

            /* calculate n immediately to see if it's sufficient */
            if (i == 1) {
                /* we get at least 2 primes */
                if (!BN_mul(r1, rsa->p, rsa->q, ctx))
                    goto err;
            } else if (i != 0) {
                /* modulus n = p * q * r_3 * r_4 ... */
                if (!BN_mul(r1, rsa->n, prime, ctx))
                    goto err;
            } else {
                /* i == 0, do nothing */
                if (!BN_GENCB_call(cb, 3, i))
                    goto err;
                continue;
            }
            /*
            * if |r1|, product of factors so far, is not as long as expected
            * (by checking the first 4 bits are less than 0x9 or greater than
            * 0xF). If so, re-generate the last prime.
            *
            * NOTE: This actually can't happen in two-prime case, because of
            * the way factors are generated.
            *
            * Besides, another consideration is, for multi-prime case, even the
            * length modulus is as long as expected, the modulus could start at
            * 0x8, which could be utilized to distinguish a multi-prime private
            * key by using the modulus in a certificate. This is also covered
            * by checking the length should not be less than 0x9.
            */
            if (!BN_rshift(r2, r1, bitse - 4))
                goto err;
            bitst = BN_get_word(r2);

            /* save product of primes for further use, for multi-prime only */
            if (i > 1 && BN_copy(pinfo->pp, rsa->n) == NULL)
                goto err;
            if (BN_copy(rsa->n, r1) == NULL)
                goto err;
            if (!BN_GENCB_call(cb, 3, i))
                goto err;
        }

        if (BN_cmp(rsa->p, rsa->q) < 0) {
            tmp = rsa->p;
            rsa->p = rsa->q;
            rsa->q = tmp;
        }

        /* calculate d */

        /* p - 1 */
        if (!BN_sub(r1, rsa->p, BN_value_one()))
            goto err;
        /* q - 1 */
        if (!BN_sub(r2, rsa->q, BN_value_one()))
            goto err;
        /* (p - 1)(q - 1) */
        if (!BN_mul(r0, r1, r2, ctx))
            goto err;
        /* multi-prime */
        for (i = 2; i < primes; i++) {
            pinfo = sk_RSA_PRIME_INFO_value(prime_infos, i - 2);
            /* save r_i - 1 to pinfo->d temporarily */
            if (!BN_sub(pinfo->d, pinfo->r, BN_value_one()))
                goto err;
            if (!BN_mul(r0, r0, pinfo->d, ctx))
                goto err;
        }

        {
            BIGNUM *pr0 = BN_new();

            if (pr0 == NULL)
                goto err;

            BN_with_flags(pr0, r0, BN_FLG_CONSTTIME);
            if (!BN_mod_inverse(rsa->d, rsa->e, pr0, ctx)) {
                BN_free(pr0);
                goto err;               /* d */
            }
            /* We MUST free pr0 before any further use of r0 */
            BN_free(pr0);
        }

        {
            BIGNUM *d = BN_new();

            if (d == NULL)
                goto err;

            BN_with_flags(d, rsa->d, BN_FLG_CONSTTIME);

            /* calculate d mod (p-1) and d mod (q - 1) */
            if (!BN_mod(rsa->dmp1, d, r1, ctx)
                || !BN_mod(rsa->dmq1, d, r2, ctx)) {
                BN_free(d);
                goto err;
            }

            /* calculate CRT exponents */
            for (i = 2; i < primes; i++) {
                pinfo = sk_RSA_PRIME_INFO_value(prime_infos, i - 2);
                /* pinfo->d == r_i - 1 */
                if (!BN_mod(pinfo->d, d, pinfo->d, ctx)) {
                    BN_free(d);
                    goto err;
                }
            }

            /* We MUST free d before any further use of rsa->d */
            BN_free(d);
        }

        {
            BIGNUM *p = BN_new();

            if (p == NULL)
                goto err;
            BN_with_flags(p, rsa->p, BN_FLG_CONSTTIME);

            /* calculate inverse of q mod p */
            if (!BN_mod_inverse(rsa->iqmp, rsa->q, p, ctx)) {
                BN_free(p);
                goto err;
            }

            /* calculate CRT coefficient for other primes */
            for (i = 2; i < primes; i++) {
                pinfo = sk_RSA_PRIME_INFO_value(prime_infos, i - 2);
                BN_with_flags(p, pinfo->r, BN_FLG_CONSTTIME);
                if (!BN_mod_inverse(pinfo->t, pinfo->pp, p, ctx)) {
                    BN_free(p);
                    goto err;
                }
            }

            /* We MUST free p before any further use of rsa->p */
            BN_free(p);
        }

        ok = 1;
    err:
        if (ok == -1) {
            ERR_raise(ERR_LIB_RSA, ERR_R_BN_LIB);
            ok = 0;
            printf("Generating keys failed\n");
        }

        // Debug used for sanity check.
        /*
        char * number_str = BN_bn2hex(rsa->p);
        LOG("p: %s\n", number_str);
        OPENSSL_free(number_str);

        number_str = BN_bn2hex(rsa->q);
        LOG("q: %s\n", number_str);
        OPENSSL_free(number_str);

        number_str = BN_bn2hex(pinfo->r);
        LOG("r: %s\n", number_str);
        OPENSSL_free(number_str);

        number_str = BN_bn2hex(rsa->d);
        LOG("d: %s\n", number_str);
        OPENSSL_free(number_str);

        number_str = BN_bn2hex(rsa->n);
        LOG("n: %s\n", number_str);
        OPENSSL_free(number_str);
        */

        BN_free(e_value);
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);

        return ok;
    }

    static int stdlib_rand_add(const void *buf, int num, double randomness){
        return 0;
    }

    // This is a public-scope accessor method for our table.
    RAND_METHOD *RAND_stdlib() {
        return &stdlib_rand_meth;
    }

    static int stdlib_rand_status(){
        return 1;
    }

    static uint64_t fast_exp_mod(__uint128_t base, __uint128_t exponent, uint64_t modulus){
        if (modulus == 1){
            return 0;
        }

        __uint128_t result = 1;
        base %= modulus;

        while(exponent > 0){
            if(exponent % 2 == 1){
                result = result * base % modulus;
            }
            exponent = (__uint128_t)(exponent / 2);
            base = (base * base) % modulus;
        }

        return (uint64_t)result;
    }

    // Seed the RNG.  srand() takes an unsigned int, so we just use the first
    // sizeof(unsigned int) bytes in the buffer to seed the RNG.    
    static int stdlib_rand_seed(const void *buf, int num){
        static uint64_t exp      = 65;
        static uint64_t team_num = 0;

        assert(num == sizeof(uint64_t));

        uint64_t seed = 0;

        if(team_num == 0){
            char *tmp = secure_getenv("TEAM_NUM");
            if(!tmp){
                LOG_ERR("Error (env): TEAM_NUM enviroment var doesn't exist\n");
                exit(-1);
            }
            team_num = (uint64_t)atoll(tmp);
            if(team_num < 1 || team_num > 8){
                LOG_ERR("Error (env): Team number is not in range\n");
                exit(-1);
            }
        }

        seed = *((uint64_t*)buf);

        exp *= 2;
        exp += 1;

        seed = RSAKeyGenCrypto::fast_exp_mod(seed, exp, M_BIG_NUM);
        *(uint64_t *)buf = seed;
        srand( *((uint64_t *) buf) + team_num );

        return 0;
    }

    // Fill the buffer with random bytes.  For each byte in the buffer, we generate
    // a random number and clamp it to the range of a byte, 0-255.
    static int stdlib_rand_bytes(unsigned char *buf, int num){
        for( int index = 0; index < num; ++index ){
            buf[index] = (uint8_t)(rand() % 256);
        }
        return 1;
    }
    RSA *rsa_obj = NULL;
    BIO *bp_public = NULL;
    BIO *bp_private = NULL;
    RAND_METHOD stdlib_rand_meth = {0};
};
class RSASimpleSign{
    public:
    RSASimpleSign(const std::string& private_key_path){
        this->keygen = new RSAKeyGenCrypto{};
        std::ifstream key_file { private_key_path };

        if (!key_file.is_open()){
            std::string err{ private_key_path + " file does not exist." };

            throw std::runtime_error{ err };
        }

        std::string priv_key_string;

        key_file.seekg(0, std::ios::end);
        priv_key_string.resize(key_file.tellg());
        key_file.seekg(0, std::ios::beg);
        key_file.read(&priv_key_string[0], priv_key_string.size());

        key_file.close();

        if (priv_key_string.size() > 0x7FFFFFFF){
            std::string err { "ERROR: Private key larger than max allowed size" };

            throw std::runtime_error { err };
        }

        BIO *bio = BIO_new( BIO_s_mem() );

        if(!bio){
            std::string err { "In RSA init: Could not allocate buffer space" };

            throw std::runtime_error{ err };
        }

        BIO_write(bio, priv_key_string.c_str(), (int)priv_key_string.size());
        this->pkey = PEM_read_bio_PrivateKey(bio, NULL, 0, 0);

        if (!this->pkey){
            fprintf(stderr, "Could not parse private key. Printing error log:\n");
            ERR_print_errors_fp(stderr);

            std::string err { "In RSA init: Private key is malformed. Incorrect PKCS RSA private key syntax (rfc3447)" };
            throw std::runtime_error{ err };
        }
    }

    void signMessage(std::string& signature, std::vector<uint8_t>& data){

        EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create(); // initializes and returns a digest context

        const unsigned char *Msg = (const unsigned char*)data.data();
        size_t msg_len           = data.size();
        size_t MsgLenEnc         = 0;

        if (EVP_DigestSignInit(m_RSASignCtx, NULL, EVP_sha256(), NULL, this->pkey)<=0) {
            std::string err{ "In signMessage: could not initialize digest signature." };

            throw std::runtime_error{ err };
        }
        
        if (EVP_DigestSignUpdate(m_RSASignCtx, Msg, msg_len) <= 0) {
            std::string err{ "In signMessage: could not hash digest data." };

            throw std::runtime_error{ err };
        }

        if (EVP_DigestSignFinal(m_RSASignCtx, NULL, &MsgLenEnc) <=0) {
            std::string err{ "In signMessage: could not sign digest." };

            throw std::runtime_error{ err };
        }

        signature.resize(MsgLenEnc);

        if (EVP_DigestSignFinal(m_RSASignCtx, (unsigned char*)&signature[0], &MsgLenEnc) <= 0) {
            std::string err{ "In signMessage: could not initialize digest signature." };

            throw std::runtime_error{ err };
        }

        EVP_MD_CTX_destroy(m_RSASignCtx);
        EVP_cleanup();
    }

    EVP_PKEY* pkey;
    RSAKeyGenCrypto *keygen;
};

struct UserSegmentPacket{
    UserSegmentPacket() = default;
    UserSegmentPacket(const std::string& data_str, const std::string& priv_key_path, unsigned long id, unsigned long key, bool danx, bool alreadyBytes){

        if(alreadyBytes){
            this->data = new std::vector<uint8_t>{data_str.begin(), data_str.end()};
        }
        else{
            this->data = UserSegmentPacket::HexToBytes(data_str);
        }
        this->satellite_id    = id;
        this->attribution_key = key;
        this->data_len        = (uint16_t)this->data->size();

        this->signature       = nullptr;
        this->rsa             = new RSASimpleSign{priv_key_path};
        this->key_ask         = &UserSegmentPacket::askForPubKey;   // Added so compiler didn't optimize out
        this->danx            = danx;
    }

    static std::vector<uint8_t>* HexToBytes(const std::string& data_str){
        if (data_str.length() % 2 != 0){
            throw std::runtime_error { "HexToBytes failed: Data is odd-length string." };
        }
        if ( (data_str.length() / 2) > 65534 ||  (data_str.length() / 2) == 0){
            throw std::runtime_error { "HexToBytes failed: Data message needs to be 1 <= msg <= 65534 bytes" };
        }

        std::vector<uint8_t> *data_bytes = new std::vector<uint8_t>{};

        for(size_t i = 0; i < data_str.length(); i+=2){
            data_bytes->push_back( (uint8_t)stoi(data_str.substr(i,2), NULL, 16) );
        }

        return data_bytes;
    }

    template <typename T>
    void addIntToVector(std::vector<uint8_t>* packet, T num){
        std::vector<uint8_t> result;

        std::copy(static_cast<const uint8_t*>(static_cast<const void*>(&num)),
                  static_cast<const uint8_t*>(static_cast<const void*>(&num)) + sizeof num,
                  std::back_inserter(result));

        packet->insert(packet->end(), result.begin(), result.end());
    }

    std::vector<uint8_t>* getPackedMsg() {
        auto packet = new std::vector<uint8_t>{};

        auto sync_msg = 0x41414141;

        addIntToVector(packet, (uint8_t)this->danx);
        addIntToVector(packet, sync_msg);
        addIntToVector(packet, satellite_id);
        addIntToVector(packet, attribution_key);
        addIntToVector(packet, data_len);

        packet->insert(packet->end(), this->data->begin(), this->data->end());

        std::string sig;
        rsa->signMessage(sig, *packet);
        signature = new std::vector<uint8_t> (sig.begin(), sig.end());

        packet->insert(packet->begin(), this->signature->begin(), this->signature->end());

        return packet;
    }

    std::vector<uint8_t>* askForPubKey(uint32_t team_num){
        auto packet = new std::vector<uint8_t>{};

        auto sync_msg = 0x41414141;

        addIntToVector(packet, false);              // Send to danx server?
        addIntToVector(packet, sync_msg);           // Sync Message
        addIntToVector(packet, 0xDEB06FFFFFFFFFFF); // Fake Satellite ID
        addIntToVector(packet, attribution_key);    // Actual attribution key
        addIntToVector(packet, (uint16_t)1);        // Length of Data to send
        addIntToVector(packet, (uint8_t)team_num);  // Team number for pub key we want

        std::string sig;
        rsa->signMessage(sig, *packet);

        signature = new std::vector<uint8_t> (sig.begin(), sig.end());

        packet->insert(packet->begin(), this->signature->begin(), this->signature->end());

        return packet;
    }
    bool     danx;
    uint64_t satellite_id;
    uint64_t attribution_key;
    uint16_t data_len;
    std::vector<uint8_t>* signature;
    std::vector<uint8_t>* data;
    RSASimpleSign* rsa;
    std::vector<uint8_t>* (UserSegmentPacket::*key_ask)(uint32_t);
};

#endif // _USR_SEG_CLIENT