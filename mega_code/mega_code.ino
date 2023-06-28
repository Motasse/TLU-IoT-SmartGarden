#define _SS_MAX_RX_BUFF 256 // RX buffer size
#include <Stepper.h> //Để điều khiển động cơ bước
#include <SoftwareSerial.h> //Tạo cổng Serial mềm
#include <ArduinoJson.h> //Tạo chuỗi JSON
#include <Wire.h> //Điều khiển LCD
#include <LiquidCrystal_I2C.h>; //Giải mã LCD qua I2C

SoftwareSerial megaserial(11,10); //RX||TX //Khai báo cổng Serial qua chân 11,10
LiquidCrystal_I2C lcd(0x27,16,2); //Khai báo LCD
const int stepsPerRevolution = 200; //Số bước trong một vòng của động cơ
Stepper upperS(stepsPerRevolution, 2, 3, 4, 5); //Khai báo động cơ trên với chân 2,3,4,5
Stepper lowerS(stepsPerRevolution, 6, 7, 8, 9); //Khai báo động cơ dưới với chân 6,7,8,9

#define pin1 A3
#define pin2 A2
#define pin3 A1
int pump = 14; //Khai báo các chân
int v1 = 17;
int v2 = 16;
int v3 = 15;
int rled = 51;
int gled = 53;
int v[3] = {49,45,39};
int r1 = 47;
int r2 = 43;
int pmp = 41;
int r1_state = 0;
int r2_state = 0;
int pmp_state = 0;
int up = 0; //Biến check trạng thái ray trên
int low = 0; //Biến check trạng thái ray dưới
int pstate = 0; //Biến check trạng thái máy bơm
int vstate[3] = {0,0,0}; //Biến check trạng thái van nước
int local = 0; //Biến state điều khiển của hệ thống
int autoo = 0; //Biến check trạng thái hoạt động của hệ thống
int state = 0; //Biến kiểm tra set state cho ray
int mcao = 90;
int rmax = 60;
int mmin[3] = {25,25,25};
int mmax[3] = {45,45,45};
long baud = 57600;
long lmin = 90000;
long lmax = 120000;
unsigned long prevmillis = 0;
unsigned long sendmillis = 0;
unsigned long automillis = 0;
//int lowwtr = 0; //Biến check mực nước

