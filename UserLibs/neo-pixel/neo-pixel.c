#include "neo-pixel.h"
#include "cmsis_os.h"

#define RESET_SLOTS 60 // 75us の Low 期間
#define TOTAL_SLOTS (NUM_LEDS * 24 + RESET_SLOTS)

static TIM_HandleTypeDef *ws_htim;
static uint32_t ws_channel;

static uint8_t pixel_data[NUM_LEDS][3];
// static uint32_t dma_buffer[TOTAL_SLOTS];
__attribute__((section(".data"))) static uint32_t dma_buffer[TOTAL_SLOTS];

// The semaphore is created by CubeMX-generated FreeRTOS initialization.
extern osSemaphoreId_t ws2812bSemHandle;

void WS2812B_Init(TIM_HandleTypeDef *htim, uint32_t channel) {
    ws_htim = htim;
    ws_channel = channel;

    __HAL_TIM_MOE_ENABLE(ws_htim);

    // 初期状態のトークンを取得して、最初の送信を開始可能にする
    if (ws2812bSemHandle != NULL) {
        osSemaphoreAcquire(ws2812bSemHandle, 0);
    }

    WS2812B_SetAll(0, 0, 0);
}

void WS2812B_SetLED(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= NUM_LEDS) return;
    pixel_data[index][0] = g; // GRB順
    pixel_data[index][1] = r;
    pixel_data[index][2] = b;
}

void WS2812B_SetAll(uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        WS2812B_SetLED(i, r, g, b);
    }
}

void WS2812B_Update(void) {
    uint32_t idx = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
        for (int c = 0; c < 3; c++) {
            uint8_t byte = pixel_data[i][c];
            for (int b = 7; b >= 0; b--) {
                dma_buffer[idx++] = (byte & (1 << b)) ? PWM_HI : PWM_LO;
            }
        }
    }

    for (int i = 0; i < RESET_SLOTS; i++) {
        dma_buffer[idx++] = 0;
    }

    // STM32F7 D-Cacheクリーン
    SCB_CleanDCache_by_Addr((uint32_t*)dma_buffer, sizeof(dma_buffer));

    // DMA転送開始
    HAL_TIM_PWM_Start_DMA(ws_htim, ws_channel, dma_buffer, TOTAL_SLOTS);
    
    // 送信完了までブロック（CPUを他のタスクへ譲渡）
    if (ws2812bSemHandle != NULL) {
        osSemaphoreAcquire(ws2812bSemHandle, osWaitForever);
    }
}

// DMA送信完了コールバック
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim == ws_htim) {
        HAL_TIM_PWM_Stop_DMA(ws_htim, ws_channel);
        if (ws2812bSemHandle != NULL) {
            osSemaphoreRelease(ws2812bSemHandle);
        }
    }
}