#include <mcp_can.h> //Include MCP2515 CAN Library
#include <SPI.h> //Include SPI Library
int kph; // Speed 
int rpm; // RPM 
int gear; // Gear 
int volts; // Battery Voltage
int bias = 513; // Brake Bias
int errorCode; // Error Code: 1 = Configuration Mode Fail, 2 = Baudrate Fail, 3 = MCP2515 Initialisation Fail, 4 = ECU Disconnected, 5 = ECU Connected
float time1 = 0; // Time ECU Connected
float time2 = 0; // Time ECU Disconnected
long unsigned int rxId; // Data Packet ID
unsigned char len = 0; // Data Packet Length
unsigned char rxBuf[8]; // Data Packet Byte(s)
char message[128]; // Serial String Array
#define CAN0_INT 2 // INT to Pin 2
MCP_CAN CAN0(10); // CS to Pin 3

void setup() {
  Serial.begin(115200); // Serial at 115200 baud
  time1 = millis();  // Initialisation Time
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) { // Initialize MCP2515 at 8MHz and Baudrate 500kB/s
    errorCode = 3; // Set Error Code to 3
    sprintf(message,"0,0,0,0,0,%1i",errorCode); // Send Error Code 3
    Serial.println(message); 
  }
  CAN0.setMode(MCP_NORMAL); // Normal MCP2515 Operation Mode
  pinMode(CAN0_INT, INPUT); // Configuring INT Input
  }

void loop()
{
  if(!digitalRead(CAN0_INT)) { // Read Receive Buffer if INT is LOW
    CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read Incoming Data
    errorCode = 5; // Set Error Code to 5
    if((rxId & 0x1FFFFFFF)==8192) { // If ID = 0x2000
      rpm = rxBuf[0]; // Data 1 = RPM
    } else if((rxId & 0x1FFFFFFF)==8193) { // If ID = 0x2001
      kph = rxBuf[4]; // Data 3 = KPH x10
    } else if((rxId & 0x1FFFFFFF)==8194) { // If ID = 0x2001
      volts = rxBuf[4]; // Data 3 = Volts x10
    }
    sprintf(message,"%1i,%2i,%3i,%4i,%5i,%6i",kph,rpm,gear,volts,bias,errorCode);
    Serial.println(message);
    time1 = millis(); 
  } 
  time2 = millis();
  if((time2-time1)>100) { // ECU Disconnected for > 100ms
    errorCode = 4; // Set Error Code to 4
    sprintf(message,"0,0,0,0,0,%1i",errorCode); // Send Error Code 4
    Serial.println(message);
    delay(500); // Delay Error Code Messages
  }
}
