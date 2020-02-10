#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* base64encode(unsigned char*, int);
unsigned char* base64decode(unsigned char*, int*);

int main(int argc, char **argv)
{
    int len, i;
    unsigned char *bytes, *output;
    if (argc == 4 && strcmp(argv[1], "encode") == 0)
    {
        if (strlen(argv[2]) != strlen(argv[3]))
        {
            printf("strings must be the same length\n");
            return 1;
        }

        len = strlen(argv[2]);
        bytes = calloc(len, sizeof(unsigned char));
        for (i = 0; i < len; i++) bytes[i] = argv[2][i] ^ argv[3][i];

        printf("%s\n", base64encode(bytes, len));
    } else if (argc == 4 && strcmp(argv[1], "decode") == 0) {
        bytes = base64decode(argv[2], &len);
        
        if (len != strlen(argv[3]))
        {
            printf("strings must be the same length\n");
            return 1;
        }

        len = strlen(argv[3]);
        output = calloc(len, sizeof(unsigned char));
        for (i = 0; i < len; i++) output[i] = bytes[i] ^ argv[3][i];

        printf("%s\n", output);
    } else if (argc == 2 && strcmp(argv[1], "test") == 0) {
        unsigned char *decoded[] = {
            "any carnal pleasure.",
            "any carnal pleasure",
            "any carnal pleasur",
            "any carnal pleasu",
            "any carnal pleas"
        };

        unsigned char *encoded[] = {
            "YW55IGNhcm5hbCBwbGVhc3VyZS4=",
            "YW55IGNhcm5hbCBwbGVhc3VyZQ==",
            "YW55IGNhcm5hbCBwbGVhc3Vy",
            "YW55IGNhcm5hbCBwbGVhc3U=",
            "YW55IGNhcm5hbCBwbGVhcw=="
        };

        for (i = 0; i < 5; i++) {
            printf("%s\n", base64encode(decoded[i], strlen(decoded[i])));
            printf("%s\n", encoded[i]);
            printf("%s\n", base64decode(encoded[i], &len));
            printf("%s\n", decoded[i]);
        }
    } else {
        printf("Usage 1: engrave encode [name1] [name2]\n");
        printf("Usage 2: engrave decode [base64] [name]\n");
        printf("Usage 3: engrave test\n");
    }

    return 0;
}

unsigned char* base64decode(unsigned char *b64, int *len)
{
    int b64len, i, j;
    unsigned char *bytes, *tmp, a;

    b64len = strlen(b64);

    *len = (b64len / 4) * 3;

    tmp = calloc(b64len+1, sizeof(unsigned char));
    strcpy(tmp, b64);
    b64 = tmp;

    bytes = calloc((*len)+1, sizeof(unsigned char));

    if (b64[b64len-1] == '=') (*len)--;
    if (b64[b64len-2] == '=') (*len)--;

    for (i = 0; i < b64len; i++)
    {
        a = b64[i];

        if (a >= 'A' && a <= 'Z') b64[i] = a - 'A';
        else if (a >= 'a' && a <= 'z') b64[i] = a - 'a' + 26;
        else if (a >= '0' && a <= '9') b64[i] = a - '0' + 52;
        else if (a == '+') b64[i] = (unsigned char) 62;
        else if (a == '/') b64[i] = (unsigned char) 63;
        else if (a == '=') b64[i] = '\0'; // padding
    }

    for (i = 0, j = 0; i < b64len; i += 4, j += 3)
    {
        bytes[j] = (b64[i] << 2) | (b64[i+1] >> 4);
        bytes[j+1] = (b64[i+1] << 4) | (b64[i+2] >> 2);
        bytes[j+2] = (b64[i+2] << 6) | (b64[i+3]);
    }

    return bytes;
}

unsigned char* base64encode(unsigned char *bytes, int len)
{
    int b64len, i, j;
    unsigned int b;
    unsigned char *b64, a, b1, b2, b3;

    b64len = (len % 3 == 0) ? ((len/3) * 4) : ((len/3) * 4 + 4);
    b64 = calloc(b64len+1, sizeof(unsigned char));

    for (i = 0, j = 0; i < b64len; i += 4, j += 3)
    {
        b1 = bytes[j];
        b2 = (len < j+1) ? '\0' : bytes[j+1];
        b3 = (len < j+2) ? '\0' : bytes[j+2];

        b = (b1 << 16) | (b2 << 8) | b3;
        b64[i+0] = (b >> 18) & 0x3F;
        b64[i+1] = (b >> 12) & 0x3F;
        b64[i+2] = (b >>  6) & 0x3F;
        b64[i+3] = (b >>  0) & 0x3F;

        if (len < j+2) b64[i+2] = 0xFF;
        if (len < j+3) b64[i+3] = 0xFF;
    }

    for (i = 0; i < b64len; i++)
    {
        a = b64[i];

        if (a < 26) b64[i] = a + 'A';
        else if (a < 52) b64[i] = (a - 26) + 'a';
        else if (a < 62) b64[i] = (a - 52) + '0';
        else if (a == 62) b64[i] = '+';
        else if (a == 63) b64[i] = '/';
        else if (a == 0xFF) b64[i] = '='; // padding
    }

    return b64;
}
