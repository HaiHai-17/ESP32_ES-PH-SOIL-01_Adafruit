#include <ModbusMaster.h>

ModbusMaster node;

#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2); // UART2 giao tiếp với CP2102

  node.begin(1, Serial2); // Địa chỉ Modbus = 1

  Serial.println("Đang đọc giá trị pH...");
}

void loop() {
  uint8_t result = node.readHoldingRegisters(0x0000, 1); // Đọc 1 thanh ghi tại địa chỉ 0x0000

  if (result == node.ku8MBSuccess) {
    float phValue = node.getResponseBuffer(0) / 10.0;
    Serial.print("Giá trị pH: ");
    Serial.println(phValue, 2);
  } else {
    Serial.print("Lỗi đọc cảm biến. Mã lỗi: ");
    Serial.println(result, HEX);
  }

  delay(2000);
}
