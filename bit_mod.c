#include <stdio.h>

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
