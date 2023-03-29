#include<stdio.h>
#include<wiringPi.h>
#include <pthread.h>
const int pinnum=27;

int main(void){
    printf("helloworld!!\n");

    wiringPiSetup();
    pinMode(pinnum,OUTPUT);

    for(;;){
        digitalWrite(pinnum,HIGH);
        delay(100);
        digitalWrite(pinnum,LOW);
        delay(100);
    }
    
}