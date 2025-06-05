# 🌐 ESP32 Read Value pH, Humidity form sensor ES-PH-SOIL-01, sensor Humnidity LM393 and Control Relay 

MDự án này sử dụng **ESP32** đọc dữ liệu từ cảm biến pH đất, cảm biến độ ẩm đất. Được điều khiển và hiển thị biểu đồ trên Adafruit.

---

## 📸 Giao diện chính (Main Interface)

| Trang chính                                                                               |
| ----------------------------------------------------------------------------------------- |
|![thuc-nghiem](https://github.com/user-attachments/assets/e6b6e924-e1a6-45c0-bdd6-e33604156f36)| 

---

## 🛠 Công nghệ sử dụng (Technologies Used)

* **ESP32**: Bộ vi điều khiển hỗ trợ WiFi, Bluetooth
* **Sensor ES-PH-SOIL-01**: Bộ cảm biến 5-24V dùng để đọc giá trị pH qua giao thức ModbusTCP (RS485)
* **Sensor Humnidity LM393**: Bộ cảm biến đo độ ẩm đất đọc giá trị analog và so sánh qua IC LM393
* **Relay**: Relay điều khiển thiết bị đầu vào 5Vdc, đầu ra 12Vdc-220Vac
* **MQTT Adafruit**: Sử dụng cloud Adafuit trực quan hoá dữ liệu lên biểu đồ, điều khiển nút bấm Relay qua giao thức MQTT
  
---

## 📂 Cách sử dụng (How to Use)

1. Điều chỉnh SSID, Password theo Wifi của bạn
2. Tạo tài khoản Adafuit và lấy dữ liệu: username, key api AIO
3. Cài thư viện ModbusTCP trên Arduino IDE
4. nạp chương trình vào ESP32 và kiểm tra
