/*

I compiled the program but when i gave input=600851475143 no result comes. The program is to find the largest prime factor( The prime factors of 13195 are 5, 7, 13 and 29. What is the largest prime factor of the number 600851475143 ?) What is wrong?


#include<stdio.h>

int isprime(unsigned long  n){
    unsigned long i;
    for(i=2;i<n;i++){
        if((n%i)==0){
            return 0;
        }
    }
    return 1;
}

int main(){
    unsigned long n,i,lpf;
    scanf("%ld",&n);
    for(i=2;i<n;i++){
        if(n%i==0){
            if(isprime(i)==1){
            lpf=i;}
        }
    }
    printf("%ld",lpf);
    return 0;
}

*/

#include <stdio.h>

int isprime(unsigned long n) {
    unsigned long i;
    for (i = 2; i < n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main() {
    unsigned long n, i, lpf = 0;
    scanf("%lu", &n);
    for (i = 2; i < n; i++) {
        if (n % i == 0) {
            if (isprime(i) == 1) {
                lpf = i;
            }
        }
    }
    printf("%lu", lpf);
    return 0;
}

