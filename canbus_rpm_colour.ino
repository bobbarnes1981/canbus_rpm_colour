#include <can.h>
#include <mcp2515.h>

#define DEBUG

//#define CYCLE

#ifdef DEBUG
HardwareSerial &s = Serial;
#endif

#define pin_r 3
#define pin_g 9
#define pin_b 6
#define pin_can 10

struct can_frame canMsg;
MCP2515 mcp2515(pin_can);
unsigned long messageHeartbeat = 0;
#define NO_MESSAGE_HEARTBEAT 1000

#define STEPS 14

word rpms[] = {
  6500,
  6000,
  5500,
  5000,
  4500,
  4000,
  3500,
  3000,
  2500,
  2000,
  1500,
  1000,
  500,
  0
};

long colours[] = {
  0xFF0000, // red
  0xFF2700,
  0xFF4E00,
  0xFF7500,
  0xFF9C00,
  0xFFC400,
  0xFFEB00,
  0xEBFF00,
  0xC4FF00,
  0x9CFF00,
  0x75FF00,
  0x4EFF00,
  0x27FF00,
  0x00FF00  // green
};

word rpm = 0x0000;

void setup() {  
  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_16MHZ);
  mcp2515.setNormalMode();

  #ifdef DEBUG
  s.begin(115200);
  s.println("started");
  #endif

  rgbColour(0xFF0000);
  delay(2000);
  rgbColour(0x00FF00);
  delay(2000);
  rgbColour(0x0000FF);
  delay(2000);
}

void loop() {
  #ifdef CYCLE
  
  rpm+=500;
  if (rpm > 7000) {
    rpm = 0;
  }
  #ifdef DEBUG
  s.println(rpm);
  #endif
  setColour();
  delay(1000);

  #else
  
  MCP2515::ERROR error = mcp2515.readMessage(&canMsg);
  switch(error) {
    case MCP2515::ERROR_OK:
      messageHeartbeat = millis();
      // TODO: filter/mask
      // 0x201 Fiesta ST150 message containg RPM byte 0 and 1
      if (canMsg.can_id == 0x201) {
        rpm = word(canMsg.data[0], canMsg.data[1]) / 4;
      }
      setColour();
      #ifdef DEBUG
      s.print("rpm: ");
      s.println(rpm);
      #endif
      break;
    case MCP2515::ERROR_NOMSG:
      // ignore
      break;
    default:
      #ifdef DEBUG
      s.print("can error: ");
      s.println(error);
      #endif
      break;
  }
  if (millis() - messageHeartbeat > NO_MESSAGE_HEARTBEAT) {
     messageHeartbeat = millis();
     // TODO: flash LED?
     #ifdef DEBUG
     s.println("no messages");
     #endif
  }

  #endif
}

void setColour() {
  for (int i = 0; i < STEPS; i++) {
    if (rpm >= rpms[i]) {
      rgbColour(colours[i]);
      #ifdef DEBUG
      s.print("colour: ");
      s.println(colours[i], HEX);
      #endif
      break;
    }
  }
}

void rgbColour(long colour) {
  int r = (colour >> 16) & 0xFF;
  int g = (colour >> 8) & 0xFF;
  int b = (colour >> 0) & 0xFF;
  analogWrite(pin_r, r);
  analogWrite(pin_g, g);
  analogWrite(pin_b, b);
}
