#include <SPI.h>
#include <LoRa.h>
#include <LowPower.h>
// "ArduinoLowPower.h"

const int csPin = 10;
const int resetPin = 7;
const int irqPin = 2;
String dataconfirm = "";
String sensorData;
String sensorData1;
String sensorData2;
String sData1;
String sData2;
String dataheader = "s1";
String waktutidur = "";
char datachar;

byte localAddress = 0xAA;
byte destinationAddress = 0xBB;
long lastSendTime = 0;
int interval = 100;
long lastSendTimeSerial = 0;
int intervalSerial = 100;
int count = 0;
bool stringComplete = false;
bool stringComplete1 = false;
bool stringComplete2 = false;
String waktukumpuldata = "";
bool waktud1 = true;
bool waktud2 = false;
String inputString = ""; 
int pinOut = 11;
String datakonfirmttl = "1";

int counterlora =0;
int counterttl = 0;

void(*saya_reset) (void) = 0;

void programsleep() {
  counterlora = 0;
  counterttl = 0;
  Serial1.flush();
  sensorData = "";
  sensorData1 = "";
  sensorData2 = "";
  sData1 = "";
  sData2 = "";
  
  for (int i = 0; i < 72; i++)  // i*8s sleep time
  {
    // LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
    //               SPI_OFF, USART0_OFF, TWI_OFF);

    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER5_OFF, TIMER4_OFF, TIMER3_OFF, 
  		  TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART3_OFF, 
  		  USART2_OFF, USART1_OFF, USART0_OFF, TWI_OFF);
  }
  waktutidur = "";
  dataconfirm = "";
  waktud1 = true;
  stringComplete = false;
  Serial.print("sensorData = ");
  Serial.println(sensorData);
  Serial.println("data kereset semua");
  saya_reset();
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.write(destinationAddress);  //0xBB
  LoRa.write(localAddress);        //0xAA
  LoRa.write(outgoing.length());   //panjang data
  LoRa.print(outgoing);            //print data
  LoRa.endPacket();
}

void kirimkonfirm()
{
    Serial1.println(datakonfirmttl);
}

void receiveMessage(int packetSize) 
{
  // Serial.println("sebelum mode nerima");
  if (packetSize == 0) return;
  // Serial.println("masuk mode nerima");

  delay(100);

  int recipient = LoRa.read();
  // byte sender = LoRa.read();
  // byte incomingLength = LoRa.read();

  String incoming = "";

  while (LoRa.available()) 
  {
    incoming += (char)LoRa.read();
  //    Serial.print((char)LoRa.read());
  }

  // if (incomingLength != incoming.length()) {
  //   Serial.println("Error: Message length does not match length");
  //   return;
  // }

   if (recipient != localAddress) {
     Serial.println("Error: Recipient address does not match local address");
     return;
   }

  Serial.print("Received data " + incoming);
//  Serial.print(" from 0x" + String(sender, HEX));
  Serial.println(" to 0x" + String(recipient, HEX));
  dataconfirm = incoming;
  delay(1000);
}

void relay()
{
  if (waktutidur == ""){
    digitalWrite(pinOut, HIGH);  
  }
  else {
    digitalWrite(pinOut, LOW);
  }
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("Start LoRa duplex");

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(920E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true) {}
  }

  pinMode(11, OUTPUT); //pin control relay 
}

