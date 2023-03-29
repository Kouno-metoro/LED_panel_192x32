//#include
#include<stdio.h>
#include<string.h>
#include<unistd.h>

//ファイル書き込み用構造体
typedef struct{
    char path[300];
    char name[300];
    char Email[300];
    char URL[300];
}data;

int main(void){
    //構造体宣言
    data data1;
    //for()用
    int i;

    //配列の初期化
    for(i=0;i<(sizeof(data1.path)/sizeof(data1.path[0]));i++){
    data1.path[i]=0;
    }
    for(i=0;i<(sizeof(data1.name)/sizeof(data1.name[0]));i++){
    data1.name[i]=0;
    }
    for(i=0;i<(sizeof(data1.Email)/sizeof(data1.Email[0]));i++){
    data1.Email[i]=0;
    }
    for(i=0;i<(sizeof(data1.URL)/sizeof(data1.URL[0]));i++){
    data1.URL[i]=0;
    }
    
    //入力
    printf("path?\n");
    gets(data1.path);
    printf("name?\n");
    gets(data1.name);
    printf("Email?\n");
    gets(data1.Email);
    printf("URL?\n");
    gets(data1.URL);

    //ファイル書き込み
    FILE *fp=fopen("gitdata.dat","wb");
    if(fp==NULL){
        printf("ERROR");
        return 0;
    }
    fwrite(&data1,sizeof(data1),1,fp);
    fclose(fp);
    return 0;
}
