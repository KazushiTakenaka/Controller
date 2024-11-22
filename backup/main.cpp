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
const int SLD_SW1 = 32;
const int SLD_SW2 = 33;
const int SLD_SW3 = 22;
const int SLD_SW4 = 23;
bool connected;
uint8_t myMacAddress[6];

// スライドボリューム値読み取りピン
const int ANALOG_READ1 = 12;
const int ANALOG_READ2 = 13;

/*関数宣言*/

void setup() {
  Serial.begin(115200);
  Serial.println("BOOT_ESP32");
  /*Secretファイルから相手側のアドレスを取得後Bluetooth通信設定*/
  String MACadd = MAC_ADDRESS_STR;
  uint8_t address[6] = {MAC_ADDRESS_BYTE[0], MAC_ADDRESS_BYTE[1], MAC_ADDRESS_BYTE[2], MAC_ADDRESS_BYTE[3], MAC_ADDRESS_BYTE[4], MAC_ADDRESS_BYTE[5]};
  EspBTSetUpClass EspBTSetObj;
  EspBTSetObj.getMacAddress();
  // EspBTSetObj.sendSetUp(address);
  /*ピンモード*/
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);
  pinMode(SW6, INPUT_PULLUP);
  pinMode(SW7, INPUT_PULLUP);
  pinMode(SW8, INPUT_PULLUP);
  pinMode(SLD_SW1, INPUT_PULLUP);
  pinMode(SLD_SW2, INPUT_PULLUP);
  pinMode(SLD_SW3, INPUT_PULLUP);
  pinMode(SLD_SW4, INPUT_PULLUP);
  pinMode(ANALOG_READ1, INPUT);
  pinMode(ANALOG_READ2, INPUT);
}

int receivedData;// 受信データ
int i;
/*アナログ読み取り変数*/
int slideVal1;
int slideVal2;

/*送信用構造体*/
struct SendData
  {
    int slideVal1;
    int slideVal2;
    int sld_sw1;
    int sld_sw2;
    int sld_sw3;
    int sld_sw4;
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
  // slideVal1 = analogRead(ANALOG_READ1);
  // slideVal2 = analogRead(ANALOG_READ2);
  /*構造体に送信情報を代入*/
  SendData sendData;
  sendData = dataLoading();
  SerialBT.write((uint8_t*)&sendData, sizeof(sendData));// 構造体送信

  /*通信途切れで再起動*/
  if (SerialBT.available()) {
    receivedData = SerialBT.read();
    i = receivedData;
  }else{
    i++;
    Serial.println(" Lost");
    if (i >= 20){
      ESP.restart();
    }
  }
  
  delay(50);
}

SendData dataLoading() {
  /*
  構造体生成
  アナログデータ読み込みする。
  生成した構造体にデータを入れて返り値にする
  return:SendData
  */
  slideVal1 = analogRead(ANALOG_READ1);
  slideVal2 = analogRead(ANALOG_READ2);
  SendData readDatas;
  readDatas.slideVal1 = map(slideVal1, 0, 4095, 0, 255);
  readDatas.slideVal2 = map(slideVal2, 0, 4095, 0, 255);
  readDatas.sld_sw1 = digitalRead(SLD_SW1);
  readDatas.sld_sw2 = digitalRead(SLD_SW2);
  readDatas.sld_sw3 = digitalRead(SLD_SW3);
  readDatas.sld_sw4 = digitalRead(SLD_SW4);
  readDatas.sw1 = digitalRead(SW1);
  readDatas.sw2 = digitalRead(SW2);
  readDatas.sw3 = digitalRead(SW3);
  readDatas.sw4 = digitalRead(SW4);
  readDatas.sw5 = digitalRead(SW5);
  readDatas.sw6 = digitalRead(SW6);
  readDatas.sw7 = digitalRead(SW7);
  readDatas.sw8 = digitalRead(SW8);
  #if 1
  Serial.print(readDatas.sld_sw1);
  Serial.print(readDatas.sld_sw2);
  Serial.print(readDatas.sld_sw3);
  Serial.print(readDatas.sld_sw4);
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