void loop() {
  delay(100);
  
  if (waktutidur == "")
  {
    digitalWrite(pinOut, HIGH);  
  }
  else 
  {
    digitalWrite(pinOut, LOW);
  }
  
  Serial1.begin(9600);
  
  delay(2000);   // memberikan waktu untuk master node mengambil data
  
  Serial.println("Menunggu data sensor");
  
  while (Serial1.available() && waktud1 == true) {  // if there's incoming serial data
    char inChar = (char)Serial1.read();  // read it
    if (inChar == 's') {        // if the incoming character is a newline      
      sensorData1 = "";
      stringComplete1 = false;     // set stringComplete to true
      sensorData1 += inChar;
    } 
    else if (inChar == '\n')
    {
      stringComplete1 = true;
      // Serial.println("true1");
      Serial.print("data1: ");
      Serial.println(sensorData1);
      delay(500);
      waktud1 = false;
      waktud2 = true;
      break;
    }
    else if(!stringComplete1) 
    {
      sensorData1 += inChar;
      // Serial.println("false1");
    }
  }

  while (Serial1.available() && waktud2 == true) {  // if there's incoming serial data
    char inChar = (char)Serial1.read();  // read it
    if (inChar == 'd') {        // if the incoming character is a newline
      sensorData2 = "";
      stringComplete2 = false;     // set stringComplete to true
      sensorData2 += inChar;
    } 
    else if (inChar == 'e'){
      stringComplete2 = true;
      // Serial.println("true2");
      Serial.print("data2: ");
      Serial.println(sensorData2);
      waktud2 = false;
      waktukumpuldata = "1";
      break;
    }
    else 
    {
      if(!stringComplete2) sensorData2 += inChar;
      // Serial.println("false2");
    }


  }

  while (waktukumpuldata == "1")
  {
    String sData1 = String(sensorData1);
    int lastStringLength = sData1.length();
    sData1.remove((lastStringLength-1), 1);
    sensorData = sData1 + "," + sensorData2 + "e1";
    Serial.print("Data dari sensor: ");
    Serial.println(sensorData);
    delay(500);
    stringComplete = true;
    waktukumpuldata = "";
  }

  if (counterttl >75)
  {
    kirimkonfirm();
    digitalWrite(pinOut, LOW);
    waktutidur = "1";
    Serial.println("data ttl melebihi batas, sleep");
    programsleep();
    return;
    
  }

  // sensorData = "s1,46,247,507,1009,0,0,215,29,31,0,65,20,490,461,194,d2,2024/02/05,09:16:46,e1";

  if(dataconfirm =! "ok")
  {
    String firstTwo = sensorData.substring(0,2);
      // Serial.println(sensorData);
      
    if (sensorData == "")
    {
      Serial.println("data belum diterima");
      delay(500);
      counterttl++;
      return;
    }
    else if (dataheader != firstTwo)
    {
      Serial.println("data header salah");
      delay(500);
      counterttl++;
      return;
    }
    else if (sensorData.length() < 3)  //dihitung dari panjang data
    {
      delay(500);
      counterttl++;
      return;
    }
    else
    {
      kirimkonfirm();
      delay(500);
      Serial1.flush();
    }
      
    while (dataconfirm != "ok") 
    {
      if (millis() - lastSendTime > interval) 
      {
        // String sensorData = "123,1234567890,12.1,30,96,1234,1234,96,100,100,123,96";
        // Serial.print(sensorData);
        sendMessage(sensorData);

        Serial.print("Sending data " + sensorData);
        Serial.print(" from 0x" + String(localAddress, HEX));
        Serial.println(" to 0x" + String(destinationAddress, HEX));
      

        lastSendTime = millis();
        interval = random(3000) + 1000;
        counterlora++;
        
      }

      //Serial.println("123");
      receiveMessage(LoRa.parsePacket());

      if(counterlora>20) //harusnya 30
      {
        digitalWrite(pinOut, LOW);
        waktutidur = "1";
        Serial.println("datalora melebihi batas, sleep");
        delay(100);
        programsleep();
        return;
      }
      
    }
  }
  else
  {
    delay(100);
    return;
  }

  Serial.println("Terima dataconfirm : " + dataconfirm);
  Serial.println("Sleep mas Bro dan Perintah relay open");
  digitalWrite(pinOut, LOW);
  waktutidur = "1";

  delay(1000);
  programsleep();

  Serial.println("Bangun Sleep");
  delay(1000);

}
