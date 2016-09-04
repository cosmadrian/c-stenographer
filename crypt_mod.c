#include <mcrypt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


static char* IV = "a01bef82135985d8";
char* bkey = "aj72hc0s9a82jvnb";

int keysize = 16;
static unsigned char pad_c = 0;

static int encrypt(char* buffer,int buffer_len,char* IV,char* key,int key_len){
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);


    if (buffer_len % blocksize != 0)
        return -1;
    
    mcrypt_generic_init(td,key,key_len, IV);
    mcrypt_generic(td,buffer,buffer_len);
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    return buffer_len;
}

int encrypt_w(char* buffer, int buf_size, char* key, int keysize){
    return encrypt(buffer, buf_size, IV, key, keysize);
}


static int decrypt(char* buffer,int buffer_len,char* IV,char* key,int key_len){
    
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);

    if(buffer_len % blocksize != 0){
        return -1;
    }

    mcrypt_generic_init(td, key, key_len, IV);
    mdecrypt_generic(td, buffer, buffer_len);
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);
    
    return buffer_len;
}

int decrypt_w(char* buffer, int buf_size, char* key, int keysize){
    return decrypt(buffer, buf_size, IV, key, keysize);
}


char* pad(char* bytes, int* bsize){

    int pad_size = 0;

    if (*bsize < keysize){
        pad_size = keysize - *bsize;

    } else if (*bsize > keysize) {
        pad_size = keysize - *bsize % keysize;
    
    }else{
        pad_size = keysize;
    }

    char* buffer = (char*) calloc(1, *bsize + pad_size + 1);

    int i;
    memcpy(buffer, bytes, *bsize);

    for(i = 0; i < pad_size; i++){
        buffer[*bsize + i] = pad_c;
    }
    *bsize += pad_size;

    return buffer;
}


int unpad(char* buffer, int buffer_len){

    int i = 0;
    int j = buffer_len;
    
    while(buffer[i] == pad_c){
        i++;
    }
    while(buffer[j-1] == pad_c){
        j--;
    }

    int k;
    for(k = 0; k < j - i; k++){
        buffer[k] = buffer[i+k];
    }

    buffer = (char*)realloc(buffer, j - i);
    return j - i;
}
