#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

// Local Openssl includes
#include "internal/deprecated.h"
#include "internal/cryptlib.h"
#include "prov/providercommon.h"
#include "rsa_local.h"

// System Openssl Includes
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/self_test.h>

#define TRUE  1
#define FALSE 0

#define KEY_BITS         172+172+1028
#define M_BIG_NUM        3930574699
#define PRIV_KEYS_FOLDER "../../configs/team_private_keys"
#define PUB_KEYS_FOLDER  "../../configs/team_public_keys"

#define DEBUG 1
#if DEBUG
    #define LOG printf
#else
    #define LOG(...) while(0){}
#endif

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)

static int stdlib_rand_add(const void *buf, int num, double randomness);
static int stdlib_rand_status();
static int stdlib_rand_seed(const void *buf, int num);
static int stdlib_rand_bytes(unsigned char *buf, int num);
static uint64_t fast_exp_mod(__uint128_t base, __uint128_t exponent, uint64_t modulus);

RAND_METHOD stdlib_rand_meth = {
    .seed       = stdlib_rand_seed,
    .bytes      = stdlib_rand_bytes,
    .cleanup    = NULL,
    .add        = stdlib_rand_add,
    .pseudorand = stdlib_rand_bytes,
    .status     = stdlib_rand_status
};

int RSAGenerateKey(RSA *rsa){        
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
    char *random_num = getenv("TEAM_RANDOM_NUM");
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

    LOG("random_num: %s | rnd_seed: %lu \n", random_num, rnd_seed);
    rnd_seed = fast_exp_mod(rnd_seed, 6, M_BIG_NUM);

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
        LOG("Generating keys failed\n");
    }

    // DEBUG TAKE OUT
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

    BN_free(e_value);
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);

    return ok;
}

void free_all_rsa_goodies(RSA *rsa, BIO *bp_public, BIO *bp_private){
    RSA_free(rsa);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
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

// Seed the RNG.  srand() takes an uint64_t, so we just use the first
// sizeof(uint64_t) bytes in the buffer to seed the RNG.
static int stdlib_rand_seed(const void *buf, int num){
    static uint64_t exp      = 65;
    static uint64_t team_num = 0;
    static int test_count = 0;

    assert(num == sizeof(uint64_t));

    uint64_t seed = 0;

    if(team_num == 0){
        char *tmp = getenv("TEAM_NUM");
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

    seed = fast_exp_mod(seed, exp, M_BIG_NUM);
    *(uint64_t *)buf = seed;
    srand( *((uint64_t *) buf) + team_num );

    test_count += 1;
    if (test_count == 3 && seed != 1){
        LOG_ERR("seed round 3 is not 1: %ld\n", seed);
        exit(-1);
    }

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

void check_folder_exists_fatal(const char *path){
    struct stat sb;

    if(!(stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))){
        LOG_ERR("Error: Make sure %s folder exists\n", path);
        exit(-1);
    }
}

int main(int argc, char **argv){
    RSA *rsa = NULL;
    BIO *bp_public = NULL, *bp_private = NULL;
    char path[100] = {0};
    const char *team_num_str = NULL, *pubkeysfolder=PUB_KEYS_FOLDER, *privkeysfolder=PRIV_KEYS_FOLDER;
    uint64_t team_num = 0;

    if(argc == 3){
        pubkeysfolder  = argv[1];
        privkeysfolder = argv[2];
    }

    check_folder_exists_fatal(pubkeysfolder);
    check_folder_exists_fatal(privkeysfolder);

    team_num_str = getenv("TEAM_NUM");
    if(!team_num_str){
        LOG_ERR("Error (env): TEAM_NUM enviroment var doesn't exist\n");
        exit(-1);
    }
    team_num = (uint64_t)atoll(team_num_str);
    if(team_num < 1 || team_num > 8){
        LOG_ERR("Error (env): Team number is not in range\n");
        exit(-1);
    }

    LOG("team_num: %lu\n", team_num);

    rsa = RSA_new();

    RAND_set_rand_method(RAND_stdlib());

    int ret = 0;

    if((ret = RSAGenerateKey(rsa)) != 1){
        LOG("(0) Error: Generating key. Got ret: (%d)\n", ret);
        free_all_rsa_goodies(rsa, bp_public, bp_private);
        exit(-1);
    }

    snprintf(path, 100, "%s/team_%s_rsa_pub.pem", pubkeysfolder, team_num_str);
    bp_public = BIO_new_file(path, "w+");

    if(bp_public == NULL){
        LOG("(1) Error: Creating new file. Does the path to the folder exist?\n");
        free_all_rsa_goodies(rsa, bp_public, bp_private);
        exit(-1);
    }

    if((ret = PEM_write_bio_RSAPublicKey(bp_public, rsa)) != 1){
        LOG("(2) Error: Writing public key to a file.\n");
        free_all_rsa_goodies(rsa, bp_public, bp_private);
        exit(-1);
    }

    BIO_free(bp_public);

    memset(path, '\0', 100);
    snprintf(path, 100, "%s/team_%s_rsa_priv.pem", privkeysfolder, team_num_str);

    bp_private = BIO_new_file(path, "w+");

    if(bp_private == NULL){
        LOG("(3) Error: Creating new file. Does the path to the folder exist?\n");
        free_all_rsa_goodies(rsa, bp_public, bp_private);
        exit(-1);
    }

    if((ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL)) != 1){
        LOG("(4) Error: Writing the private key to a file.\n");
        free_all_rsa_goodies(rsa, bp_public, bp_private);
        exit(-1);
    }

    return 0;
}