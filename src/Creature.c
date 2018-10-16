#include "Creature.h"

#include <cmath>

inline float get_battery_voltage() {
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature(uint8_t kit_num) {
  _kit_num = kit_num;
  _addr = pow(2, kit_num) - digitalRead(ID_PIN);

  delay(100);
  pinMode(LED_PIN, OUTPUT);
  ponMode(ID_PIN, INPUT);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setBatteryVisible(true);
  oled.setBattery(get_battery_voltage());
  oled.renderBattery();
  oled.display();

  strip.begin();
  strip.setBrightness(5);
  strip.show();

  Serial.begin(115200);

  pinMode(RFM69_RRST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  // Manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init() || !rf69.setFrequency(RF69_FREQ)) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  rf69.setTxPower(14, true);

  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  rf69.setEncryptionKey(key);

  Serial.println("RFM69 radio @ %d MHz", (int) RF69_FREQ);
}
