//打地鼠游戏机
#include "userDef.h"
#include "sound.h"
#include <Microduino_Number.h>    //引用数码管库
#include <Microduino_ColorLED.h>  //引用彩灯库
#include <Microduino_Key.h>       //按键库

ColorLED strip = ColorLED(NUMPIXELS, PIN); //将ColorLED类命名为strip，并定义彩灯数量和彩灯引脚号

SoftwareSerial mySerial(PIN_NUMLED_RX, PIN_NUMLED_TX); 
Number NumLed(2, &mySerial);    

DigitalKey key[4] = {
  DigitalKey(4), DigitalKey(6), DigitalKey(8), DigitalKey(10),
};

void setup() {
  strip.begin();
  NumLed.begin();
  key[0].begin();
  key[1].begin();
  key[2].begin();
  key[3].begin();
}

int score = 0, light_num = 0;
uint32_t last_time;
void loop() {
  //游戏准备开始
  strip.setBrightness(0);
  strip.show();
  delay(1);
  //3和4号按键同时按下游戏开始，在此之前数码管闪烁
  while (key[2].readVal() != KEY_PRESSED || key[3].readVal() != KEY_PRESSED) {
    NumLed.turnOff(ALL);
    delay(100);
    NumLed.setNumber(0, score / 10, 0);
    NumLed.setNumber(1, score % 10, 0);
    NumLed.show();
    delay(100);
  }
  //分数清零，发出提示音
  score = 0;
  NumLed.setNumber(0, score / 10, 0);
  NumLed.setNumber(1, score % 10, 0);
  NumLed.show();
  buzzerTinkle();
  buzzerTinkle();
  delay(1000);

  //游戏开始
  strip.setBrightness(5);
  last_time = millis();//开始计时
  //15秒内重复执行打地鼠程序
  while (millis() - last_time < 15000) {
    light_num = getRandom(0, 4);//随机选择一个彩灯号码
    strip.setPixelColor(0, COLOR_NONE);//熄灭所有彩灯
    strip.setPixelColor(1, COLOR_NONE);
    strip.setPixelColor(2, COLOR_NONE);
    strip.setPixelColor(3, COLOR_NONE);
    strip.setPixelColor(light_num, COLOR_COLD);//点亮该彩灯
    strip.show();
    delay(1);
    while (!pressAnyButton());//任意键按下
    //如果没按下点亮的彩灯，则发出警报音、扣分
    if (key[light_num].readVal() != KEY_PRESSED) {
      strip.setPixelColor(light_num, COLOR_RED);//点亮该彩灯
      strip.show();
      delay(1);
      buzzerAlarm();
      if (score > 0) {
        score--;
      }
    }
    //否则发出提示音、加分
    else {
      tone(BuzzerPin, 2000);
      score++;
    }
    while (pressAnyButton());//等待所有按键抬起
    delay(5);
    noTone(BuzzerPin);
    NumLed.setNumber(0, score / 10, 0);//刷新数码管分数显示
    NumLed.setNumber(1, score % 10, 0);
    NumLed.show();
  }
  //游戏结束，如果大于30分则播放音乐
  if (score >= 30) {
    playMusic();
  }
}

//高级随机函数（每次随机数不重复）
byte last_random = 0;
byte now_random = 0;
byte getRandom(byte _min, byte _max) {
  while (last_random == now_random) {
    now_random = random(_min, _max);
  }
  last_random = now_random;
  return now_random;
}

//任意键按下函数（带消抖功能）
bool pressAnyButton() {
  if (key[0].readVal() == KEY_PRESSED || key[1].readVal() == KEY_PRESSED || key[2].readVal() == KEY_PRESSED || key[3].readVal() == KEY_PRESSED)
    return true;
  else
    return false;
}
