#include <SPI.h>
#include <mcp2515.h>

struct can_frame MasterMsg;
struct can_frame canMsg;

int air_temp = 0;
int air_hum = 0;
int preassure = 0;
int windspeed = 0;
int winddirection = 0;
int soil_temp = 0;
int soil_hum = 0;
int soil1 = 0;
int soil2 = 0;
int gwl = 0;
int lux = 0;
int rain = 0;
String dataseluruh;
String datafinal;
String dataheader = "s1";
String datattl = "";
String statusup = "";

MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  SPI.begin();
}

void AWS(){
  if(air_temp == 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 1; //Frame 1 AWS
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(100);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        if ((canMsg.can_id == 1) && (canMsg.data[0] == 1)){ //air_temp
          air_temp = ((int)(canMsg.data[1] << 8) | canMsg.data[2]);
          air_hum = ((int)(canMsg.data[3] << 8) | canMsg.data[4]);
          preassure = ((int)(canMsg.data[5] << 8) | canMsg.data[6]);
        }
      }
    }
  }
  
  if(air_temp != 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 2; //Frame 2 AWS
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(100);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        if ((canMsg.can_id == 1) && (canMsg.data[0] == 2)) { //wind
          windspeed = ((int)(canMsg.data[1] << 8) | canMsg.data[2]);
          winddirection = ((int)(canMsg.data[3] << 8) | canMsg.data[4]);
        }
      }
    }
  }
}

void Soil(){
  if(soil_temp == 0 && soil2 == 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 3;
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(500);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { 
        if (canMsg.can_id == 2){ //Soil_Temp_Soil_hum
          soil_temp = ((int)(canMsg.data[0] << 8) | canMsg.data[1]);
          soil_hum = ((int)(canMsg.data[2] << 8) | canMsg.data[3]);
          soil1 = ((int)(canMsg.data[4] << 8) | canMsg.data[5]);
          soil2 = ((int)(canMsg.data[6] << 8) | canMsg.data[7]);
        }
      }
    }
  }
}

 void GWL(){
  if(soil2 != 0 && gwl == 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 4;
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(100);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { 
        if (canMsg.can_id == 3){ //GWL
          gwl = ((int)(canMsg.data[0] << 8) | canMsg.data[1]);
        }
      }
    }
  }
}

void LUX(){
  if(gwl != 0 && lux == 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 5;
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(100);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { 
        if (canMsg.can_id == 4){ //LUX
          lux = ((int)(canMsg.data[0] << 8) | canMsg.data[1]);
        }
      }
    }
  }
}

void RAIN(){
  if(lux != 0){
    for (int i = 0; i < 5; i++){
      MasterMsg.can_id  = 0x999; // ID must be unique in a single CAN Bus network.
      MasterMsg.can_dlc = 1; // Data Length Code
      MasterMsg.data[0] = 6;
      mcp2515.sendMessage(&MasterMsg); // Send CAN messaage
      delay(100);
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { 
        if (canMsg.can_id == 5){ //LUX
          rain = ((int)(canMsg.data[0] << 8) | canMsg.data[1]);
        }
      }
    }
  }
}

void loop() {
//  AWS();
//  Soil();
//  GWL();
//  RAIN();
//  LUX();
//  RAIN();

//  dataseluruh = ((dataheader)+(",")+(String(air_temp))+(",")+(String(air_hum))+(",")+(String(preassure))+(",")+(String(windspeed))+(",")+(String(winddirection))+(",")+(String(soil_temp))+(",")+(String(soil_hum))+(",")+(String(soil1))+(",")+(String(soil2))+(",")+(String(gwl))+(",")+(String(lux)));


while(statusup == "")
 {
   dataseluruh = "2023/11/15,13:13:16,243,625,1001,107,270,0,8,9,140,64,110,494,513,192";
   int datalength = dataseluruh.length();
   String stringlength = String(datalength);

   datafinal = ((dataheader)+(",")+(stringlength)+(",")+(dataseluruh));

   if (datattl == "")
   {  
     Serial.println(datafinal);
     delay(500);
     datattl = Serial.read();
   }
   else
   {
    statusup = "1";
   }
 }

datattl = "";
statusup = "";
Serial.println("simulasi sleep");
delay (5000);


  
  // Serial.println("Begin Lora");
  // Serial.print("air_temp: ");
  // Serial.println(air_temp);
  // Serial.print("air_hum: ");
  // Serial.println(air_hum);
  // Serial.print("Preassure: ");
  // Serial.println(preassure);
  // Serial.print("Wind Speed: ");
  // Serial.println(windspeed);
  // Serial.print("Wind Direction: ");
  // Serial.println(winddirection);
  // Serial.print("Soil_Temp: ");
  // Serial.println(soil_temp);
  // Serial.print("Soil_Hum: ");
  // Serial.println(soil_hum);
  // Serial.print("Soil1: ");
  // Serial.println(soil1);
  // Serial.print("Soil2: ");
  // Serial.println(soil2);
  // Serial.print("GWL: ");
  // Serial.println(gwl);
  // Serial.print("LUX: ");
  // Serial.println(lux);
  // Serial.println("End lora");
  // Serial.println("");
  // delay(500);
}
