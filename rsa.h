#ifndef RSA_H
#define RSA_H

#include <gmp.h>

void generate_keys(const char *pfile, const char *qfile, const char *nfile, const char *efile, const char *dfile);
void encrypt_file(const char *plainfile, const char *nfile, const char *efile, const char *cipherfile);
void sign_file(const char *plainfile, const char *nfile, const char *dfile, const char *cipherfile);

#endif // RSA_H
