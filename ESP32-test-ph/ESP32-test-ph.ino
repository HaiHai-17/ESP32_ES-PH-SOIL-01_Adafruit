#include <ModbusMaster.h>

ModbusMaster node;

#define RXD2 16
#define TXD2 17
#define RE 18
#define DE 19  

void preTransmission() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delayMicroseconds(10);
}

void postTransmission() {
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);
  delayMicroseconds(10);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2); // UART2 giao tiếp với CP2102

  pinMode(DE, OUTPUT);
  pinMode(RE, OUTPUT);
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  node.begin(1, Serial2); // Địa chỉ Modbus = 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

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
