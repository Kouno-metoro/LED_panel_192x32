#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct
{
    uint *width;   // 画像の幅
    uint *height;  // 画像の高さ
    uchar *data;   // 画像データ
    uint file_ver; // バージョン
    uint seets;    // 画像の枚数
    size_t size;   // 累計サイズ
} inport_file;

typedef struct
{
    int data_R1;
    int data_G1;
    int data_B1;
    int data_A1;
    int data_R2;
    int data_G2;
    int data_B2;
    int data_A2;
} LED_buf;

const unsigned short pattern[16] = {
    0b0000000000000000,
    0b1000000000000000,
    0b1000000010000000,
    0b1000010000100000,
    0b1000100010001000,
    0b1001001001001000,
    0b1010101001010100,
    0b1010101010101010,
    0b1110101010101010,
    0b1110101011101010,
    0b1110111011101010,
    0b1110111011101110,
    0b1111111011101110,
    0b1111111011111110,
    0b1111111111111110,
    0b1111111111111111};

// ピン定義
const int G1 = 8;
const int G2 = 9;
const int R1 = 7;
const int R2 = 2;
const int B1 = 0;
const int B2 = 3;
const int A = 15;
const int B = 16;
const int C = 1;
const int D = 4;
const int CLK = 5;
const int LAT = 13;
const int OE = 12;
const int display_width = 192;
const int display_height = 32;

int count = 0;

int fileload(inport_file *infile);
void filedecode(inport_file *infile, inport_file *decodefile);
void LEDdrive(inport_file *decodefile);
void LEDdrive(inport_file *decodefile);
void calloc_check_uchar(uchar **point, size_t size);
void calloc_check_uint(uint **point, size_t size);
void pin_setup(void);
void ALLpin_LOW(void);
void LED_data_decode(inport_file *decodefile, LED_buf *LED_data, int x, int y);
void LED_control(LED_buf *LED_data);

int main(void)
{
    inport_file infile = {0};
    inport_file decodefile = {0};

    fileload(&infile);

    filedecode(&infile, &decodefile);

    pin_setup();
    ALLpin_LOW();

    while (1)
    {
        printf("drive\n");
        LEDdrive(&decodefile);
        count++;
        if(count>15){
            count=0;
        }
    }
}

int fileload(inport_file *infile)
{
    FILE *fp = NULL;
    uint space = 0;

    // ファイルオープン
    fp = fopen("data.dat", "rb");
    if (fp == NULL)
    {
        printf("FILE NULL");
        return -1;
    }

    // ファイル読み込み(ファイル形式データ)
    fread(&infile->file_ver, sizeof(uint), 1, fp);
    fread(&infile->size, sizeof(size_t), 1, fp);
    fread(&infile->seets, sizeof(uint), 1, fp);
    fread(&space, sizeof(uint), 7, fp);

    printf("%u\n",infile->file_ver);
    printf("%lu\n",infile->size);
    printf("%u\n",infile->seets);

    
    // メモリ確保
    /*calloc_check_uchar(&infile->data, infile->size);   
    calloc_check_uint(&infile->width, sizeof(uint) * infile->seets);
    calloc_check_uint(&infile->height, sizeof(uint) * infile->seets);*/

    infile->data=(uchar*)calloc(infile->size,sizeof(uchar));
    if(infile==NULL){
        printf("callocNULL");
        exit(-1);
    }

    // ファイル読み込み(ファイル本体)
    fread(infile->data, sizeof(uchar), infile->size, fp);printf("calloc1\n");
    fread(infile->width, sizeof(uint), infile->seets, fp);printf("calloc2\n");
    fread(infile->height, sizeof(uint), infile->seets, fp);printf("calloc3\n");

    fclose(fp);
}

