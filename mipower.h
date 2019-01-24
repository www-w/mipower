#pragma once
#include <stc12.h>
#include <stdint.h>

#define IRPIN P1_3
#define LED P1_2
#define POWER P1_7

// #define DEBUG // 串口输出红外编码数据

//红外编码数据
//引导：9ms_04.7ms_1
//逻辑0: 0.5ms_0.5ms- 逻辑1：0.5ms_1.7ms-
//8位厂商码+8位反码
//8位数据+8位反码
typedef union{
	uint32_t int32;
	uint8_t int8[4];
} unionulong;

#define BTNRED 0x1CE3926D
#define BTNGRE 0x1CE352AD
#define BTNYEL 0x1CE3D22D
#define BTNBLU 0x1CE332CD


#define MINUTES(TT) (TT*60*20)
