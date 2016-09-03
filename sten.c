#include <mcrypt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char* IV = "a01bef82135985d8";
char key[16];
char* bkey = "aj72hc0s9a82jvnb"; // default key

int keysize = 16;
unsigned char pad_c = 0;

int get_bit(char b, int num){
    return (b & (1 << num)) >> num;
}

char change_bit(char b, int num, int bit_val){
    return b ^ (-bit_val ^ b) & (1 << num);
}

void print_bits(char b){
	int j = 0;;
	for(j=7;j>=0;j--){
            	printf("%d",(b >> j) & 0x01);
        }
        printf("\n");
}

void display(char* ciphertext, int len){
	int v;
	for (v=0; v<len; v++){
		printf("%x ",ciphertext[v]);		
	}
	printf("\n");
}


int encrypt(char* buffer,int buffer_len,char* IV,char* key,int key_len){
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

int encrypt_w(char* buffer, int buf_size){
    return encrypt(buffer, buf_size, IV, key, keysize);
}


int decrypt(char* buffer,int buffer_len,char* IV,char* key,int key_len){
    
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

int decrypt_w(char* buffer, int buf_size){
    return decrypt(buffer, buf_size, IV, key, keysize);
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

char* pad(char* bytes, int* bsize){

    int pad_size = 0;

    if (*bsize < keysize){
        pad_size = keysize - *bsize;

    } else if (*bsize > keysize) {
        pad_size = keysize - *bsize % keysize;
    
    }else{
        pad_size = keysize;
    }

    printf("PADSIZE: %d\n\n",pad_size);

    char* buffer = (char*) calloc(1, *bsize + pad_size + 1);

    int i;
    memcpy(buffer, bytes, *bsize);

    for(i = 0; i < pad_size; i++){
        buffer[*bsize + i] = pad_c;
    }
    *bsize += pad_size;

    return buffer;
}


int hide_me(char* filename, char* buffer, int buf_len){

    FILE *f = fopen(filename, "r+b");
    int i,j;
    char byte;
    char* pbuffer = (char*)malloc((buf_len + 1)*sizeof(char));
    memcpy(pbuffer,buffer,buf_len);
    pbuffer[buf_len] = 0;

    for(i = 0; i < buf_len + 1; i++){
        //print_bits(pbuffer[i]);
        for(j=0; j < 8; j ++){
            int bit1 = get_bit(pbuffer[i], j);
            j++;
            int bit2 = get_bit(pbuffer[i], j);
            
          //  printf("Got %d%d\n",bit1,bit2);

            fread(&byte, 1, 1, f);

            //printf("Changing...\n");
            //print_bits(byte);
            
            byte = change_bit(byte, 0, bit1);
            byte = change_bit(byte, 1, bit2);
            print_bits(byte);

            //printf("############\n");
           
            fseek(f, -1, SEEK_CUR);
            fwrite(&byte, 1, 1, f);
        }
    }
    free(pbuffer);

    fclose(f);
    return 0;
}

int find_me(char* filename, char* buffer, int* buf_len){
    FILE *f = fopen(filename, "r+b");
    int i,j;

    buffer = (char*)realloc(buffer, 1024*sizeof(char));

    char rbyte = NULL;

    for(i =0; i < 64; i++){
        fread(&rbyte, 1, 1, f);
        print_bits(rbyte);
    } 


    fclose(f);

    return 0;
}

void usage(){
	printf("\nUsage: hide [-e <text> | -d] -i <filename> -k <key>\"\n\n");
}

int main(int argc, char** argv) {
        
        int eflag = 0, dflag = 0, iflag = 0, kflag = 0;

	int mode = 0; 
	char* file = (char*) malloc(64*sizeof(char));
	char* text = (char*) malloc(1024*sizeof(char));
        
        int opt;
        while((opt = getopt(argc, argv, "e:di:k:")) != -1){
            FILE *f;
            switch(opt){
                case 'e': 
                    eflag = 1;
                    mode = 1;
                    strcpy(text, optarg);
                    break;
                case 'd':
                    dflag = 1;
                    mode = 2;
                    break;
                case 'i':
                    iflag = 1;
                    f = fopen(optarg,"r");
                    if (f == NULL){
                        printf("File '%s' does not exist.\n",optarg);
                        exit(-1);
                    }
                    fclose(f);
                    strcpy(file,optarg);
                    break;
                case 'k':
                    kflag = 1;
                    if(strlen(optarg) < 16){
                        printf("The key must be 16 chars long.\n");
                        exit(-1);
                    }
                    memcpy(key,optarg,keysize);
                    break;
                default:
                    usage();
                    exit(-1);
            }
        }

        if(eflag - dflag == 0){
            printf("Please choose either to encrypt or to decrypt!\n");
            return -1;
        }
        
        if(kflag == 0){
            memcpy(key,bkey,keysize);
        }

        if(iflag == 0){
            printf("Please provide an input file.\n");
            return -1;
        }

        if (mode == 1){
            printf("Encrypting ... \n");

            int padded_size = strlen(text);
            char* padded_text = pad (text, &padded_size);
            int e_size = encrypt_w(padded_text, padded_size);
           
            if(e_size == -1){
                printf("Incorrect padding ... \n");
                return -1;
            }

            int status = hide_me(file, padded_text, e_size);

            if (status == 0){
                printf("Text was hidden.\n");
            } else {
                printf("Error while hiding... \n");
            }

            free(padded_text);

        } else if (mode == 2){

            char* buffer = NULL;
            int buf_len;
            // check file type
            
            int status = find_me(file, buffer, &buf_len);
            // decrypt
            
            int d_size = decrypt_w(buffer, &buf_len);
            display(buffer, d_size);
        }

        free(file);
        free(text);
	return 0;
}
