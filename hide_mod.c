#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bit_mod.h"


int hide_me(char* filename, char* buffer, int buf_len){

    FILE *f = fopen(filename, "r+b");
    int i,j;
    char byte;
    char* pbuffer = (char*)malloc((buf_len + 1)*sizeof(char));
    memcpy(pbuffer,buffer,buf_len);
    pbuffer[buf_len] = 0;

    for(i = 0; i < buf_len + 1; i++){
        for(j=0; j < 8; j ++){
            int bit1 = get_bit(pbuffer[i], j);
            j++;
            int bit2 = get_bit(pbuffer[i], j);
            
            fread(&byte, 1, 1, f);
            
            byte = change_bit(byte, 0, bit1);
            byte = change_bit(byte, 1, bit2);

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

    char rbyte;
    do{
        rbyte = '\0';
        char sbytes[4];
        fread(&sbytes, 1, 4, f);
        int i, j = 0;
        for(i = 0; i < 4; i++){
            int bit1 = get_bit(sbytes[i], 0);
            int bit2 = get_bit(sbytes[i], 1);
            
            rbyte = change_bit(rbyte, j, bit1);
            j++;
            rbyte = change_bit(rbyte, j, bit2);
            j++;
        }

        buffer[*buf_len] = rbyte;
        *buf_len += 1;

    }while(rbyte != '\0' && !feof(f));

    fclose(f);

    return 0;
}
