/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.

 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch

 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).

 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/
// ============================== BLE 사용을 위한 헤더 선언 및 PIN 설정
#include <SoftwareSerial.h> // 소프트웨어를 통해 직렬 통신(Serial Communication)을 구현하는 라이브러리
#include <String.h> // 문자열 처리 라이브러리

SoftwareSerial bluetooth(2, 3); // 블루투스 모듈을 선언합니다.
// ============================== BLE

#include "HX711.h"

HX711 scale[4]; //HX711 scale(DOUT, CLK);
//double calibration_factor[4] = {-18350, -17550, -17350, -16550}; //-7050 worked for my 440lb max scale setup (back up!!!)
double calibration_factor[4] = {-15350, -15050, -14850, -14850}; //-7050 worked for my 440lb max scale setup (60kg set)

//double calibration_factor[4] = {-10950, -10950, -9050, -9050}; // 켈리브레이션 재진행
double loadcell_data[4] = {0};

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  // scale's DOUT and SCK begin roof ( scale[0] <DOUT> 4, <SCK> 5 -> ... -> scale[3] 10, 11 )
  for(int begin_scales = 0; begin_scales < 4; begin_scales++ ){
    scale[begin_scales].begin((begin_scales+2)*2 /*LOADCELL_DOUT_PIN*/ , (begin_scales+2)*2+1 /*LOADCELL_SCK_PIN*/ );
  }

  // massage 1 // WILL EDIT... 
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  // scale's scale data reset roof
  for(int reset_scales = 0; reset_scales < 4; reset_scales++){
    scale[reset_scales].set_scale(calibration_factor[reset_scales]);
    scale[reset_scales].tare(); //Reset the scale to 0
  }

  long zero_factor[4] = {0};
  for(int view = 0 ; view < 4 ; view++ ){
    zero_factor[view]=scale[view].read_average();
    Serial.print("Zero factor ["); Serial.print(view); Serial.print("] : "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor[view]);
  }
  // delay(150);
}

void GetLoadcellData(void){ // 로드셀 데이터 불러오기
  for(int cali = 0 ; cali < 4 ; cali++){
    scale[cali].set_scale(calibration_factor[cali]); //Adjust to this calibration factor
    loadcell_data[cali] = scale[cali].get_units()*0.453592; // 로드셀 데이터를 kg으로 환산하는 코드 밎 출력
    if(loadcell_data[cali]<0) loadcell_data[cali]=-loadcell_data[cali]; // 음수 값을 양수로 바꾸어주는 연산을 진행. 
  }
}

void ViewLoadcellPlotter(){ // 측정된 load 데이터를 IDE에서 플로터 형태로 모니터에 출력
  for(int serial_plotter = 0 ; serial_plotter < 4 ; serial_plotter++){
    Serial.print((loadcell_data[serial_plotter]));
    Serial.print(",");
  }
  // total length
  Serial.print(loadcell_data[0]+loadcell_data[1]+loadcell_data[2]+loadcell_data[3]);
  Serial.println("");
}

// void BluetoothDataSend(void){ // [BLE] 블루투스로 FSR 데이터 값 보내기.
//   bluetooth.println(String(int(loadcell_data[0]*1000))+"@"+String(int(loadcell_data[1]*1000))+"@"+String(int(loadcell_data[2]*1000))+"@"+String(int(loadcell_data[3]*1000))+"@"); // 문자형으로 데이터를 보내고 있음.
// }

// void BluetoothDataSend(void){ // [BLE] 블루투스로 FSR 데이터 값 보내기.
//   for(int sendDataCount = 0; sendDataCount<4; sendDataCount++){
//     bluetooth.print(String((unsigned int)(loadcell_data[sendDataCount]*1000))); // 문자형으로 데이터를 보내고 있음.
//     bluetooth.print("@");
//   }
//   bluetooth.println("");
// }

void BluetoothDataSend(void){ // [BLE] 블루투스로 FSR 데이터 값 보내기. (4개를 1개로 융합)
  double total=0;
  
  for(int sendDataCount = 0; sendDataCount<4; sendDataCount++){ //각각의 센서로부터 값을 전부 합함
    total+=loadcell_data[sendDataCount];
  }

  total/=10; // total 값을 10으로 나눔

  if(total>=10) total=10;
  else if(total<=0) total=0;

  bluetooth.print(String(int(total))+"@"); // 문자형으로 데이터를 보내고 있음.
  bluetooth.println("");
}

int loopCount = 0;
unsigned long timeStart;
unsigned long timeEnd;

unsigned long timeStartTotal = 0;
unsigned long timeEndTotal = 0;

void loop() {
  // timeStart = timeStartTotal = millis();
  GetLoadcellData(); // ============================== Loadcell 데이터 불러오기
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- GetLoadcellData");

  // timeStart = millis();
  ViewLoadcellPlotter(); // ============================== Loadcell 데이터를 Serial Plotter로 출력 함수
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- ViewLoadcellPlotter");

  //===============================================
  // this delay is able for COMFORTABLE DATA SEND
  // 결과 : 일부 지연을 주니 값을 정상적으로 전송함
  delay(50);
  //===============================================


  // timeStart = millis();
  BluetoothDataSend(); // ============================== BLE 데이터 전송 코드
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- BluetoothDataSend");

  // timeEndTotal = millis();
  // Serial.print(timeEndTotal-timeStartTotal);
  // Serial.println(" <- TOTAL TIME");
}
