#include "ds18b20.h"
#include "uart.h"

bit DS18B20_Init() {
    bit presence;
    DS18B20_PIN = 0;
    Delay_us(480);
    DS18B20_PIN = 1;
    Delay_us(70);
    presence = DS18B20_PIN;
    Delay_us(410);
    return !presence;
}

static void DS18B20_WriteByte(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        DS18B20_PIN = 0;
        Delay_us(2);
        DS18B20_PIN = dat & 0x01;
        Delay_us(60);
        DS18B20_PIN = 1;
        dat >>= 1;
    }
}

static unsigned char DS18B20_ReadByte() {
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++) {
        DS18B20_PIN = 0;
        Delay_us(2);
        DS18B20_PIN = 1;
        Delay_us(12);
        dat >>= 1;
        if (DS18B20_PIN) dat |= 0x80;
        Delay_us(50);
    }
    return dat;
}

float DS18B20_GetTemp() {
    unsigned char tempL, tempH;
    short temp;
    if (!DS18B20_Init()) return -1000;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);
    Delay_ms(200);
    if (!DS18B20_Init()) return -1000;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);
    tempL = DS18B20_ReadByte();
    tempH = DS18B20_ReadByte();
    temp = ((short)tempH << 8) | tempL;
    return temp * 0.0625;
}

