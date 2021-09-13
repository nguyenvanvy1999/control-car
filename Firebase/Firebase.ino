#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <ArduinoJson.h>


#define FIREBASE_HOST "projectdemo-a4fe4-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "5yDT06F36Ohnax4rYAspshKXg4zbGzQXqtMlQ5Nq"
#define WIFI_SSID "Longdeppzai"
#define WIFI_PASSWORD "anhyeuem11226688"

#define ENA 14  //GPIO14(D5) Chân dùng để bật/ set tốc độ cho motor bên Phải
#define ENB 12  // GPIO12(D6) bật/set tốc độ cho motor bên phải
#define IN_1 15 // GPIO15(D8)  Điều khiển trạng thái của motor Phải
#define IN_2 13 // GPIO13(D7)  Điều khiển trạng thái của motor Phải
#define IN_3 2  // GPIO2(D4) Điều khiển trạng thái của motor Trái
#define IN_4 0  // GPIO0(D3) Điều khiển trạng thái của motor Trái
#define MQ_PIN 10

String path = "/";
FirebaseJson json;
FirebaseData firebaseData;
int speedCar;
const int speed_Coeff = 800;

void setup() {
  // pinMode các chân là output
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
	pinMode(MQ_PIN, INPUT);

  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("Reason" + firebaseData.errorReason());
    Serial.println();
  }
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// hàm tiến
void goAhead()
{
  digitalWrite(IN_1, LOW); // xuất tín hiệu ra chân in1 và in2.
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar); // xuất xung điều khiển tốc độ
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}
// hàm lùi
void goBack()
{
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}
// phải
void goRight()
{
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}
// trái
void goLeft()
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}
// tiến phải
void goAheadRight()
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar / speed_Coeff); // khi đi chéo thì 1 bên được cấp tốc độ nhỏ hơn
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}
// tiến trái
void goAheadLeft()
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar / speed_Coeff);
}
// lùi phải
void goBackRight()
{
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar / speed_Coeff);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}
// lùi trái
void goBackLeft()
{
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar / speed_Coeff);
}
// dừng
void stopRobot()
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void loop() {
	float LPG = analogRead(MQ_PIN);
	Firebase.setFloat(firebaseData, path + "/control", LPG);
  String control;
  if (Firebase.getString(firebaseData, path + "/control")) control = firebaseData.stringData();
  Serial.println(control);
	if (control == "F") {
		goAhead();
	} else if (control == "B") {
		goBack();
	} else if (control == "L") {
		goLeft();
	} else if (control == "R") {
		goRight();
	} else if (control == "I") {
		goAheadRight();
	} else if (control == "G") {
		goAheadLeft();
	} else if (control == "J") {
		goBackRight();
	} else if (control == "H") {
		goBackLeft();
	} else {
		stopRobot();
	}

	String speed;
  if (Firebase.getString(firebaseData, path + "/speed")) speed = firebaseData.stringData();
  Serial.println(speed);
	if (speed == "0") {
		speedCar = 400;
	} else if (speed == "1") {
		speedCar = 470;
	} else if (speed == "2") {
		speedCar = 540;
	} else if (speed == "3") {
		speedCar = 610;
	} else if (speed == "4") {
		speedCar = 680;
	} else if (speed == "5") {
		speedCar = 750;
	} else if (speed == "6") {
		speedCar = 820;
	} else if (speed == "7") {
		speedCar = 890;
	} else if (speed == "8") {
		speedCar = 960;
	} else {
		speedCar = 1023;
	}
}