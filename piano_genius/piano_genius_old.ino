/*#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "pitches.h"

String user = "grupo1-bancadaB1";
String passwd = "L@Bdygy1B1";

const char* ssid = "matheus";
const char* password = "ixxu4151";
const char* mqtt_server = "labdigi.wiseful.com.br";
//const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};


int doremifa[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, 
                  NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_D4, NOTE_D4,
                  NOTE_C4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4,
                  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4,
                  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, -1};


int doremifa_d[] = {250, 250, 250, 250, 200, 200,
                    250, 250, 250, 250, 200, 200,
                    250, 250, 250, 250, 200, 200,
                    250, 250, 250, 250, 200, 200,
                    250, 250, 250, 250, 200, 200};


int tetris[] = {NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4,
                NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4,
                NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_D5,
                NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_E5,
                NOTE_D5, NOTE_C5, -1};
                
int tetris_d[] = {750, 250, 250, 500, 250, 250, 750,
                  250, 250, 500, 250, 250, 750, 250,
                  250, 500, 500, 500, 500, 1000, 700,
                  250, 750, 250, 250, 750, 250, 750,
                  250, 250, -1};

int fallen[] = {NOTE_FS5, NOTE_CS5, NOTE_FS5, NOTE_CS5, NOTE_FS5, NOTE_CS5, 
                NOTE_FS5, NOTE_CS5, NOTE_FS5, NOTE_CS5, NOTE_FS5, NOTE_CS5, 
                NOTE_B4, NOTE_A4, NOTE_CS5, NOTE_A4, NOTE_B4, NOTE_E5,
                NOTE_DS5, NOTE_E5, NOTE_FS5, NOTE_DS5, NOTE_B4, NOTE_FS5,
                NOTE_A4, NOTE_FS5, NOTE_A4, NOTE_FS5, NOTE_A4, NOTE_FS5, -1};

int fallen_d[] = {250, 250, 250, 250, 250, 250,
                  250, 250, 250, 250, 250, 250,
                  250, 250, 250, 250, 250, 250,
                  250, 250, 250, 250, 250, 250,
                  250, 250, 250, 250, 250, 250};

int mario[] = {NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5,
               NOTE_G5, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4,
               NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_G4,
               NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
               NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_G5,
               NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_E5, NOTE_E5, -1};

int mario_d[] = {250, 250, 250, 250, 250,
                 250, 250, 250, 250, 250,
                 250, 250, 250, 250, 250,
                 250, 250, 250, 250, 250,
                 250, 250, 250, 250, 250,
                 250, 250, 250, 250, 250};


int song = 0;
int itera = -1;

int value = 0; //sinal de heartbeat

bool prevBtn1 = 0; //estado anterior do botão
bool btn1 = 0; //estado atual do botão

bool prevBtn2 = 0; //estado anterior do botão
bool btn2 = 0; //estado atual do botão

bool prevBtn3 = 0; //estado anterior do botão
bool btn3 = 0; //estado atual do botão

bool prevBtn4 = 0; //estado anterior do botão
bool btn4 = 0; //estado atual do botão

bool led = 0; //estado do led

uint32_t prev_millis;
uint32_t ms_cnt = 0;

const char* zero_cstr = "0";
const char* one_cstr = "1";

int valor;

void setup_wifi() {

  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  // Controle leds
  
  if (strcmp(topic,(user+"/S0").c_str())==0) {
    if ((char)payload[0] == '1') {
      digitalWrite(D3, HIGH);
    } else {
      digitalWrite(D3, LOW);
    }
  }
  
  if (strcmp(topic,(user+"/S1").c_str())==0) {
    if ((char)payload[0] == '1') {
      digitalWrite(D4, HIGH);
    } else {
      digitalWrite(D4, LOW);
    }
  }
  
  if (strcmp(topic,(user+"/S2").c_str())==0) {
    if ((char)payload[0] == '1') {
      digitalWrite(D8, HIGH);
    } else {
      digitalWrite(D8, LOW);
    }
  }

  if (strcmp(topic,(user+"/S3").c_str())==0) {
    if ((char)payload[0] == '1') {
      digitalWrite(D0, HIGH);
    } else {
      digitalWrite(D0, LOW);
    }
  }  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = user;
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), user.c_str(), passwd.c_str())) {
      
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish((user+"/homehello").c_str(), "hello world");

      client.subscribe((user+"/ledhome").c_str());

      client.subscribe((user+"/S0").c_str());
      client.subscribe((user+"/S1").c_str());
      client.subscribe((user+"/S2").c_str());
      client.subscribe((user+"/S3").c_str());
      
    } else {
      
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
      
    }
  }
}



void setup() {
  pinMode(D1, INPUT); // Botão 0
  pinMode(D2, INPUT); // Botão 1
  pinMode(D5, INPUT); // Botão 2
  pinMode(D6, INPUT); // Botão 3

  pinMode(D3, OUTPUT); // Led 0 topico S0
  pinMode(D4, OUTPUT); // Led 1 topico S1
  pinMode(D8, OUTPUT); // Led 2 topico S2
  pinMode(D0, OUTPUT); // Led 3 topico S3

  pinMode(D7, OUTPUT); // Buzzer

  Serial.begin(115200);
  
  setup_wifi();
  
  client.setServer(mqtt_server, 80);
  client.setCallback(callback);

  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(D7, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }


}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(prev_millis!=millis()){
    prev_millis=millis();
    if(ms_cnt%100==0){
      
      btn1 = digitalRead(D1); //leitura do estado do botão
      if (!prevBtn1 and btn1){
          if (itera == 32) {
            itera = -2;
          }
          if(itera == -1){
            song = 1;
          }
          if (doremifa[itera] == -1){
            itera = -2;
          }
          Serial.println("botão 1 pressionado (borda de subida)");
          client.publish((user+"/E3").c_str(), "1"); // botão1 virtual
          if (itera >= 0){
            switch(song){
              case 1:
                tone(D7, doremifa[itera], doremifa_d[itera]);
                break;
              case 2:
                tone(D7, tetris[itera], tetris_d[itera]);
                break;
              case 3:
                tone(D7, fallen[itera], fallen_d[itera]);
                break;
              case 4:
                tone(D7, mario[itera], mario_d[itera]);
                break;
            }
          
          }
        itera++;
      }
      prevBtn1 = btn1;

      btn2 = digitalRead(D2); //leitura do estado do botão
      if (!prevBtn2 and btn2){
          if (itera == 32) {
            itera = -2;
          }
          if(itera == -1){
            song = 2;
          }
          if (doremifa[itera] == -1){
            itera = -2;
          }
          
          Serial.println("botão 2 pressionado (borda de subida)");
          client.publish((user+"/E4").c_str(), "2"); // botão virtual
          if (itera >= 0){
            switch(song){
              case 1:
                tone(D7, doremifa[itera], doremifa_d[itera]);
                break;
              case 2:
                tone(D7, tetris[itera], tetris_d[itera]);
                break;
              case 3:
                tone(D7, fallen[itera], fallen_d[itera]);
                break;
              case 4:
                tone(D7, mario[itera], mario_d[itera]);
                break;
            }
          
          }
        itera++;
      }
      prevBtn2 = btn2;

      btn3 = digitalRead(D5); //leitura do estado do botão
      if (!prevBtn3 and btn3){
          if (itera == 32) {
            itera = -2;
          }
          if(itera == -1){
            song = 3;
          }
          if (doremifa[itera] == -1){
            itera = -2;
          }
          Serial.println("botão 3 pressionado (borda de subida)");
          client.publish((user+"/E5").c_str(), "3"); // botão virtual
        if (itera >= 0){
            switch(song){
              case 1:
                tone(D7, doremifa[itera], doremifa_d[itera]);
                break;
              case 2:
                tone(D7, tetris[itera], tetris_d[itera]);
                break;
              case 3:
                tone(D7, fallen[itera], fallen_d[itera]);
                break;
              case 4:
                tone(D7, mario[itera], mario_d[itera]);
                break;
            }
          
          }
        itera++;
      }
      prevBtn3 = btn3;

      btn4 = digitalRead(D6); //leitura do estado do botão
      if (!prevBtn4 and btn4){
          if (itera == 32) {
            itera = -2;
          }
          if(itera == -1){
            song = 4;
          }
          if (doremifa[itera] == -1){
            itera = -2;
          }
          Serial.println("botão 4 pressionado (borda de subida)");
          client.publish((user+"/E6").c_str(), "4"); // botão virtual
        if (itera >= 0){
            switch(song){
              case 1:
                tone(D7, doremifa[itera], doremifa_d[itera]);
                break;
              case 2:
                tone(D7, tetris[itera], tetris_d[itera]);
                break;
              case 3:
                tone(D7, fallen[itera], fallen_d[itera]);
                break;
              case 4:
                tone(D7, mario[itera], mario_d[itera]);
                break;
            }
          
          }
        itera++;
      }
      prevBtn4 = btn4;
      
    }
    ms_cnt++;
  }

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "#%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish((user+"/homehello").c_str(), msg);

    client.publish((user+"/E3").c_str(), zero_cstr); // botão virtual
    client.publish((user+"/E4").c_str(), zero_cstr); // botão virtual
    client.publish((user+"/E5").c_str(), zero_cstr); // botão virtual
    client.publish((user+"/E6").c_str(), zero_cstr); // botão virtual
  }
}
*/