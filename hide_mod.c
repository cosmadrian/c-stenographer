#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bit_mod.h"


static int get_header_length(char* extension){
    if(strcmp(extension, "txt") == 0 || strcmp(extension, "") == 0){
        return 0;
    }else{
        return 0;
    }
}

static void get_extension(char* filename, char* extension){
    int i = 0, j = 0;
    for(i = strlen(filename) - 1; i >= 0; i--)
        if(filename[i] == '.'){
            i++;
            break;
        }
    for(;i < strlen(filename) && j < 5; i++, j++)
        extension[j] = filename[i];

    printf("Extension found: \"%s\"\n\n",extension);
}

struct ex_file{
    FILE* stream;
    int header_length;
};

static void ex_file_close(struct ex_file* f){
    fclose(f->stream);
    free(f);
}

static struct ex_file* ex_file_open(char* filename) {
    struct ex_file *f = (struct ex_file*)malloc(sizeof(struct ex_file));
    f->stream = fopen(filename, "r+b");
    if(f->stream == NULL){
        ex_file_close(f);
        return NULL;
    }
    char extension[5];
    memset(extension, 0, 5);

    get_extension(filename, extension);
    
    f->header_length = get_header_length(extension);
    fseek(f->stream,f->header_length,SEEK_SET);

    return f;
}


int hide_me(char* filename, char* buffer, int buf_len){

    struct ex_file* f = ex_file_open(filename);

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
            
            fread(&byte, 1, 1, f->stream);
            
            byte = change_bit(byte, 0, bit1);
            byte = change_bit(byte, 1, bit2);

            fseek(f->stream, -1, SEEK_CUR);
            fwrite(&byte, 1, 1, f->stream);
        }
    }
    free(pbuffer);

    ex_file_close(f);
    return 0;
}

int find_me(char* filename, char* buffer, int* buf_len){

    struct ex_file* f = ex_file_open(filename);

    char rbyte;
    do{
        rbyte = '\0';
        char sbytes[4];
        fread(&sbytes, 1, 4, f->stream);
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

    }while(rbyte != '\0' && !feof(f->stream));

    ex_file_close(f);

    return 0;
}
