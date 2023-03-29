#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

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

void pin_setup(void);
void ALLpin_LOW(void);

int main(void)
{
    // 変数宣言
    int width = 0;
    int height = 0;
    int file_ver = 0;
    int seets = 0;
    unsigned char *file_data = NULL;
    unsigned char *data = NULL;
    unsigned char data_R1 = 0;
    unsigned char data_R2 = 0;
    unsigned char data_G1 = 0;
    unsigned char data_G2 = 0;
    unsigned char data_B1 = 0;
    unsigned char data_B2 = 0;
    unsigned char data_A1 = 0;
    unsigned char data_A2 = 0;
    FILE *fp = NULL;

    int debug = 0;

    // ファイルオープン
    fp = fopen("data.dat", "rb");
    if (fp == NULL)
    {
        printf("FILE NULL");
        return -1;
    }

    // ファイル読み込み(ファイル形式データ)
    fread(&file_ver, sizeof(int), 1, fp);
    fread(&width, sizeof(int), 1, fp);
    fread(&height, sizeof(int), 1, fp);
    fread(&seets, sizeof(int), 1, fp);

    // メモリ確保
    file_data = (unsigned char *)calloc((size_t)width * height * seets * 2, sizeof(unsigned char));
    if (file_data == NULL)
    {
        printf("calloc NULL");
        return -2;
    }

    data = (unsigned char *)calloc((size_t)display_width * display_height * seets * 2, sizeof(unsigned char));
    if (data == NULL)
    {
        printf("calloc NULL");
        return -3;
    }

    // ファイル読み込み(ファイル本体)
    fread(file_data, sizeof(unsigned char), (size_t)width * height * seets * 2, fp);

    // ファイルデコード(filedata>data)
    for (int s = 0; s < seets; s++)
    {
        for (int y = 0; y < display_height; y++)
        {
            for (int x = 0; x < display_width; x++)
            {
                if ((x > (display_width - width - 1)) && (y > (display_height - height - 1)))
                {
                    data[((s * display_width * display_height) + (y * display_width) + x) * 2] = file_data[((s * width * height) + ((y - (display_height - height)) * width) + (x - (display_width - width))) * 2];
                    data[(((s * display_width * display_height) + (y * display_width) + x) * 2) + 1] = file_data[((s * width * height) + ((y - (display_height - height)) * width) + (x - (display_width - width))) * 2 + 1];
                }
                else
                {
                    data[((s * display_width * display_height) + (y * display_width) + x) * 2] = 0x00;
                    data[(((s * display_width * display_height) + (y * display_width) + x) * 2) + 1] = 0x00;
                }
            }
        }
    }

    pin_setup();
    ALLpin_LOW();
    while (1)
    {
        for (unsigned char y = 0; y < (display_height / 2); y++)
        {

            for (int x = 0; x < display_width; x++)
            {
                data_R1 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2] & 0x0f;
                data_G1 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2] & 0xf0;
                data_B1 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1] & 0x0f;
                data_A1 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1] & 0xf0;
                data_R2 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + (display_height * display_width)] & 0x0f;
                data_G2 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + (display_height * display_width)] & 0xf0;
                data_B2 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1 + (display_height * display_width)] & 0x0f;
                data_A2 = data[((0 * display_width * display_height) + (y * display_width) + x) * 2 + 1 + (display_height * display_width)] & 0xf0;

                if ((data_R1 != 0) && (data_A1 != 0))
                {
                    digitalWrite(R1, HIGH);
                }
                else
                {
                    digitalWrite(R1, LOW);
                }
                if ((data_G1 != 0) && (data_A1 != 0))
                {
                    digitalWrite(G1, HIGH);
                }
                else
                {
                    digitalWrite(G1, LOW);
                }
                if ((data_B1 != 0) && (data_A1 != 0))
                {
                    digitalWrite(B1, HIGH);
                }
                else
                {
                    digitalWrite(B1, LOW);
                }
                if ((data_R2 != 0) && (data_A2 != 0))
                {
                    digitalWrite(R2, HIGH);
                }
                else
                {
                    digitalWrite(R2, LOW);
                }
                if ((data_G2 != 0) && (data_A2 != 0))
                {
                    digitalWrite(G2, HIGH);
                }
                else
                {
                    digitalWrite(G2, LOW);
                }
                if ((data_B2 != 0) && (data_A2 != 0))
                {
                    digitalWrite(B2, HIGH);
                }
                else
                {
                    digitalWrite(B2, LOW);
                }
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