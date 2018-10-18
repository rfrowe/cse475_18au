#include "Creature.h"

#include <cmath>

inline float get_battery_voltage() {
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature(uint8_t kit_num) {
  pinMode(ID_PIN, INPUT);
  _kit_num = kit_num;
  _addr = 2 * kit_num  - digitalRead(ID_PIN);
}

void Creature::setup() {
  Serial.print(F("Booting kit: "));
  Serial.println(_kit_num);
  Serial.print(F("Address: "));
  Serial.println(_addr);

  delay(100);
  pinMode(LED_PIN, OUTPUT);
  

  _oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  _oled.display();
  _oled.clearDisplay();
  _oled.init();
  _oled.setBatteryVisible(true);
  _oled.setBattery(get_battery_voltage());
  _oled.renderBattery();
  _oled.display();

  _strip.begin();
  _strip.setBrightness(5);
  _strip.show();

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  // Manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!_rf69.init() || !_rf69.setFrequency(RFM69_FREQ)) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  _rf69.setTxPower(14, true);

  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  _rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @ ");
  Serial.print(RFM69_FREQ);
  Serial.println("MHz");
}

