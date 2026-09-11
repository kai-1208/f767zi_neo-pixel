# f767zi_neo-pixel

## 概要
UserLibsに入ってるneo-pixelっていう自作ライブラリで光ります。制御はCore/Src/neo-pixel.cppに書いているコードを、Core/Src/freertos.cで呼び出します。基本的には、neo-pixel.cppのStartNeoPixelTask関数で制御します。

## cubemxの設定
- Pinout viewでPE13にTIM1_CH3を割り当て
- TimersでTIM1選んで、Clock SourceをInternal Clock、Channel3をPWM Generation CH3に設定
- 下のDMA SettingsでAddしてTIM1_CH3、DMA2 Stream1、Memory to Peripheral、Very Highにして、ModeをNormal、Increment AddressをMemoryだけチェック、Data WidthをどっちもWordに設定
- 横のParameter SettingsでCounter PeriodをCLock ConfigurationのHCLKの値から計算して設定、216MHzなら269に設定
- Parameter SettingsでPWM Generation Channel3でModeをPWM Mode1、Pulseを0、Output compare preloadをEnable、Fast ModeをDisable、CH PolarityをHigh、CH Idle Stateをresetに設定
- System CoreのGPIOでTIMにあるPE13を選んで、GPIO ModeをAlternate Function Push Pull、GPIO Pull-up/Pull-downをNo pull-up and no pull-down、Maximum output speedをVery Highに設定

### ※以下の設定は、freertosでdefaultTask使うなら設定の必要なし
- Middleware and Software PacksのFREERTOS選んで、Task and QueuesでTasksでAddして、NameをneoPixelTask、PriorityをNormal、Stack sizeを512、Entry functionをStartNeoPixelTask、Code Generation OptionをAs externalⅱ設定
