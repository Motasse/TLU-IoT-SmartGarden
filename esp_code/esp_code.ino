
#define MQTT_MAX_PACKET_SIZE 1024
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#include <PageBuilder.h>

SoftwareSerial espserial(D5,D6);
long baud = 57600;

const char* mqtt_server = "172.16.6.87";
const char* ap = "flogro";
const char* pw = "66668888";

#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""

ESP8266WebServer Server;
AutoConnect Portal(Server);
WiFiClient wfclient;
PubSubClient client(wfclient);
void setup(){
    Serial.begin(baud);
    espserial.begin(baud);
    Portal.config(ap, pw);
    if(Portal.begin()){
      Serial.println("connected:" + WiFi.SSID());
      Serial.println("IP:" + WiFi.localIP().toString());
    } else {
      Serial.println("connection failed:" + String(WiFi.status()));
      while (1) {
        delay(100);
        yield();
      }
    }
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}
void reconnect() { //Hàm này để kết nối lại client với broker
  //Lặp đến khi nào có kết nối với broker
  while (!client.connected()) {
    Serial.println("Let's try and connect");
    Serial.print("Hmmm...");
    //Tạo một ClientID ngẫu nhiên
    String clientId = "Client-";
    clientId += String(random(0xffff), HEX);
    //Thử kết nối
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("Nice! You're in");
      //Một khi đã kết nối subscribe topic để callback
      //client.publish("flogro/json", "connected");
      espserial.print("{'local':0}");
      client.subscribe("flogro/control");
      client.subscribe("flogro/json");
    } else {
      espserial.print("{'local':1}");
      Serial.println("Uh oh! Houston, we have a problem");
      Serial.print("Here! Grab an rc=");
      Serial.println(client.state());
      Serial.println("We'll be right back after the commercial break");
      // Chờ 2s trc khi thử lại
      delay(2000);
    }
  }
}

void loop(){
  while(espserial.available()){ //Đọc giá trị json gửi qua serial và gán vào bộ nhớ đệm riêng để sử dụng
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(espserial);
    if (root.success()) {
        if(root.containsKey("done")){
            client.unsubscribe("flogro/json");
        }else{
          size_t len = root.measureLength();
          size_t size = len+1;
          char Data[size];
          root.printTo(Data, size);
          client.publish("flogro/json", Data, true); //Dùng tổ hợp có retain message để khi hệ thống sập vẫn giữ được state cuối cùng
        }
    }else{
      Serial.println("Parsing fail");
      break;
    }
  }
  if(!client.connected()) reconnect();
  Portal.handleClient();
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length){
  String str = topic;
  if (str == "flogro/control"){
    payload[length]='\0';
    String s = String((char*)payload);
    //Serial.println(s);
    StaticJsonBuffer<500> jBuff;
    JsonObject& jsonmsg = jBuff.parseObject(s);
    if(jsonmsg.success()){
      jsonmsg.printTo(espserial);
      jsonmsg.printTo(Serial);
    }else{
      Serial.println("Failed! Try again");
    }
    jBuff.clear();
  }
  if(str == "flogro/json"){
    payload[length]='\0';
    String in = String((char*)payload);
    Serial.println(in);
    StaticJsonBuffer<400> stat;
    JsonObject& det = stat.parseObject(in);
    if(det.success()){
      StaticJsonBuffer<100> dat;
      JsonObject& last = dat.createObject();
      JsonArray& motor = last.createNestedArray("motor");
      motor.add(det["upper"]);
      motor.add(det["lower"]);
      last.printTo(espserial);
      last.printTo(Serial);
      dat.clear();
    }else Serial.println("ERROR");
    stat.clear();
  }
}
