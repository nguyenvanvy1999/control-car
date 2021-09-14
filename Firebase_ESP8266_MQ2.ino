#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "cambien-4b7cd.firebaseio.com"             // duong dan toi du an firebase
#define FIREBASE_AUTH "VPprd7SCsTqBv21hL6jXpRbzaknR3gmcdjhCvwhs" // auth cho firebase
#define WIFI_SSID "OPPO A91"                                     //ten wifi mach se ket noi
#define WIFI_PASSWORD "11112222"                                 // password wifi

/************************Hardware Related Macros************************************/
#define MQ_PIN (0)                 // Chan annalog duco su dung tren mach
#define RL_VALUE (5)               //Dien tro Rl cua cam bien,tinh bang kilo ohr//10
#define RO_CLEAN_AIR_FACTOR (9.83) //Giá trị Rs/Ro của không khí sạch. Lấy từ data
/***********************Software Related Macros************************************/
#define CALIBRATION_SAMPLE_TIMES (50)     //So lan lay mau trong giai doan hieu chinh cam bien
#define CALIBRATION_SAMPLE_INTERVAL (500) //Khoang cach thoi gian giua cac lan lay mau (ms) trong giai doan hieu chinh
#define READ_SAMPLE_INTERVAL (50)         //So lan lay mau trong giai doan binh thuong
#define READ_SAMPLE_TIMES (5)             //Khoang cach thoi gian giua cac lan lay mau
/**********************Application Related Macros**********************************/
#define GAS_LPG (0)   // nong do khi LPG
#define GAS_CO (1)    // nong do Co
#define GAS_SMOKE (2) // nong do khoi

/*****************************Globals***********************************************/
/*
Gia su phuong trinh co dang Y=mX+b , Nhung day la phuong trinh log, tuc la X=log(x) , Y=log(y)
LPG lay 2 diem la (200,1.6) va (10000,0.27)=> Tinh duoc m=-0.47
Chon tiep (5000,0,46)=> b=1,413
=> log(x)=[log(y)-b]/m => tinh duoc nong do LPG tai moi diem
Tuong tu doi voi CO va Smoke
{log(x1),log(y1),m}
*/

float LPGCurve[3] = {2.3, 0.21, -0.47};   //2 diem duoc lay tu duong cong. 2 diem nay tao thanh 1 duong gan tuong tu nhu duong cong ban dau
                                          //data format:{ x, y, slope}; point1: (log(200), log(1.6)), point2: (log10000, -0.59)
float COCurve[3] = {2.3, 0.72, -0.34};    //2 diem duoc lay tu duong cong. 2 diem nay tao thanh 1 duong gan tuong tu nhu duong cong ban dau
                                          //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15)
float SmokeCurve[3] = {2.3, 0.53, -0.44}; //2 diem duoc lay tu duong cong. 2 diem nay tao thanh 1 duong gan tuong tu nhu duong cong ban dau
                                          //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)
float Ro = 10;                            //Ro duoc khoi tao thanh 10 kilo ohms

