/*
 * FSR SENSOR Read Quad 
 * 
 * 
 *
 * 
 *
 *
*/
// ============================== BLE 사용을 위한 헤더 선언 및 PIN 설정
#include <SoftwareSerial.h> // 소프트웨어를 통해 직렬 통신(Serial Communication)을 구현하는 라이브러리
#include <String.h> // 문자열 처리 라이브러리

SoftwareSerial bluetooth(2, 3); // 블루투스 모듈을 선언합니다.
// ============================== BLE

const int fsrPin[4] = {A0, A1, A2, A3}; // 
const double fsrCali[4] = {26.0, 26.5, 25.5, 26.0}; // fsr sensor's Calibration values

int fsrVoltData = 0;
double vout;

double fsr_data[4] = {0,}; 

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600); // [BLE] 블루투스 통신을 위한 대역 설정

  for(int sensorPin = 0; sensorPin < 4; sensorPin++) { // [FSR] 
    pinMode(fsrPin[sensorPin], INPUT); // fsrPin 배열의 각 핀에 대해 입력 모드 설정
  }
}

void GetFsrData(void){ // 각각의 FSR 데이터 값 fsr_data 배열에 저장
  for(int sensorPin = 0; sensorPin < 4; sensorPin++){
    fsrVoltData = analogRead(fsrPin[sensorPin]); // Updated variable name here
    vout = (fsrVoltData * 5.0) / 1023.0;
    fsr_data[sensorPin] = vout * fsrCali[sensorPin];
  }
}

void ViewFsrPlotter(){ // 측정된 fsr 데이터를 IDE에서 플로터 형태로 모니터에 출력
  for(int viewCount = 0; viewCount < 4; viewCount++){
    Serial.print((fsr_data[viewCount]));
    Serial.print(",");
  }
  Serial.println("");
}

// void BluetoothDataSend(void){ // [BLE] 블루투스로 FSR 데이터 값 보내기.
//   for(int sendDataCount = 0; sendDataCount<4; sendDataCount++){
//     bluetooth.print(String(int(fsr_data[sendDataCount]*1000))); // 문자형으로 데이터를 보내고 있음.
//     bluetooth.print("@");
//   }
//   bluetooth.println("");
// }

void BluetoothDataSend(void){ // [BLE] 블루투스로 FSR 데이터 값 보내기. (4개를 1개로 융합)
  double total=0;
  
  for(int sendDataCount = 0; sendDataCount<4; sendDataCount++){ //각각의 센서로부터 값을 전부 합함
    total+=fsr_data[sendDataCount];
  }

  total*=2;
  total/=10; // total 값을 10으로 나눔

  if(total>=10) total=10;
  else if(total<=0) total=0;

  bluetooth.print(String(int(total))+"@"); // 문자형으로 데이터를 보내고 있음.
  bluetooth.println("");
}

int loopCount = 0;
unsigned long timeStart;
unsigned long timeEnd;

void loop() {
  // timeStart = millis();
  GetFsrData(); // ============================== fsr 데이터 불러오기
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- GetFsrData");

  // timeStart = millis();
  ViewFsrPlotter(); // ============================== fsr 데이터를 Serial Plotter로 출력 함수
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- ViewFsrPlotter");

  delay(50);
  // timeStart = millis();
  BluetoothDataSend();// ============================== BLE 데이터 전송 코드
  // timeEnd = millis();
  // Serial.print(timeEnd-timeStart);
  // Serial.println(" <- BluetoothDataSend");
}
