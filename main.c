#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rsa.h"

int main(int argc, char *argv[])
{
    int opt;
    char *plainfile = NULL, *nfile = NULL, *efile = NULL, *dfile = NULL, *cipherfile = NULL;

    while ((opt = getopt(argc, argv, "p:n:e:d:c:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            plainfile = optarg;
            break;
        case 'n':
            nfile = optarg;
            break;
        case 'e':
            efile = optarg;
            break;
        case 'd':
            dfile = optarg;
            break;
        case 'c':
            cipherfile = optarg;
            break;
        default:
            fprintf(stderr, "Usage: e3rsa [-p plainfile -n nfile [-e efile] [-d dfile] -c cipherfile]\n");
            exit(EXIT_FAILURE);
        }
    }

    if (plainfile == NULL)
    {
        // key generation into generated_txts directory (default values)
        generate_keys("generated_txts/p.txt", "generated_txts/q.txt", "generated_txts/n.txt", "generated_txts/e.txt", "generated_txts/d.txt");
    }
    else
    {
        if (nfile == NULL || cipherfile == NULL)
        {
            fprintf(stderr, "Missing arguments\n");
            exit(EXIT_FAILURE);
        }
        if (efile)
        {
            // encryption
            encrypt_file(plainfile, nfile, efile, cipherfile);
        }
        else if (dfile)
        {
            // digital signature
            sign_file(plainfile, nfile, dfile, cipherfile);
        }
        else
        {
            fprintf(stderr, "Must specify -e or -d for encryption/signature\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
