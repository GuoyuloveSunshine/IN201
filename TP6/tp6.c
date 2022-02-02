#include <stdio.h>
#include <stdlib.h>

int main(){
    char *mot = "+ 3 2";
    char operation[1];
    int num1;
    int num2;
    sscanf(mot,"%c %d %d", operation, &num1, &num2);
    if (*operation =='+'){
        num2 = num2+num1;
    }
    else if (*operation == '-'){
        num2 = num2-num1;
    }
    printf("%s,%d,%d",operation,num1,num2);
    return 0;
}