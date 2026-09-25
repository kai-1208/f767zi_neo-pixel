#include "main.h"
#include "cmsis_os.h"
#include "neo-pixel.h"

// 使用するタイマー (TIM2)
extern TIM_HandleTypeDef htim2;

// それぞれのnucleoの現在状態
static volatile LedState_t state_nucleo1 = STATE_NORMAL;
static volatile LedState_t state_nucleo2 = STATE_NORMAL;

// 外部プログラムからの状態設定関数
extern "C" void NeoPixel_SetState(NucleoTarget_t target, LedState_t new_state)
{
    if (target == TARGET_NUCLEO1) {
        state_nucleo1 = new_state;
    } else if (target == TARGET_NUCLEO2) {
        state_nucleo2 = new_state;
    }
}

extern "C" void NeoPixel_SetNucleo1State(LedState_t new_state) {
    NeoPixel_SetState(TARGET_NUCLEO1, new_state);
}

extern "C" void NeoPixel_SetNucleo2State(LedState_t new_state) {
    NeoPixel_SetState(TARGET_NUCLEO2, new_state);
}

// 状態に応じた色を取得する関数　グループ1,2の表示用
static void GetStatusColor(LedState_t state, uint32_t timer_ms, uint8_t &r, uint8_t &g, uint8_t &b)
{
    r = 0;
    g = 0;
    b = 0;

    switch (state)
    {
        case STATE_NET_DISCONNECT: {
            // ルータ通信遮断: 赤色で点滅
            bool on = ((timer_ms / 100) % 2) == 0;
            if (on) r = 127;
            break;
        }
        case STATE_CAN1_ERROR: {
            // CAN1通信遮断: 黄色で点滅
            bool on = ((timer_ms / 250) % 2) == 0;
            if (on) { r = 127; g = 127; }
            break;
        }
        case STATE_CAN2_ERROR: {
            // CAN2通信遮断: オレンジ色で点滅
            bool on = ((timer_ms / 250) % 2) == 0;
            if (on) { r = 127; g = 63; }
            break;
        }
        case STATE_NORMAL:
        default:
            // 正常時: 白色
            r = 80;
            g = 80;
            b = 80;
            break;
    }
}

// グループ3の表示用設定
static const uint8_t  WAVE_COLOR_R  = 0;
static const uint8_t  WAVE_COLOR_G  = 255;
static const uint8_t  WAVE_COLOR_B  = 255;
static const uint32_t WAVE_SPEED_MS = 60;    // 波が進む速さ [ms]
static const float    WAVE_FADE     = 0.82f; // 暗くなる速さ（減衰率）

// 描画fps設定
static const uint32_t FRAME_DELAY_MS = 20; // [ms]

// メインタスク関数
extern "C" void StartNeoPixelTask(void *argument)
{
    // neopixelの初期化（PA0 / TIM2 Channel 1）
    WS2812B_Init(&htim2, TIM_CHANNEL_1);

    // グループ3の明るさバッファ
    float wave_brightness[GRP3_LED_COUNT] = {0.0f};
    uint16_t wave_head = 0;
    uint32_t wave_timer_ms = 0;

    // 点滅計算用のミリ秒タイマー
    uint32_t blink_timer_ms = 0;

    for(;;)
    {
        blink_timer_ms += FRAME_DELAY_MS;

        // グループ1の描画: nucleo1状態表示
        uint8_t r1, g1, b1;
        GetStatusColor(state_nucleo1, blink_timer_ms, r1, g1, b1);
        for (int i = 0; i < GRP1_LED_COUNT; i++) {
            WS2812B_SetLED(GRP1_START + i, r1, g1, b1);
        }

        // グループ2の描画: nucleo2状態表示
        uint8_t r2, g2, b2;
        GetStatusColor(state_nucleo2, blink_timer_ms, r2, g2, b2);
        for (int i = 0; i < GRP2_LED_COUNT; i++) {
            WS2812B_SetLED(GRP2_START + i, r2, g2, b2);
        }

        // グループ3の描画: 常時ウェーブ表示
        wave_timer_ms += FRAME_DELAY_MS;
        if (wave_timer_ms >= WAVE_SPEED_MS)
        {
            wave_timer_ms = 0;
            wave_brightness[wave_head] = 1.0f;
            // グループ3の個数でループ
            wave_head = (wave_head + 1) % GRP3_LED_COUNT;
        }

        for (int i = 0; i < GRP3_LED_COUNT; i++)
        {
            wave_brightness[i] *= WAVE_FADE;
            if (wave_brightness[i] < 0.01f) wave_brightness[i] = 0.0f;

            uint8_t wr = (uint8_t)(WAVE_COLOR_R * wave_brightness[i]);
            uint8_t wg = (uint8_t)(WAVE_COLOR_G * wave_brightness[i]);
            uint8_t wb = (uint8_t)(WAVE_COLOR_B * wave_brightness[i]);

            WS2812B_SetLED(GRP3_START + i, wr, wg, wb);
        }

        // 全LEDを更新
        WS2812B_Update();
        osDelay(FRAME_DELAY_MS);
    }
}