/****************** MQResistanceCalculation ****************************************
Ham tinh Rs
*@param
raw_adc - Gia tri tho, doc duoc tu adc, dai dien cho dien ap
*@return
Rs
Remarks: Cong thuc tinh la Rs=(vin-raw_adc)/raw_adc. Vi thang do duoc chia thanh 1023 nen vin-1023
************************************************************************************/
float MQResistanceCalculation(int raw_adc)
{
    return (((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/***************************** MQCalibration ****************************************
Ham tinh Ro
*@param
mq_pin - gia tri doc duoc tu chan analog
*@return
Remarks: Gia su cam bien o trong khong khi sach.-
Dung ham MQResistanceCalculation de tinh toan dien
tro cua cam bien khi o trong khong khi sach sau do
chia cho RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR xap xi 10.
Ro khac nhau doi voi tung cam bien khac nhau
Ro=9.8/Rs
************************************************************************************/
float MQCalibration(int mq_pin)
{
    int i;
    float val = 0;

    for (i = 0; i < CALIBRATION_SAMPLE_TIMES; i++)
    {
        val += MQResistanceCalculation(analogRead(mq_pin));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    val = val / CALIBRATION_SAMPLE_TIMES; // Tinh gia tri trung binh

    val = val / RO_CLEAN_AIR_FACTOR; //Chia cho RO_CLEAN_AIR_FACTOR
                                     //Theo bieu do trong datasheet
    return val;
}
/*****************************  MQRead *********************************************
*@param mq_pin - Chan analog
*@return Rs
Remarks: Ham nay dung de tinh toan Rs bang cach doc 1 so lan sau do cong lai va chia lay
trung binh cong
************************************************************************************/
float MQRead(int mq_pin)
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++)
    {
        rs += MQResistanceCalculation(analogRead(mq_pin));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs = rs / READ_SAMPLE_TIMES;

    return rs;
}

/*****************************  MQGetGasPercentage **********************************
*@param rs_ro_ratio - Rs chia cho Ro
        gas_id      - loại khí gas đo
*@return ppm của khí đó
Remarks: Hàm nay chuyển các số liệu khác nhau để tính toán ppm (phần triệu) của các loại khí gas khác nhau
************************************************************************************/
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
    if (gas_id == GAS_LPG)
    {
        return MQGetPercentage(rs_ro_ratio, LPGCurve);
    }
    else if (gas_id == GAS_CO)
    {
        return MQGetPercentage(rs_ro_ratio, COCurve);
    }
    else if (gas_id == GAS_SMOKE)
    {
        return MQGetPercentage(rs_ro_ratio, SmokeCurve);
    }

    return 0;
}

/*****************************  MQGetPercentage **********************************
* @param rs_ro_ratio - Rs divided by Ro
        pcurve      - pointer to the curve of the target gas
*@return ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
        of the line could be derived if y(rs_ro_ratio) is provided. As it is a
        logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
        value.
************************************************************************************/
int MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
    return (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}

/**
 * @note
 * Khí dầu mỏ hóa lỏng (Liquefied Petroleum Gas - LPG) là sản phẩm hydrocacbon có nguồn gốc dầu mỏ với thành phần chính là Propan (C3H8) và Butan (C4H10).
 * Hai thành phần chính này ở điều kiện nhiệt độ và áp suất bình thường sẽ ở dạng khí. Với một áp suất nhất định hoặc khi được làm lạnh,
 * hai hợp chất khí này sẽ hóa lỏng để dễ dàng được vận chuyển, tồn trữ và sử dụng.
 */

void setup()
{
    Serial.begin(115200);
    Serial.print("Conecting to Wifi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi Connected!");
    Serial.println(WiFi.localIP());
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    if (Firebase.failed())
    {
        delay(500);
        Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
        Serial.println(Firebase.error());
        Serial.println("Connection to firebase failed. Reconnecting...");
        delay(500);
    }
    else
    {
        Serial.println("Firebase connected !");
    }
    Serial.print("Calibrating...\n");
    Ro = MQCalibration(MQ_PIN); //Hieu chinh cam bien. Dat cam bien o trong khong khi sach
    Serial.print("Calibration is done...\n");
    Serial.print("Ro=");
    Serial.print(Ro);
    Serial.print("kohm");
    Serial.print("\n");
}

void loop()
{
    float LPG_ppm = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG);
    float CO_ppm = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO);
    float SMOKE_ppm = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE);
    Serial.print("A0:");
    float A = analogRead(MQ_PIN);
    Serial.println(A);
    Serial.print("LPG:");

    Firebase.setFloat("Firebase/LPG", LPG_ppm);
    if (Firebase.failed())
    {
        Serial.print("setting /LPG failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print(LPG_ppm);
    Serial.print("ppm");
    Serial.print("    ");

    Serial.print("CO:");
    Firebase.setFloat("Firebase/CO", CO_ppm);
    if (Firebase.failed())
    {
        Serial.print("setting /CO failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print(CO_ppm);
    Serial.print("ppm");
    Serial.print("    ");

    Serial.print("SMOKE:");
    Firebase.setFloat("Firebase/SMOKE", SMOKE_ppm);
    if (Firebase.failed())
    {
        Serial.print("setting /SMOKE failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print(SMOKE_ppm);
    Serial.print("ppm");
    Serial.print("\n");

    delay(1000);
}
