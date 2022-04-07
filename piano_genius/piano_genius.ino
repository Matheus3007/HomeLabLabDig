#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "pitches.h"

String user = "grupo1-bancadaB1";
String passwd = "L@Bdygy1B1";

const char* ssid = "matheus";
const char* password = "ixxu4151";
const char* mqtt_server = "labdigi.wiseful.com.br";

WiFiClient espClient;
PubSubClient client(espClient);

int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

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

const String leds_topics[] = {"/S0", "/S1", "/S2", "/S3"};
const uint8_t leds_pins[] = {D3, D4, D8, D0};

const String btn_topics[] = {"/E0", "/E1", "/E2", "/E3"};
const uint8_t btn_pins[] = {D1, D2, D5, D6};

bool led_change = false;
int btn_recieved = 0;
bool timeout = false;

int song = 0;
int itera = -1;

bool btn_values[4] = {false};
bool prev_btn_values[4] = {false};

uint32_t prev_millis;
uint32_t ms_cnt = 0;

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

    for (int i = 0; i < 4; i++) {
        if (strcmp(topic, (user + leds_topics[i]).c_str()) == 0) {
            if ((char)payload[0] == '1') {
                digitalWrite(leds_pins[i], HIGH);

                if (led_change) {
                    timeout = true;
                }

                led_change = true;
            } else {
                digitalWrite(leds_pins[i], LOW);
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (strcmp(topic, (user + btn_topics[i]).c_str()) == 0) {
            if ((char)payload[0] == '1' && btn_recieved == 0) {
                btn_recieved = i + 1;
            }
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
            client.subscribe((user+"/S0").c_str());
            client.subscribe((user+"/S1").c_str());
            client.subscribe((user+"/S2").c_str());
            client.subscribe((user+"/S3").c_str());
            client.subscribe((user+"/E0").c_str());
            client.subscribe((user+"/E1").c_str());
            client.subscribe((user+"/E2").c_str());
            client.subscribe((user+"/E3").c_str());
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
        ms_cnt++;

        if(ms_cnt%100==0){
            for (int i = 0; i < 4; i++) {
                btn_values[i] = digitalRead(btn_pins[i]);

                if (btn_values[i] != prev_btn_values[i]) {
                    prev_btn_values[i] = btn_values[i];

                    if (btn_values[i] == HIGH) {
                        Serial.println("recebeu botão");
                        Serial.println(i);
                        client.publish((user + btn_topics[i]).c_str(), "1");
                    } else {
                        client.publish((user + btn_topics[i]).c_str(), "0");
                    }
                }
            }

            if (itera == -2) {
                if (btn_recieved > 0) {
                    Serial.println("recomecou partida");
                    itera++;
                    btn_recieved = 0;
                }
            }

            if (itera == -1) {
                if (btn_recieved > 0) {
                    Serial.println("escolheu musica");
                    Serial.println(btn_recieved);
                    song = btn_recieved;
                    itera++;
                    btn_recieved = -1;
                }

                return;
            }

            if (led_change) {
                if (btn_recieved == -1) {
                    Serial.println("recebeu led");
                    btn_recieved = 0;
                } else if (btn_recieved > 0) {
                    itera++;
                    Serial.println("proxima nota");

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

                    btn_recieved = -1;

                    if (itera == 30) {
                        itera = -2;
                        btn_recieved = 0;
                    }

                    led_change = false;
                } else if (timeout) {
                    itera++;
                    Serial.println("timeout");
                }
            }
        }
    }
}
