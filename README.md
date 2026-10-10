# Smart Attendance System using ESP32 & RFID

An IoT-based Smart Attendance Management System powered by an **ESP32 Development Board**, an **RC522 RFID Module**, and a **Buzzer** for audio-visual feedback. 

This project reads unique RFID keycards/tags via SPI communication, validates user IDs in real-time, and provides immediate audio cues, making it an ideal starter project for smart access control and attendance logging.

---

## 📸 Features

* **Real-time RFID Authentication:** Instant card scanning and UID detection using the MFRC522 SPI interface.
* **Audio Feedback:** Active/passive buzzer integration to confirm successful scans or alert on unrecognized tags.
* **Compact Breadboard Layout:** Neat wiring setup connecting peripherals directly to the ESP32 development board.

---

## 🛠 Hardware Required

| Component | Quantity | Description |
| :--- | :--- | :--- |
| **ESP32 Development Board** | 1 | Wi-Fi & Bluetooth microcontroller board |
| **RC522 RFID Module** | 1 | 13.56MHz RFID Reader/Writer SPI module |
| **Buzzer** | 1 | Audio indicator for scan verification |
| **Breadboard & Wires** | - | Jumper cables for pin interconnections |

---

## 🔌 Wiring & Pin Mapping

### 1. RC522 RFID Module to ESP32 (SPI Connection)
> **Note:** The RC522 module works on **3.3V logic**. Ensure VCC is connected to the 3.3V rail.

| RC522 Pin | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **VCC** | **3.3V** | Power Supply (3.3V only) |
| **RST** | **GPIO 22** | Reset Pin |
| **GND** | **GND** | Ground |
| **MISO** | **GPIO 19** | Master In Slave Out (SPI) |
| **MOSI** | **GPIO 23** | Master Out Slave In (SPI) |
| **SCK** | **GPIO 18** | Serial Clock (SPI) |
| **SS / SDA** | **GPIO 5** | Slave Select / Chip Select |

### 2. Buzzer to ESP32

| Buzzer Pin | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **Positive (+)** | **GPIO 15** | Digital Control Pin |
| **Negative (-)** | **GND** | Ground Rail |

---

## 📚 Required Libraries

Install the following library via the **Arduino Library Manager** (*Sketch > Include Library > Manage Libraries*):

1. **`MFRC522`** by GithubCommunity / Seeed Studio

---

## 🚀 How to Run

1. Connect your **ESP32**, **RC522 module**, and **Buzzer** to the breadboard according to the pin mappings above.
2. Open **Arduino IDE**, install the **MFRC522** library, and select your **ESP32 Dev Module** board.
3. Update the **authorizedUID** variable in the script with your specific RFID card/tag UID.
4. Upload the code and open the **Serial Monitor (115200 baud)** to view real-time card scans and attendance feedback.

---

## 📜 License
This project is open-source under the **MIT License**.
