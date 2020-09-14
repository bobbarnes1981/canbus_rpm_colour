#include <can.h>
#include <mcp2515.h>

#define DEBUG

#ifdef DEBUG
HardwareSerial &s = Serial;
#endif

struct can_frame canMsg;
MCP2515 mcp2515(10);
unsigned long messageHeartbeat = 0;
#define NO_MESSAGE_HEARTBEAT 1000

#define pin_r 11
#define pin_g 10
#define pin_b 9

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
}

void loop() {
  MCP2515::ERROR error = mcp2515.readMessage(&canMsg);
  switch(error) {
    case MCP2515::ERROR_OK:
      messageHeartbeat = millis();
      // TODO: filter/mask
      // 0x201 Fiesta ST150 message containg RPM byte 0 and 1
      if (canMsg.can_id == 0x201) {
        rpm = word(canMsg.data[0], canMsg.data[1]) / 4;
      }
      setColour(rpm);
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
}

void setColour(word rpm) {
  // TODO: fade colour?
  if (rpm < 5000) {
    rgbColour(0, 255, 0); // green
  } else {
    rgbColour(255, 0, 0); // red
  }
}

void rgbColour(int r, int g, int b) {
  analogWrite(pin_r, r);
  analogWrite(pin_g, g);
  analogWrite(pin_b, b);
}
