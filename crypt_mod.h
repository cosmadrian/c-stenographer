#ifndef __CRYPT_MOD_H__
#define __CRYPT_MOD_H__

int encrypt_w(char*, int, char*, int);
int decrypt_w(char*, int, char*, int);
char* pad(char*, int*);
int unpad(char*, int);

#endif
