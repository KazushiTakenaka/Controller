#include <Arduino.h>
// Bluetooth用設定
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
#include <string>
#include "Secret.h"
#include "Esp32BT.h"

const int SW1 = 15;
const int SW2 = 16;
const int SW3 = 17;
const int SW4 = 18;
const int SW5 = 14;
const int SW6 = 25;
const int SW7 = 26;
const int SW8 = 27;
const int SLD_SW1_1 = 32;
const int SLD_SW1_2 = 2;
const int SLD_SW2_1 = 33;
const int SLD_SW2_2 = 4;
const int SLD_SW3_1 = 22;
const int SLD_SW3_2 = 5;
const int SLD_SW4_1 = 23;
const int SLD_SW4_2 = 19;
const int BLE_LED = 21;
const int BATTERY = 35;
bool connected;
uint8_t myMacAddress[6];

// スライドボリューム値読み取りピン
const int ANALOG_READ1 = 12;
const int ANALOG_READ2 = 13;
//BUTTRY電圧確認用
float battery_value = 0;

/*関数宣言*/
float getVoltage();

void setup() {
  Serial.begin(115200);
  Serial.println("BOOT_ESP32");
  /*mac addressの取得後子機としてBluetooth起動*/
  EspBTSetUpClass EspBTSetObj;
  EspBTSetObj.getMacAddress();
  EspBTSetObj.childUnitSetUp();
  
  /*ピンモード*/
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);
  pinMode(SW6, INPUT_PULLUP);
  pinMode(SW7, INPUT_PULLUP);
  pinMode(SW8, INPUT_PULLUP);
  pinMode(SLD_SW1_1, INPUT_PULLUP);
  pinMode(SLD_SW1_2, INPUT_PULLUP);
  pinMode(SLD_SW2_1, INPUT_PULLUP);
  pinMode(SLD_SW2_2, INPUT_PULLUP);
  pinMode(SLD_SW3_1, INPUT_PULLUP);
  pinMode(SLD_SW3_2, INPUT_PULLUP);
  pinMode(SLD_SW4_1, INPUT_PULLUP);
  pinMode(SLD_SW4_2, INPUT_PULLUP);
  pinMode(ANALOG_READ1, INPUT);
  pinMode(ANALOG_READ2, INPUT);
  pinMode(BLE_LED, OUTPUT);
}

int receivedData;// 受信データ
int i;
int j = 0;
/*アナログ読み取り変数*/
int slideVal1;
int slideVal2;

/*送信用構造体*/
struct SendData
  {
    int slideVal1;
    int slideVal2;
    int sld_sw1_1;
    int sld_sw1_2;
    int sld_sw2_1;
    int sld_sw2_2;
    int sld_sw3_1;
    int sld_sw3_2;
    int sld_sw4_1;
    int sld_sw4_2;
    int sw1;
    int sw2;
    int sw3;
    int sw4;
    int sw5;
    int sw6;
    int sw7;
    int sw8;
  };

SendData dataLoading();

void loop() {
  /*電池残用読み込み*/
  // battery_value = getVoltage();
  battery_value = 1.0;
  /*構造体に送信情報を代入*/
  SendData sendData;
  sendData = dataLoading();
  SerialBT.write((uint8_t*)&sendData, sizeof(sendData));// 構造体送信

  /*通信途切れで再起動*/
  if (SerialBT.available()) {
    /*バッテリー残量確認
     残量小のときLED点滅*/
    if (battery_value <= 1.6){
      j++;
      if (j <= 25){
        Serial.print("1:");
        digitalWrite(BLE_LED, LOW);
      }else if (j >= 26 && j <= 49){
        Serial.print("2:");
        digitalWrite(BLE_LED, HIGH);
      }else if (j >= 50){
        j = 0;
      }
    }else if (battery_value > 1.6){
      digitalWrite(BLE_LED, HIGH);
    }

    receivedData = SerialBT.read();
    i = receivedData;
  }else {
    i++;
    digitalWrite(BLE_LED, LOW);
    // if (i >= 20){
    //   ESP.restart();
    // }
  }
  
  delay(40);
}

SendData dataLoading() {
  /*
  構造体生成
  アナログデータ読み込みする。
  生成した構造体にデータを入れて戻り値にする
  return:SendData
  */
  slideVal1 = analogRead(ANALOG_READ1);
  slideVal2 = analogRead(ANALOG_READ2);
  SendData readDatas;
  readDatas.slideVal1 = map(slideVal1, 0, 4095, 0, 255);
  readDatas.slideVal2 = map(slideVal2, 0, 4095, 0, 255);
  readDatas.sld_sw1_1 = digitalRead(SLD_SW1_1);
  readDatas.sld_sw1_2 = digitalRead(SLD_SW1_2);
  readDatas.sld_sw2_1 = digitalRead(SLD_SW2_1);
  readDatas.sld_sw2_2 = digitalRead(SLD_SW2_2);
  readDatas.sld_sw3_1 = digitalRead(SLD_SW3_1);
  readDatas.sld_sw3_2 = digitalRead(SLD_SW3_2);
  readDatas.sld_sw4_1 = digitalRead(SLD_SW4_1);
  readDatas.sld_sw4_2 = digitalRead(SLD_SW4_2);
  readDatas.sw1 = digitalRead(SW1);
  readDatas.sw2 = digitalRead(SW2);
  readDatas.sw3 = digitalRead(SW3);
  readDatas.sw4 = digitalRead(SW4);
  readDatas.sw5 = digitalRead(SW5);
  readDatas.sw6 = digitalRead(SW6);
  readDatas.sw7 = digitalRead(SW7);
  readDatas.sw8 = digitalRead(SW8);
  #if 0
  Serial.print(readDatas.sld_sw1_1);
  Serial.print(readDatas.sld_sw1_2);
  Serial.print(readDatas.sld_sw2_1);
  Serial.print(readDatas.sld_sw2_2);
  Serial.print(readDatas.sld_sw3_1);
  Serial.print(readDatas.sld_sw3_2);
  Serial.print(readDatas.sld_sw4_1);
  Serial.print(readDatas.sld_sw4_2);
  Serial.print(readDatas.sw1);
  Serial.print(readDatas.sw2);
  Serial.print(readDatas.sw3);
  Serial.print(readDatas.sw4);
  Serial.print(readDatas.sw5);
  Serial.print(readDatas.sw6);
  Serial.print(readDatas.sw7);
  Serial.print(readDatas.sw8);
  Serial.print(readDatas.slideVal1);
  Serial.println(readDatas.slideVal2);
  #endif
  return readDatas;
}

// 電源電圧を取得する関数
float getVoltage() {
  // ADCで値を読み取る
  int adc_value = analogRead(BATTERY);

  // 電圧を計算
  float voltage = adc_value * (3.3 / 4096);

  return voltage;
}