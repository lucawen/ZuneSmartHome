#include <LiquidCrystal.h>   
#include <SoftwareSerial.h>

//--------------------------------- Temperatura Início
  double Thermister(int RawADC) { // Converte voltagem em temperatura
  double Temp;
  Temp = log(((10240000/RawADC) - 10000));
  // Asumindo o uso de um Thermistor 10k.
  //Calculo Atual: Resistance = (1024 * BalanceResistor/ADC) - BalanceResistor
 
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  //in case you need farenheit:
  // Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  return Temp;
}
//--------------------------------- Temperatura FIM

//--------------------------------- Simbolos
byte term[8] = { // Termômetro
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};
byte celcius[8] = { // Celcius
  B00000,
  B10110,
  B01001,
  B01000,
  B01000,
  B01001,
  B00110,
  B00000
};
 
byte lamp[8] = { // Lâmpada
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B01110,
  B01110,
  B00100
};
//--------------------------------- Simbolos FIM

//Lâmpadas (LEDS)
int Quarto  =  10;
int Sala  =  9;

//Estados
bool QuartoEstado  = false; 
bool SalaEstado  = false;

//Hardware
int Buzzer = 6;

//Sensor
const int PinoSensor = 0; // Sensor Temperatura Pinagem em Analógico 0
int temperatura;

// Outros
int ligarPort;
int lcdOpt;
int tMsg;
char intCommand;

// Bluetooth
const int TX_BT = 7;
const int RX_BT = 8;
SoftwareSerial btSerial(TX_BT, RX_BT);

//Frequencia para enviar mensagens periodicas pro WP em milesegundos.
const unsigned long periodicMessageFrequency = 5000;
unsigned long time = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//Processar comando vindo do Windows Phone.
void processCommand(char* command) {
  if(strcmp(command,"TURN_QUARTO") == 0) {
    intCommand = '1';
    ligar(Quarto);
    lcdMsg(1, 1000);
  } else if(strcmp(command,"TURN_SALA") == 0) {
    intCommand = '2';
    ligar(Sala);
    lcdMsg(2, 1000);
  }
}

//Envia mensagem de volta para o Windows Phone.
void sendMessage(char* message) {
  int messageLen = strlen(message);
  if(messageLen < 256) {  
    btSerial.write(messageLen);
    btSerial.print(message);
  }
}

//Send a set of periodic messages to the Windows Phone.
//This message could be a sensor data, like a thermometer data.
void sendPeriodicMessages() {
    //char *s;
    //sprintf(s,"TEMPERATURA:%d",temperatura);
    //sendMessage(s);
}

void setup(){
  //Icones
  lcd.createChar(1, term);
  lcd.createChar(2, celcius);
  lcd.createChar(3, lamp);

  //LCD
  lcd.begin(16, 2);

  //Serial
  Serial.begin(9600);  
  Serial.println("USB Conectado!!!");
  btSerial.begin(9600);

  pinMode(Quarto, OUTPUT);
  pinMode(Sala, OUTPUT);
  pinMode(Buzzer, OUTPUT);
 }

 void lcdMsg(int lcdOpt, int tMsg){ 
  switch (lcdOpt) {
      case 1:
      if (QuartoEstado == true){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   QUARTO");
        lcd.setCursor(0,1);
        lcd.print("   LIGADO");
        delay(tMsg);
        lcd.clear();
        }else if (QuartoEstado == false){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("   QUARTO");
          lcd.setCursor(0,1);
          lcd.print("   DESLIGADO");
          delay(tMsg);
          lcd.clear();
        }
      beep();
        break;
      case 2:
      if (SalaEstado == true){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   SALA");
        lcd.setCursor(0,1);
        lcd.print("   LIGADO");
        delay(tMsg);
        lcd.clear();
        }else if (SalaEstado == false){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("   SALA");
          lcd.setCursor(0,1);
          lcd.print("   DESLIGADO");
          delay(tMsg);
          lcd.clear();
        }
      beep();
        break;
        default:
        lcd.setCursor(0,0);   
        lcd.print("GordoxAutomation");   
        lcd.setCursor(0,1);
        lcd.write(1);
        lcd.setCursor(1,1);
        lcd.print(temperatura);
        lcd.setCursor(3,0);
        lcd.write(2);
    }
  }

 void loop(){
  if(btSerial.available()) {
      int commandSize = (int)btSerial.read();
      char command[commandSize];
      int commandPos = 0;
      while(commandPos < commandSize) {
        if(btSerial.available()) {
          command[commandPos] = (char)btSerial.read();
          commandPos++;
        }
      }
      command[commandPos] = 0;
      processCommand(command);
  }
  unsigned long currentTime = millis();
  if((currentTime - time) > periodicMessageFrequency) {
    sendPeriodicMessages();
    time = currentTime;
  }
  lcdMsg(0, 600);
  //temperatura = double(Thermister(analogRead(PinoSensor))); // Atualização Temperatura
}

 void ligar(int ligarPort){
  if(digitalRead(ligarPort) == HIGH){
      digitalWrite(ligarPort, LOW);
      if(intCommand == '1'){
        sendMessage("QUARTO:OFF");
        QuartoEstado = false;
        }else if(intCommand == '2'){
          SalaEstado = false;
          sendMessage("SALA:OFF");
        }
    }else  {
      digitalWrite(ligarPort, HIGH);
      if(intCommand == '1'){
        QuartoEstado = true;
        sendMessage("QUARTO:ON");
        }else if(intCommand == '2'){
          SalaEstado = true;
          sendMessage("SALA:ON");
        }
    } 
 }

void beep(){
  digitalWrite(Buzzer, HIGH);
  delay(260);
  digitalWrite(Buzzer, LOW);
  delay(100);
  digitalWrite(Buzzer, HIGH);
  delay(200);
  digitalWrite(Buzzer, LOW);
}