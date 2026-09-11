#include "main.h"
#include "cmsis_os.h"
#include "neo-pixel.h"

// 使用するタイマー（CubeMXが生成したもの）
extern TIM_HandleTypeDef htim1;

// 色生成ヘルパークラス例（C++の機能を利用）
class ColorWheel {
public:
    static void getRGB(uint8_t pos, uint8_t &r, uint8_t &g, uint8_t &b) {
        if (pos < 85) {
            r = pos * 3;
            g = 255 - pos * 3;
            b = 0;
        } else if (pos < 170) {
            pos -= 85;
            r = 255 - pos * 3;
            g = 0;
            b = pos * 3;
        } else {
            pos -= 170;
            r = 0;
            g = pos * 3;
            b = 255 - pos * 3;
        }
    }
};

// C言語側から呼び出すため extern "C" を付与
extern "C" void StartNeoPixelTask(void *argument)
{
    // NeoPixelの初期化（TIM1 Channel 3）
    WS2812B_Init(&htim1, TIM_CHANNEL_3);

    for(;;)
    {
        // 全LEDを白色に設定
        WS2812B_SetAll(255, 255, 255);
        WS2812B_Update();
        osDelay(30);
    }
    
    // uint8_t offset = 0;
    // uint8_t r, g, b;

    // for(;;)
    // {
    //     // レインボーアニメーションの計算
    //     for (int i = 0; i < NUM_LEDS; i++) {
    //         ColorWheel::getRGB(((i * 256 / NUM_LEDS) + offset) & 255, r, g, b);
    //         // 眩しさ軽減のため輝度を1/4に落とす (>> 2)
    //         WS2812B_SetLED(i, r >> 2, g >> 2, b >> 2);
    //     }

    //     // DMAでLEDへ出力（送信中はスリープ）
    //     WS2812B_Update();

    //     offset += 2;

    //     // 次の更新まで30ms待機（約33fps）
    //     osDelay(30);
    // }
}