#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

// normalize hex string: convert uppercase A-F to lowercase
static void normalize_hex(char *s)
{
    for (; *s; ++s)
    {
        if (*s >= 'A' && *s <= 'F')
            *s = *s - 'A' + 'a';
    }
}

void generate_keys(const char *pfile, const char *qfile, const char *nfile, const char *efile, const char *dfile)
{
    // ensure output directory exists
    if (mkdir("generated_txts", 0755) != 0 && errno != EEXIST)
    {
        perror("Failed to create txts directory");
        exit(EXIT_FAILURE);
    }
    mpz_t p, q, n, phi, e, d, t1, t2, gcd;
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, (unsigned long)time(NULL));

    mpz_inits(p, q, n, phi, e, d, t1, t2, gcd, NULL);
    // generate p
    do
    {
        mpz_urandomb(p, state, 64);
    } while (mpz_cmp_ui(p, 10000000000UL) <= 0);
    mpz_nextprime(p, p);
    // generate q
    do
    {
        mpz_urandomb(q, state, 64);
    } while (mpz_cmp_ui(q, 10000000000UL) <= 0);
    mpz_nextprime(q, q);
    // n = p * q
    mpz_mul(n, p, q);
    // phi = (p-1)*(q-1)
    mpz_sub_ui(t1, p, 1);
    mpz_sub_ui(t2, q, 1);
    mpz_mul(phi, t1, t2);
    // choose e
    mpz_set_ui(e, 65537);
    mpz_gcd(gcd, e, phi);
    while (mpz_cmp_ui(gcd, 1) != 0)
    {
        mpz_nextprime(e, e);
        mpz_gcd(gcd, e, phi);
    }
    // compute d
    if (mpz_invert(d, e, phi) == 0)
    {
        fprintf(stderr, "Error computing inverse\n");
        exit(1);
    }
    // write to files in hex
    char *s;
    FILE *f;
    // p
    s = mpz_get_str(NULL, 16, p);
    f = fopen(pfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    // q
    s = mpz_get_str(NULL, 16, q);
    f = fopen(qfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    // n
    s = mpz_get_str(NULL, 16, n);
    f = fopen(nfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    // e
    s = mpz_get_str(NULL, 16, e);
    f = fopen(efile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    // d
    s = mpz_get_str(NULL, 16, d);
    f = fopen(dfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);

    mpz_clears(p, q, n, phi, e, d, t1, t2, gcd, NULL);
    gmp_randclear(state);
}

void encrypt_file(const char *plainfile, const char *nfile, const char *efile, const char *cipherfile)
{
    mpz_t m, n, e, c;
    mpz_inits(m, n, e, c, NULL);
    char buf[1024];
    FILE *f;
    // read plaintext
    f = fopen(plainfile, "r");
    if (!f)
    {
        perror("fopen plaintext");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read plaintext\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(m, buf, 16);
    // read n
    f = fopen(nfile, "r");
    if (!f)
    {
        perror("fopen nfile");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read n\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(n, buf, 16);
    // read e
    f = fopen(efile, "r");
    if (!f)
    {
        perror("fopen efile");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read e\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(e, buf, 16);
    // c = m^e mod n
    mpz_powm(c, m, e, n);
    // write ciphertext
    char *s = mpz_get_str(NULL, 16, c);
    f = fopen(cipherfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    mpz_clears(m, n, e, c, NULL);
}

void sign_file(const char *plainfile, const char *nfile, const char *dfile, const char *cipherfile)
{
    mpz_t m, n, d, sgn;
    mpz_inits(m, n, d, sgn, NULL);
    char buf[1024];
    FILE *f;
    // read "plaintext"
    f = fopen(plainfile, "r");
    if (!f)
    {
        perror("fopen plaintext");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read plaintext for signature\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(m, buf, 16);
    // read n
    f = fopen(nfile, "r");
    if (!f)
    {
        perror("fopen nfile");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read n for signature\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(n, buf, 16);
    // read d
    f = fopen(dfile, "r");
    if (!f)
    {
        perror("fopen dfile");
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%1023s", buf) != 1)
    {
        fprintf(stderr, "Failed to read d for signature\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    normalize_hex(buf);
    mpz_set_str(d, buf, 16);
    // signature = m^d mod n
    mpz_powm(sgn, m, d, n);
    // write signature
    char *s = mpz_get_str(NULL, 16, sgn);
    f = fopen(cipherfile, "w");
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
    mpz_clears(m, n, d, sgn, NULL);
}
