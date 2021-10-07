#include <stdio.h>
#include <string.h>
#include "type.h"
#include "table.h"

int main(){
    type_t types[4] = {0,1,2,3};
    int n1 = 5, n2 = 7;
    int *ip;
    char c1[64] = "Hola", c2[64] = "mundo.";
    char *cp;
    long l1 = 2602845, l2 = 2579301;
    long *lp;
    double d1 = 3.141516, d2 = 2.681424;
    double *dp;

    printf("Valores enteros: n1 = %d, n2 = %d\n", n1,n2);
    printf("Strings: c1 = %s, c2 = %s\n", c1,c2);
    printf("Valores long: l1 = %ld, l2 = %ld\n", l1,l2);
    printf("Valores double: d1 = %lf, d2 = %lf\n", d1,d2);

    printf("Tamaño entero: %ld\n", value_length(types[0], &n1));
    printf("Tamaño string c1: %ld\n", value_length(types[1], c1));
    printf("Tamaño long: %ld\n", value_length(types[2], &l1));
    printf("Tamaño double: %ld\n", value_length(types[3], &d1));

    printf("Impresión de valores:\n");

    print_value(stdout, types[0],&n1);
    print_value(stdout, types[1],c1);
    print_value(stdout, types[2],&l1);
    print_value(stdout, types[3],&d1); 

    printf("\nComparación de valores:\n");

    printf("n1 - n2 = %d\n", value_cmp(types[0], &n1, &n2));
    printf("c1 - c2 = %d\n", value_cmp(types[1], c1, c2));
    printf("l1 - l2 = %d\n", value_cmp(types[2], &l1, &l2));
    printf("d1 - d2 = %d\n", value_cmp(types[3], &d1, &d2));

    printf("Cambio de valores:\n");

    ip = value_parse(types[0], "225");
    cp = value_parse(types[1], "mañana");
    lp = value_parse(types[2], "22598876");
    dp = value_parse(types[3], "54.321");


    print_value(stdout, types[0],ip);
    print_value(stdout, types[1],cp);
    print_value(stdout, types[2],lp);
    print_value(stdout, types[3],dp);


    return 0;  
}
