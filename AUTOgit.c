//#include
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

//ファイルのデータを入れる構造体
typedef struct{
    char path[300];
    char name[300];
    char Email[300];
    char URL[300];
}data;


void main(){

    //構造体の定義
    data data1;
    //gitコマンド(固定)
    const char init[]="git init";
    const char add[]="git add .";   
    const char push[]="git push -f origin master";
    //gitコマンド(合成)
    char cd[]="cd ";
    char name[]="git config --global user.name ";
    char Email[]="git config --global user.email ";
    char URL[]="git remote add origin ";
    char commit[]="git commit -m ";
    //gitコマンド(合成コマンド出力用)
    char cmdpath[300];
    char cmdname[300];
    char cmdEmail[300];
    char cmdURL[300];
    char cmdcommit[300];
    //for()用
    int i=0;
    //時間取得用
    time_t time1;
    char chartime[20];
    //入力判定用
    //int inp=0;

    //配列の初期化
    for(i=0;i<(sizeof(cmdpath)/sizeof(cmdpath[0]));i++){
    cmdpath[i]=0;
    }
    for(i=0;i<(sizeof(cmdname)/sizeof(cmdname[0]));i++){
    cmdname[i]=0;
    }
    for(i=0;i<(sizeof(cmdEmail)/sizeof(cmdEmail[0]));i++){
    cmdEmail[i]=0;
    }
    for(i=0;i<(sizeof(cmdURL)/sizeof(cmdURL[0]));i++){
    cmdURL[i]=0;
    }
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

    //ファイル読み取り
    FILE *file=fopen("gitdata.dat","rb");
    if(fopen==NULL){
        printf("ERROR");
        printf("1.5");
        return ;
    }
    fread(&data1,sizeof(data1),1,file);
    fclose(file);

    //入力判定
    /*printf("%s","A");
    scanf("%d",inp);
    if(inp=1){
        system(delete);
    }*/

    //時間取得
    time(&time1);
    struct tm *local=localtime(&time1);
    sprintf(chartime,"%04d/%02d/%02d/%02d/%02d/%02d",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

    //gitコマンド合成
    sprintf(cmdpath,"%s%s",cd,data1.path);
    sprintf(cmdname,"%s%s",name,data1.name);
    sprintf(cmdEmail,"%s%s",Email,data1.Email);
    sprintf(cmdURL,"%s%s",URL,data1.URL);
    sprintf(cmdcommit,"%s%s",commit,chartime);

   
    printf("%s\n","start");
    printf("%s\n",cmdpath);
    printf("%s\n",cmdname);
    printf("%s\n",cmdEmail);
    printf("%s\n",init);
    printf("%s\n",add);
    printf("%s\n",cmdURL);
    printf("%s\n",commit);
    printf("%s\n",push);
    printf("%s\n","end");

    //コマンド出力
    system(cmdpath);//cd "path"
    system(cmdname);//git config --global user.name "name"
    system(cmdEmail);//git config --global user.email "Email"
    system(init);//git init
    system(add);//git add .
    system(cmdURL);//git remote add origin "URL"
    system(cmdcommit);
    system(push);

    return ;
}

