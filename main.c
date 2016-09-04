#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hide_mod.h"
#include "crypt_mod.h"

extern int keysize;
extern char* bkey;

void usage(){
	printf("\nUsage: hide [-e <text> | -d] -i <filename> -k <key>\"\n\n");
}

int main(int argc, char** argv) {
        
        int eflag = 0, dflag = 0, iflag = 0, kflag = 0;
	char key[16];
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

        } else if (mode == 2){

            char* buffer = (char*) malloc(1024*sizeof(char));
            int buf_len = 0;
            
            int status = find_me(file, buffer, &buf_len);
            
            if(status == 0){
                printf("Text was found!\n");
                
                int d_size = decrypt_w(buffer, buf_len - 1, key, keysize);
                
                if(d_size == -1){
                    printf("BULLSHITE\n");
                    return -1;
                }else{
                    printf("%s\n",buffer);
                }
                free(buffer);

            }else {
                printf("Something went wrong.\n");
                return 1;
            }

       }

        free(file);
        free(text);
	return 0;
}
