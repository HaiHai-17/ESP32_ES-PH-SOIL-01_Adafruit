#include <WiFi.h>
#include "AdafruitIO_WiFi.h"
#include <ModbusMaster.h>

// ======= CẤU HÌNH CẢM BIẾN PH =======
ModbusMaster node;
#define RXD2 16
#define TXD2 17
float phValue;

// ======= CẤU HÌNH WIFI & ADAFRUIT IO =======
#define WIFI_SSID       "ssid"
#define WIFI_PASS       "password"

#define AIO_USERNAME    "username"
#define AIO_KEY         "user api"

AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);

// ======= KHAI BÁO FEED =======
AdafruitIO_Feed *doAmDat     = io.feed("do-am-dat");
AdafruitIO_Feed *doPhDat     = io.feed("do-ph-dat");
AdafruitIO_Feed *relayFeed   = io.feed("relay");
AdafruitIO_Feed *autoFeed    = io.feed("auto");
AdafruitIO_Feed *autoStatus  = io.feed("auto-status");

// ======= KHAI BÁO RELAY & CẢM BIẾN ĐỘ ẨM =======
#define RELAY_PIN 27
#define SENSOR 35

// ======= BIẾN THỜI GIAN & TRẠNG THÁI =======
unsigned long thoiGianTruoc = 0;
const unsigned long chuKyGui = 30000; // 30 giây
bool cheDoAuto = true;
bool relayDangBat = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2); // UART2 giao tiếp với Sensor pH
  node.begin(1, Serial2); // Địa chỉ Modbus = 1

  // ======= KẾT NỐI WIFI =======
  Serial.println("Đang kết nối WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int dem = 0;
  while (WiFi.status() != WL_CONNECTED && dem++ < 20) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Đã kết nối WiFi!");
  } else {
    Serial.println("\n❌ Không thể kết nối WiFi!");
    while (true);
  }

  // ======= KẾT NỐI ADAFRUIT IO =======
  Serial.println("Đang kết nối Adafruit IO...");
  io.connect();
  dem = 0;
  while (io.status() < AIO_CONNECTED && dem++ < 20) {
    delay(500);
    Serial.print(".");
  }
  if (io.status() == AIO_CONNECTED) {
    Serial.println("\n✅ Đã kết nối Adafruit IO!");
  } else {
    Serial.println("\n❌ Không thể kết nối Adafruit IO!");
    while (true);
  }
  Serial.println("Đang đọc giá trị pH...");

  // ======= CẤU HÌNH RELAY =======
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  relayDangBat = false;

  // ======= ĐĂNG KÝ HÀM XỬ LÝ TỪ SERVER =======
  autoFeed->onMessage(handleAutoMessage);
  relayFeed->onMessage(handleRelayMessage);
  doAmDat->onMessage(handleMessage);
  doPhDat->onMessage(handleMessage);
}

  // ======= NHẬN DỮ LIỆU TỪ SERVER =======
void handleMessage(AdafruitIO_Data *data) {
  Serial.print("Nhận dữ liệu: ");
  Serial.println(data->value());
}

// ======= XỬ LÝ NHẤN BUTTON AUTO (dạng toggle ngược trạng thái) =======
void handleAutoMessage(AdafruitIO_Data *data) {
  cheDoAuto = !cheDoAuto;
  Serial.print("✅ Chuyển chế độ: ");
  Serial.println(cheDoAuto ? "TỰ ĐỘNG" : "THỦ CÔNG");
  autoStatus->save(cheDoAuto ? "AUTO" : "MANUAL");
}

// ======= XỬ LÝ ĐIỀU KHIỂN RELAY KHI THỦ CÔNG =======
void handleRelayMessage(AdafruitIO_Data *data) {
  if (!cheDoAuto) {
    String lenh = data->value();
    if (lenh == "ON") {
      digitalWrite(RELAY_PIN, HIGH);
      relayDangBat = true;
      Serial.println("✅ Relay BẬT (THỦ CÔNG)");
    } else {
      digitalWrite(RELAY_PIN, LOW);
      relayDangBat = false;
      Serial.println("✅ Relay TẮT (THỦ CÔNG)");
    }
  } else {
    Serial.println("⚠️ Bỏ qua lệnh thủ công vì đang ở chế độ TỰ ĐỘNG");
  }
}

  // ======= ĐỌC THANH GHI TỪ CẢM BIẾN PH =======
void readSensorpH(){
  uint8_t result = node.readHoldingRegisters(0x0000, 1); // Đọc 1 thanh ghi tại địa chỉ 0x0000

  if (result == node.ku8MBSuccess) {
    phValue = node.getResponseBuffer(0) / 10.0;   // Chia cho 10 để ra số thập phân
    Serial.print("Giá trị pH: ");
    Serial.println(phValue, 2);
  } else {
    Serial.print("Lỗi đọc cảm biến. Mã lỗi: ");
    Serial.println(result, HEX);
  }
}

  // ======= ĐỌC ANALOG TỪ CẢM BIẾN ĐỘ ẨM =======
int readDoAmDat() {
  int doAmRaw = analogRead(SENSOR);             // Đọc giá trị analog
  int doAmPercent = map(doAmRaw, 0, 4095, 0, 100);  // Chuyển về % (giá trị từ 0 -> 4095 tương ứng 0 -> 100%)
  return doAmPercent;
}


void loop() {
  io.run();

  unsigned long thoiGianHienTai = millis();

  if (thoiGianHienTai - thoiGianTruoc >= chuKyGui) {
    thoiGianTruoc = thoiGianHienTai;

    readSensorpH();             // Gọi hàm đọc độ ph

    int doAm = readDoAmDat();  // Gọi hàm đọc độ ẩm đất

    Serial.print("🌱 Độ ẩm: ");
    Serial.print(doAm);
    Serial.print(" | pH: ");
    Serial.println(phValue);

    // ======= ĐIỀU KHIỂN RELAY TỰ ĐỘNG =======
    if (cheDoAuto) {
      if (doAm < 50 && !relayDangBat) {
        digitalWrite(RELAY_PIN, HIGH);
        relayDangBat = true;
        Serial.println("✅ Relay BẬT (AUTO: độ ẩm < 50)");
        relayFeed->save("ON");
      } else if (doAm > 70 && relayDangBat) {
        digitalWrite(RELAY_PIN, LOW);
        relayDangBat = false;
        Serial.println("✅ Relay TẮT (AUTO: độ ẩm > 70)");
        relayFeed->save("OFF");
      } else {
        Serial.println("ℹ️ Giữ nguyên trạng thái relay");
      }
    } else {
      Serial.println("⚙️ Chế độ THỦ CÔNG ➝ không can thiệp relay tự động");
    }

    // ======= GỬI DỮ LIỆU CẢM BIẾN =======
    doAmDat->save(doAm);
    doPhDat->save(phValue);
  }
}
