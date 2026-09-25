#ifndef NEO_PIXEL_H_
#define NEO_PIXEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

// 分割グループLEDの数の定義
#define GRP1_LED_COUNT  20 // グループ1: nucleo1のLED数
#define GRP2_LED_COUNT  20 // グループ2: nucleo2のLED数
#define GRP3_LED_COUNT  10 // グループ3: 常時ウェーブのLED数

// 分割グループLEDの開始インデックス定義
#define GRP1_START 0                             // グループ1: nucleo1状態表示
#define GRP2_START (GRP1_START + GRP1_LED_COUNT) // グループ2: nucleo2状態表示
#define GRP3_START (GRP2_START + GRP2_LED_COUNT) // グループ3: 常時ウェーブ表示

// 全LEDの総数
#define NUM_LEDS (GRP1_LED_COUNT + GRP2_LED_COUNT + GRP3_LED_COUNT)

// タイマークロック216MHz、ARR=269におけるデューティ設定
#define PWM_HI 90
#define PWM_LO 45

// システム状態のネオピク表示定義
typedef enum {
    STATE_NORMAL = 0,     // 正常動作     : 緑
    STATE_NET_DISCONNECT, // ルータ通信遮断: 赤
    STATE_CAN1_ERROR,     // CAN1通信遮断 : 黄
    STATE_CAN2_ERROR,     // CAN2通信遮断 : 黄
} LedState_t;

// nucleo1,2識別用
typedef enum {
    TARGET_NUCLEO1 = 0,
    TARGET_NUCLEO2 = 1
} NucleoTarget_t;

void WS2812B_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void WS2812B_SetLED(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_SetAll(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_Update(void);

void NeoPixel_SetState(NucleoTarget_t target, LedState_t new_state);

// 外部プログラムからの状態設定関数　topicで受け取ったらこれでneopixelの状態変更してね
void NeoPixel_SetNucleo1State(LedState_t new_state);
void NeoPixel_SetNucleo2State(LedState_t new_state);

#ifdef __cplusplus
}
#endif

#endif /* NEO_PIXEL_H_ */