#ifndef NEO_PIXEL_H_
#define NEO_PIXEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

// 接続するLEDの数
#define NUM_LEDS 15

// タイマークロック216MHz、ARR=269におけるデューティ設定
#define PWM_HI 180
#define PWM_LO 90

// システム状態のネオピク表示定義
typedef enum {
    LED_STATE_NORMAL = 0,     // 通常動作
    LED_STATE_NET_DISCONNECT, // ルータ通信遮断
    LED_STATE_CAN1_ERROR,     // CAN1通信遮断
    LED_STATE_CAN2_ERROR      // CAN2通信遮断
} LedSystemState_t;

void WS2812B_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void WS2812B_SetLED(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_SetAll(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_Update(void);
void NeoPixel_SetSystemState(LedSystemState_t new_state); // 外部プログラムからシステム状態を設定するための関数

#ifdef __cplusplus
}
#endif

#endif /* NEO_PIXEL_H_ */