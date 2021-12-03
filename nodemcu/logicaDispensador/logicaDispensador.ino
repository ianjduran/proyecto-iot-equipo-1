#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiClient.h>

// Definicion de Pines
#define SS_PIN 15 //D8
#define RST_PIN 16 //D0
#define green_led D1//5 //D1
#define red_led D2//4 //D2

int minutoActual;
int horaActual;

struct Mascota {
  String id;
  String nombre;
  int intervaloComida;
  int horaUltimaComida;
  int minutoUltimaComida;

  Mascota(String id, String nombre, int intervaloComida){
     this->id = id;
     this->nombre = nombre;
     this->intervaloComida = intervaloComida;
     this-> horaUltimaComida= -1;
     this-> minutoUltimaComida = -1;
  }
};

Mascota perro1("1A E8 C2 82","Rambo", 6);
Mascota perro2("9C E8 76 6E","Luna", 6);

// Credenciales de la red
char *ssid     = "vw-10411";
char *password = "ZTERRTHK7503436";

// Variables para HTTP POST
String URL = "http://192.168.1.240:3000/api/experimentos_web_api/";
WiFiClient wclient;
HTTPClient httpClient;

//wifi time
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP, "pool.ntp.org");
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instancia de la clase

MFRC522::MIFARE_Key key; 

// Arreglo que almacenara el nuevo NUID 
byte nuidPICC[4];

void setup() { 
  pinMode(green_led,OUTPUT);
  pinMode(red_led,OUTPUT);

  Serial.begin(115200);
  Serial.println();
  
  // Inicializa la conexion wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(red_led,HIGH);
    digitalWrite(green_led,HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(red_led,LOW);
    digitalWrite(green_led,LOW);
    delay(250);
  }
  Serial.println();

  
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone
  timeClient.setTimeOffset(-21600); //gmt-6
   
  // INICIALIZACION RFID
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  
}
 
void loop() {
  timeClient.update(); // Actualiza la hora desde el servidor NTP
  
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  
  // Inidca que tipo de tarjeta es
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  String id = uidToStr(rfid.uid.uidByte, rfid.uid.size);
  minutoActual = timeClient.getMinutes(); 
  horaActual = timeClient.getHours();
  
  Serial.print(horaActual);
  Serial.print(":");  
  Serial.println(minutoActual);
  Serial.println(id);
  
  Serial.println();
  Serial.println(F("New card detected."));
  Serial.println(F("Intento Identificado"));  
  
  if(id==perro1.id){
    Serial.println(F("Mascota 1: "));
    Serial.println(perro1.nombre);
    Serial.print(F("Intervalo de comida: "));
    Serial.println(perro1.intervaloComida);
    
    if(perro1.horaUltimaComida==-1){
        Serial.println();
        Serial.println(F("Primera Comida!"));
        Serial.println();
        logIntento(perro1.id,1);
        turnLedOk();
        perro1.horaUltimaComida = horaActual;
        perro1.minutoUltimaComida = minutoActual;
        
    }
    else{
      checkFood(perro1,horaActual,minutoActual);
    }
  } else if(id==perro2.id){
    Serial.println(F("Mascota 2: "));
    Serial.println(perro2.nombre);
    Serial.print(F("Intervalo de comida: "));
    Serial.println(perro2.intervaloComida);
    if(perro2.horaUltimaComida==-1){
        Serial.println();
        Serial.println(F("Primera Comida!"));
        Serial.println();
        logIntento(perro2.id,1);
        turnLedOk();
        perro2.horaUltimaComida = horaActual;
        perro2.minutoUltimaComida = minutoActual;
        
    }
    else{
      checkFood(perro2,horaActual,minutoActual);
    }
  }
  else{
    Serial.println(F("La mascota no se encuentra registrada"));
  }


  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper to dump a byte array as hex values to String. 
 */

String uidToStr(byte *buffer, byte bufferSize) {
  String str;
  for (byte i = 0; i < bufferSize; i++) {
    str = str + String(buffer[i] < 0x10 ? " 0" : " ");
    String aux = String(buffer[i],HEX); 
    aux.toUpperCase();
    str = str + aux;
  }
  str.remove(0,1);
  return str;
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Funcion que enciende un Led Verde indicando que el intento de pedir comida fue valido
 */

void turnLedOk(){
  digitalWrite(green_led,HIGH); 
  delay(1000);
  digitalWrite(green_led,LOW);
}

/**
 * Funcion que enciende un LED Rojo indiciando que el intento de pedir comida fue inválido
 */
void turnLedNOk(){
  int del = 200;
  int numBlink=3;
  for(int i=0;i<numBlink;i++){
    digitalWrite(red_led,HIGH);
    delay(del);
    digitalWrite(red_led,LOW);
    delay(del);
  }
}

void checkFood(Mascota &perro, int hours, int minutes){
  String id = perro.id;
  int interval = perro.intervaloComida;
  float diffHour, diffMin;
  // En caso que el minuto final sea menor al minuto inicial
  // se debe llevar la cuenta con un carry para restarlo en 
  // la diferencia de horas
  int carry=0; 
  // Se calcula la diferencia en minutos
  diffMin = minutes - perro.minutoUltimaComida;
  if(diffMin<0){
    diffMin+=60;
    carry=1;
  }
  // Se calcula la diferencia en horas
  diffHour = hours - perro.horaUltimaComida - carry;
  if(diffHour<0){
    diffHour+=24;
  }
  
  Serial.print(F("Pasaron: "));
  Serial.print((int)diffHour);
  Serial.print(F(" Hora(s) y  "));
  Serial.print((int)diffMin);
  Serial.println(F(" Minuto(s) desde la ultima comida. "));
  int flag = 0;
  diffHour+=diffMin/60;
  if(diffHour<interval){
    float diffInterval = interval-diffHour;
    Serial.print(F("Invalido. Faltan "));
    Serial.print(diffInterval);
    Serial.println(F(" Hora(s) "));
    turnLedNOk();
  }
  else{
    Serial.println("Intento Valido :)");
    perro.horaUltimaComida = hours; // hours -> hora actual
    perro.minutoUltimaComida = minutes;
    turnLedOk();
    flag = 1;
  }
  logIntento(id, flag);
}


void logIntento(String id, int estado){
  // Verifica que haya conexión a internet
  if(WiFi.status() == WL_CONNECTED){
    id.replace(" ", "%20"); 
    String data = URL;
    data = data + id + "/" + estado;
    Serial.println(data);
    httpClient.begin(wclient, data.c_str());      //Specify request destination
    httpClient.addHeader("Content-Type", "Content-Type: application/json");
    int httpResponseCode = httpClient.POST(data.c_str());
    Serial.println(httpResponseCode);
    httpClient.end();
  }
  return;
  
}