void setup() {
  Serial.begin(baud); //Mở cổng Serial với 115200 baudrate
  megaserial.begin(baud); //Mở cổng Serial mềm với 115200 baudrate
  upperS.setSpeed(200); //Đặt tốc độ cho động cơ bước
  lowerS.setSpeed(200);
  lcd.init(); //Bật LCD
  lcd.backlight(); //Bật đèn LCD
  pinMode(pump, OUTPUT); //Đặt các kiểu cho chân pin
  pinMode(v1, OUTPUT);
  pinMode(v2, OUTPUT);
  pinMode(v3, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(v ,INPUT);
  pinMode(r1 ,INPUT);
  pinMode(r2 ,INPUT);
  pinMode(pmp ,INPUT);
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(A0, INPUT);
  pinMode(A4, INPUT);
  /*pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);*/
  digitalWrite(pump, HIGH);
  digitalWrite(v1, HIGH);
  digitalWrite(v2, HIGH);
  digitalWrite(v3, HIGH);
  digitalWrite(rled, HIGH);
  digitalWrite(gled, LOW);
}
//==============================Hàm gửi JSON==============================
void jsonSend(){
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject(); //Tạo root cho chuỗi Json
  JsonObject& sensor = root.createNestedObject("sensor"); //Thêm các Object hoặc Array cho chuỗi
  JsonArray& sensor_moist = sensor.createNestedArray("moist");
  JsonArray& valve = root.createNestedArray("valve");
  for(int i=0; i<3; i++){
    sensor_moist.add(moistRead(i));
    valve.add(vstate[i]);
  }
  sensor["rainRead"] = rainRead(); //Gán các giá trị vào Object hoặc Array
  sensor["lightRead"] = lightRead();
  root["upper"]=up;
  root["lower"]=low;
  root["pumpstate"] = pstate;
  root["auto"] = autoo;
  //root["waterlvl"] = lowwtr;
  //root.prettyPrintTo(Serial); //In ra màn hình để tiện debug
  root.printTo(megaserial); //Gửi root qua cổng Serial mềm
  jsonBuffer.clear();
}
 
void loop(){
  unsigned long now = millis();
  while(megaserial.available()){ //check xem serial có đang trả về giá trị nào k
    DynamicJsonBuffer ctrl; //tạo json buffer mới với tên jBuff
    JsonObject& root = ctrl.parseObject(megaserial); //gán thẳng giá trị từ serial vào object
    if(root.success()){
      root.prettyPrintTo(Serial);
      resolvemsg(root);
    }else{
      Serial.println("failed");
      break;
    }
  }
  if(local==1){ //Đổi màu của đèn theo state local và chạy hàm nút điều khiển
    digitalWrite(rled, HIGH);
    digitalWrite(gled, LOW);
    button();
  }else{
    digitalWrite(rled, LOW);
    digitalWrite(gled, HIGH);
  }
  if(autoo==1){
    automation();
  }
  if(now-prevmillis >= 2000){ //Cập nhật lcd mỗi 2s
    prevmillis=now;
    lcdSet();
    lcdupdate();
  }
  if(state==1){ //Tự động gửi JSON mỗi 1s
    if(local==0){
      if(now-sendmillis >= 1000){
        sendmillis=now;
        jsonSend();
      }
    }
  }
}
//==============================Hàm clear màn hình==============================
void clearDisplay() { //Sử dụng hàm này thay cho lcd.clear vì lcd.clear sẽ làm interrupt hệ thống
  for (int i=0; i < 80; i++) {
    lcd.write(' ');  //In liên tục 80 dấu cách (tương tự kí tự trống)
  }
}
//==============================Set các giá trị không đổi trên lcd==============================
void lcdSet(){
  lcd.setCursor(7,0);
  lcd.print("L:");
  lcd.setCursor(0,0);
  lcd.print("T1:");
  lcd.setCursor(0,1);
  lcd.print("T2:");
  lcd.setCursor(6,1);
  lcd.print("T3:");
}
//==============================Ghi các giá trị của cảm biến lên lcd==============================
void lcdupdate(){
  lcd.setCursor(8,0);
  lcd.print(lightRead()); //Mức ánh sáng
  lcd.setCursor(3,0);
  lcd.print(moistRead(0)); //Độ ẩm tầng 1
  lcd.setCursor(3,1);
  lcd.print(moistRead(1)); //Độ ẩm tầng 2
  lcd.setCursor(9,1);
  lcd.print(moistRead(2)); //Độ ẩm tầng 3
  lcd.setCursor(12,1);
  if(autoo == 1){
    lcd.print("AUTO"); //Thông báo chế độ AUTO
  }
  if(pstate == 1){
	lcd.print("PUMP"); //Thông báo máy bơm
  }
}
//==============================Hàm xử lý JSON nhận==============================
void resolvemsg(JsonObject& jobj){//Các hàm containsKey sẽ kiểm tra chuỗi JSON có string đã cho không rồi thực hiện lệnh
  if(jobj.containsKey("motor")){ //Set state cho các motor nếu hệ thống bị tắt đột ngột
    up=jobj["motor"][0];
    low=jobj["motor"][1];
    state=1;
    megaserial.print("{'done':1}");
  }
  if(jobj.containsKey("setauto")){
    lmin=jobj["light"][0];
    lmax=jobj["light"][1];
    rmax=jobj["rain"];
    for(int i=0; i<3; i++){
      mmin[i]=jobj["valveMin"][i];
      mmax[i]=jobj["valveMax"][i];
    }
  }
  if(jobj.containsKey("local")){
    local=jobj["local"];
  }
  if(jobj.containsKey("upper")){
    uppertray(jobj["upper"]);
  }
  if(jobj.containsKey("lower")){
    lowertray(jobj["lower"]);
  }
  if(jobj["req_data"] == 1){
    jsonSend();
  }
  if(jobj.containsKey("pump")){
    wtrpump(jobj["pump"]);
  }
  if(jobj.containsKey("auto")){
    autoo = jobj["auto"];
  }
  if(jobj.containsKey("valve")){
    for(int i=0; i<3; i++){
      if(jobj["valve"] == "v"+String(i+1)+"on"){
        if(vstate[i]==0){
          vctrl(i);
        }
      }else if(jobj["valve"] == "v"+String(i+1)+"off"){
        if(vstate[i]==1){
          vctrl(i);
        }
      }
    }
  }
}
//==============================Hàm kiểm tra mực nước==============================
/*void checkWtr(){
  if(digitalRead(18) == HIGH){
    lowwtr = 0;
  } else{
    lcd.setCursor(4,0);
    lcd.print("Alert!");
    lcd.setCursor(1,1);
    lcd.print("Change water");
    lowwtr = 1;
  }
}*/
//==============================Các hàm đọc giá trị sensors==============================
int rainRead(){ //% lượng mưa(detect quá 60% = mưa)
  float value = analogRead(A0);
  return 100-(value/1023.0)*100;
}
long lightRead(){
  float value = analogRead(A4);
  return (1-(value/1023.0))*188000;
}
int moistRead(int x){ //đọc độ ẩm đất theo %
  byte Pins[]= {pin1, pin2, pin3};
  float value = analogRead(Pins[x]);   
  return 100-(value/1023.0)*100;
}
//==============================Hàm điều khiển ray trên==============================
void uppertray(int u) {  //check trạng thái khay trên rồi mở
  clearDisplay();
  if(u == 1){
    if (up==0){
      lcd.setCursor(2,1);
      lcd.print("Opening upper");
      upperS.step(-3*stepsPerRevolution);
      up=1;
    }
  }else{
    if (up==1){
      lcd.setCursor(2,1);
      lcd.print("Closing upper");
      upperS.step(3*stepsPerRevolution);
      up=0;
    }
  }clearDisplay();
}
//==============================Hàm điều khiển ray dưới==============================
void lowertray(int l){
  clearDisplay();
  if(l == 1){
    if(low==0){
      lcd.setCursor(2,1);
      lcd.print("Opening lower");
      lowerS.step(-5*stepsPerRevolution);
      low=1;
    }
  }else{
    if(low==1){
      lcd.setCursor(2,1);
      lcd.print("Closing lower");
      lowerS.step(5*stepsPerRevolution);
      low=0;
    }
  }clearDisplay();
}
//==============================Hàm điều khiển máy bơm==============================
void wtrpump(int w){
  if(w == 1){
    if (digitalRead(pump)== HIGH) {
      digitalWrite(pump, LOW);
      pstate = 1;
    }
  }else{
    if (digitalRead(pump)== LOW){
      digitalWrite(pump, HIGH);
      pstate = 0;
    }
  }
}
//==============================Hàm điều khiển các van nước==============================
void vctrl(int v){
  int valve[3]={v1,v2,v3};
  if (digitalRead(valve[v]) == HIGH){
    digitalWrite((valve[v]), LOW);
    vstate[v] = 1;
  }else{
    digitalWrite((valve[v]), HIGH);
    vstate[v]= 0;
  }
}
//==============================Hàm tự động xử lý==============================
void automation(){
  //checkWtr();
  if (rainRead() > rmax && lightRead() < lmin){ //đọc giá trị nếu quá 60% nước trên bề mặt(mưa lớn) sẽ đưa chậu vào
      if(up==1){
        uppertray(0);
      }
      if(low==1){
        lowertray(0);
      }
  }
  if (lightRead() >= lmax){
    //Serial.println("Hey it's sunny, let's get all the flower out");
    if(up==0){
        uppertray(1);
    }
    if(low==0){
      lowertray(1);
    }
  }
  for(int i=0; i<3; i++){
    if(moistRead(i) <= mmin[i] && rainRead() < 40){
        if(pstate == 0){
          wtrpump(1);
        }
        switch(i){
          case 0:
          if(vstate[i]==0){
            vctrl(i);
          }break;
          case 1:
          if(up == 0){
            uppertray(1);
          }
          if(vstate[i]==0){
            vctrl(i);
          }break;
          case 2:
          if(low == 0){
            lowertray(1);
          }
          if(vstate[i]==0){
            vctrl(i);
          }break;
        }
    }else if(moistRead(i) > mmax[i]||rainRead()>60){
      if(vstate[0]==0 && vstate[1]==0 && vstate[2]==0){
        wtrpump(0);
      }
      switch(i){
        case 0:
        if(vstate[i]==1){
          vctrl(i);
        }break;
        case 1:
        if(moistRead(i)>mcao){
          uppertray(0);
        }
        if(vstate[i]==1){
          vctrl(i);
        }break;
        case 2:
        if(moistRead(i)>mcao){
          lowertray(0);
        }
        if(vstate[i]==1){
          vctrl(i);
        }break;
      }
    }
  }
}
//==============================Nút điều khiển local==============================
void button(){
  int r1_statenow = digitalRead(r1); //Tạo trạng thái cho nút bấm để tránh viẹc chạy liên tục
  int r2_statenow = digitalRead(r2);
  int pmp_statenow = digitalRead(pmp);
  if(r1_state != r1_statenow){
    r1_state = r1_statenow;
    uppertray(digitalRead(r1));
  }
  if(r2_state != r2_statenow){
    r2_state = r2_statenow;
    lowertray(digitalRead(r2));
  }
  for(int i=0; i<3; i++){
    if(digitalRead(v[i])==1){
      if(vstate[i]==0) vctrl(i);
    }else{
      if(vstate[i]==1) vctrl(i);
    }
    if(vstate[i]==1){
      if(pmp_state != pmp_statenow){
        pmp_state = pmp_statenow;
        wtrpump(digitalRead(pmp));
      }
    }
  } 
}
