/*
  =================================================================================
                          | 什么是一对一投篮机? |
  ---------------------------------------------------------------------------------
  和你的朋友来一场一对一的篮球赛，看看谁才是真正的投篮之王！

  =================================================================================
                                   | 开始 |
  ---------------------------------------------------------------------------------
  用冷冰冰的传感器创造出一个有趣的互动游戏。
  =================================================================================
                                 | 怎么玩 |
  ---------------------------------------------------------------------------------
  1. 蜂鸣器开始读秒 (嘀，嘀，嘀)。当LED灯从红色变成绿色时，游戏开始了！
  2. 开始往你的球篮里投球!
  3. 一旦有人得分:
     a. 蜂鸣器会响。
     b. 得分玩家的LED灯会闪烁绿光，闪烁次数为领先分数。
     c. 另一玩家的LED灯会闪烁红光，闪烁次数为落后分数。
     例如: 玩家A 领先2分。玩家A的LED灯将会闪绿光两次。
           玩家B 落后2分。玩家B的LED灯将会闪红光两次。
  4. 如果一个玩家领先5分，他/她就获胜了!
     a. 获胜者的LED 灯会呈现出彩虹效果。
     b. 失败者的LED 灯会发出纯红色光。
     c. 蜂鸣器会最后一次发出声音，表明游戏结束了。
  5. 按住碰撞传感器再来一局!
  =================================================================================
                           | 自己动手修改代码! |
  ---------------------------------------------------------------------------------
  点开 userDef.h 文件，然后根据你自己的喜好更改下面的参数。 :)

  SCORE_MAX: 获胜要领先的分数。
  DOCK_MAX:  灰度传感器的最小触发值。
  BRIGHT_MAX: LED彩灯显示的最大亮度。

  修改完这些值后，记得再把代码上传到mBattery里。
  =================================================================================
                              | 头脑风暴|
  ---------------------------------------------------------------------------------
  1. 可以把代码改成只有LED是某一种颜色的时候你才得分吗？
     这样的话如果颜色错了，说不定即使你进了球还会丢分。
  2. 可以加一个电机，让它在球筐上方旋转吗?
     这样你需要把握好投篮的时机，要不然球可能会被打飞。
  3. 可以把这个结构改成足球网吗?
     当然可以！！!

   注意: 以上功能上的变动会涉及到程序上的改动。
  =================================================================================
                               |端口连接|
  ---------------------------------------------------------------------------------
  碰撞传感器： D6
  蜂鸣器:      D10
  ColorLED：   D12
  灰度传感器： A0 和 A2
  =================================================================================
                                  | 问题 |
  ---------------------------------------------------------------------------------
  欢迎访问 http://forum.microduino.cn/，在我们的论坛提交您的问题， 我们会尽快回复您。
*/
#include "colorLed.h"
#include "userDef.h"
#include "game.h"

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(BRIGHT_MAX);
  keyLineA.begin(INPUT);
  keyLineB.begin(INPUT);
  keyButton.begin(INPUT_PULLUP);
#if DEBUG
  Serial.println("**************START************");
#endif
  soundInit();
}

void loop() {
#if DEBUG
  Serial.print("LINEA Val:");
  Serial.print(analogRead(PIN_LINEA));  //灰度传感器 A 值
  Serial.print("\tLINEB Val:");
  Serial.println(analogRead(PIN_LINEB));//灰度传感器 B 值
#endif
  gameReset();
  if (scoreCmp < SCORE_MAX) {
    updateScore();
  }
  else {
    if (scoreCmp == SCORE_MAX) {
      delay(1000);
      tone(PIN_BUZZER, 800);
      delay(500);
      tone(PIN_BUZZER, 1000);
      delay(500);
      tone(PIN_BUZZER, 1200);
      delay(500);
      noTone(PIN_BUZZER);
      scoreCmp++;
    }
    else {
      gameOver();
    }
  }
  delay(15);
}

