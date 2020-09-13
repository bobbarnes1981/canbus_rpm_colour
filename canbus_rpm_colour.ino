#include <can.h>
#include <mcp2515.h>

// TODO: define debug / sprintf
      
struct can_frame canMsg;
MCP2515 mcp2515(10);
unsigned long messageHeartbeat = 0;
#define NO_MESSAGE_HEARTBEAT 1000

word rpm = 0x0000;

void setup() {
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_16MHZ);
  mcp2515.setNormalMode();

  Serial.println("started");
}

void loop() {
  MCP2515::ERROR error = mcp2515.readMessage(&canMsg);
  switch(error) {
    case MCP2515::ERROR_OK:
      // TODO: filter/mask
      // 0x201 Fiesta ST150 message containg RPM byte 0 and 1
      if (canMsg.can_id == 0x201) {
        rpm = word(canMsg.data[0], canMsg.data[1]) / 4;
      }
      Serial.print(rpm);
      Serial.println();
      // TODO: configure RGB led and set colour
      messageHeartbeat = millis();
      break;
    case MCP2515::ERROR_NOMSG:
      // ignore
      break;
    default:
      Serial.print("can error: ");
      Serial.println(error);
      break;
  }
  if (millis() - messageHeartbeat > NO_MESSAGE_HEARTBEAT) {
     messageHeartbeat = millis();
     Serial.println("no messages");
  }
}