void filedecode(inport_file *infile, inport_file *decodefile)
{
    // メモリ確保
    calloc_check_uchar(&decodefile->data, sizeof(uchar) * display_height * display_width * infile->seets * 2);

    // ファイルデコード(filedata>data)
    for (int s = 0; s < infile->seets; s++)
    {
        for (int y = 0; y < display_height; y++)
        {
            for (int x = 0; x < display_width; x++)
            {
                if ((x > (display_width - infile->width[s] - 1)) && (y > (display_height - infile->height[s] - 1)))
                {
                    decodefile->data[((s * display_width * display_height) + (y * display_width) + x) * 2] = infile->data[((s * infile->width[s] * infile->height[s]) + ((y - (display_height - infile->height[s])) * infile->width[s]) + (x - (display_width - infile->width[s]))) * 2];
                    decodefile->data[(((s * display_width * display_height) + (y * display_width) + x) * 2) + 1] = infile->data[((s * infile->width[s] * infile->height[s]) + ((y - (display_height - infile->height[s])) * infile->width[s]) + (x - (display_width - infile->width[s]))) * 2 + 1];
                }
                else
                {
                    decodefile->data[((s * display_width * display_height) + (y * display_width) + x) * 2] = 0x00;
                    decodefile->data[(((s * display_width * display_height) + (y * display_width) + x) * 2) + 1] = 0x00;
                }
            }
        }
    }
}

void LEDdrive(inport_file *decodefile)
{

    LED_buf LED_data = {0};

    for (unsigned char y = 0; y < (display_height / 2); y++)
    {

        for (int x = 0; x < display_width; x++)
        {
            LED_data_decode(decodefile, &LED_data, x, y);
            LED_control(&LED_data);

            digitalWrite(CLK, HIGH);
            delayMicroseconds(5);
            digitalWrite(CLK, LOW);
        }

        digitalWrite(A, (y & 0b00000001));
        digitalWrite(B, (y & 0b00000010) >> 1);
        digitalWrite(C, (y & 0b00000100) >> 2);
        digitalWrite(D, (y & 0b00001000) >> 3);

        digitalWrite(OE, HIGH);
        digitalWrite(LAT, HIGH);
        delayMicroseconds(1);
        digitalWrite(LAT, LOW);
        digitalWrite(OE, LOW);
    }
}

void calloc_check_uchar(uchar** point, size_t size)
{
    *point = calloc(size,sizeof(uchar));
    if (*point == NULL)
    {
        printf("calloc_check_NULL");
        exit(-2);
    }
}

void calloc_check_uint(uint** point, size_t size)
{
    *point = calloc(size,sizeof(uint));
    if (*point == NULL)
    {
        printf("calloc_check_NULL");
        exit(-2);
    }
}

void pin_setup(void)
{

    wiringPiSetup();
    pinMode(G1, OUTPUT);
    pinMode(G2, OUTPUT);
    pinMode(R1, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(B1, OUTPUT);
    pinMode(B2, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(LAT, OUTPUT);
    pinMode(OE, OUTPUT);
}

void ALLpin_LOW(void)
{
    digitalWrite(G1, LOW);
    digitalWrite(G2, LOW);
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(B1, LOW);
    digitalWrite(B2, LOW);
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
    digitalWrite(CLK, LOW);
    digitalWrite(LAT, LOW);
    digitalWrite(OE, LOW);
}

void LED_data_decode(inport_file *decodefile, LED_buf *LED_data, int x, int y)
{
    LED_data->data_R1 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2] & 0x0f;
    LED_data->data_G1 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2] & 0xf0;
    LED_data->data_B1 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1] & 0x0f;
    LED_data->data_A1 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1] & 0xf0;
    LED_data->data_R2 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + (display_height * display_width)] & 0x0f;
    LED_data->data_G2 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + (display_height * display_width)] & 0xf0;
    LED_data->data_B2 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1 + (display_height * display_width)] & 0x0f;
    LED_data->data_A2 = decodefile->data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1 + (display_height * display_width)] & 0xf0;
}

void LED_control(LED_buf *LED_data)
{

    digitalWrite(R1, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A1 != 0));
    digitalWrite(R2, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A2 != 0));
    digitalWrite(G1, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A1 != 0));
    digitalWrite(G2, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A2 != 0));
    digitalWrite(B1, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A1 != 0));
    digitalWrite(B2, ((pattern[LED_data->data_R1] >> count) & 1) * (LED_data->data_A2 != 0));
}