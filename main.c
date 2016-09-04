#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hide_mod.h"
#include "crypt_mod.h"

extern int keysize;
extern char* bkey;

void help(){
    printf("\nHide files / text into other files. \nInput bytes are encrypted with 128-bit AES, and then hidden into the input file.\n\nExample usage:\n./hideme -e \"foo bar\" -i funny_cat.jpg -k \"16bytekeyherelol\"\n\
\nEncrypts and hides the text \"foo bar\" into the specified \"funny_cat.jpg\" file, using the 16-byte key.\n\
To have it returned: \n\
./hideme -d -i funny_cat.jpg -k \"16bytekeyherelol\"\n\n");
}

void usage(){
	printf("\nUsage: hide [-e <text> | -d] -i <filename> -k <key>\"\n\n");
}

int main(int argc, char** argv) {
        
        int eflag = 0, dflag = 0, iflag = 0, kflag = 0;
	char key[16];
	char* file = (char*) malloc(64*sizeof(char));
	char* text = (char*) malloc(1024*sizeof(char));
        
        int opt;
        while((opt = getopt(argc, argv, "e:di:k:h")) != -1){
            FILE *f;
            switch(opt){
                case 'h':
                    help();
                    exit(0);
                    break;
                case 'e': 
                    eflag = 1;
                    strcpy(text, optarg);
                    break;
                case 'd':
                    dflag = 1;
                    break;
                case 'i':
                    iflag = 1;
                    f = fopen(optarg,"r");
                    if (f == NULL){
                        printf("\nFile '%s' does not exist.\n\n",optarg);
                        exit(-1);
                    }
                    fclose(f);
                    strcpy(file,optarg);
                    break;
                case 'k':
                    kflag = 1;
                    if(strlen(optarg) < 16){
                        printf("\nThe key must be 16 chars long.\n\n");
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
            printf("\nPlease choose either to encrypt or to decrypt!\n\n");
            return -1;
        }
        
        if(kflag == 0){
            memcpy(key,bkey,keysize);
        }

        if(iflag == 0){
            printf("\nPlease provide an input file.\n\n");
            return -1;
        }

        if (eflag == 1){
            printf("Encrypting ... \n");

            int padded_size = strlen(text);
            char* padded_text = pad (text, &padded_size);
            int e_size = encrypt_w(padded_text, padded_size, key, keysize);
           
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

        } else if(dflag == 1) {

            char* buffer = (char*) malloc(1024*sizeof(char));
            int buf_len = 0;
            
            int status = find_me(file, buffer, &buf_len);
            
            if(status == 0){
                printf("Text was found!\n");
                
                int d_size = decrypt_w(buffer, buf_len - 1, key, keysize);
                
                if(d_size == -1){
                    printf("\nUnable to decrypt.\n\n");
                    return -1;
                }else{
                    printf("%s\n",buffer);
                }

                free(buffer);

            }else {
                printf("\nSomething went wrong.\n\n");
                return 1;
            }

       }

        free(file);
        free(text);
	return 0;
}
