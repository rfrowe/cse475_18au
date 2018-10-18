#include "Creature.h"

#include <cmath>

inline float get_battery_voltage() {
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature(uint8_t kit_num) {
  pinMode(ID_PIN, INPUT);
  _kit_num = kit_num;
  _addr = 2 * kit_num  - digitalRead(ID_PIN);

  _last_startle = millis();
  _last_loop = millis();

  // Set WAIT as initial state
  _next = new Wait();
}

uint8_t _buf[RH_RF69_MAX_MESSAGE_LEN], _len;
void Creature::loop() {
  uint32_t this_loop = millis();
  uint32_t dt = this_loop - _last_loop;

  // Only trigger state loops and transitions every CYCLE_TIME ms
  if (dt > GLOBALS.CYCLE_TIME) {
    if (_next != NULL) {
      // We have a predefined next state, transition immediately
      _state = _next;
      _num_repeats = _state.num_repeats();
    } else if (_num_repeats > 0) {
      // This state should be repeated more. Call loop and decrement
      _state.loop(dt);
      _num_repeats--;
    } else {
      // We're out of repeats, do a transition
      _state = _state.transition();
      _num_repeats = _state.num_repeats();
    }

    _last_loop = this_loop;
  }

  // Poll radio for packets
  if (rf69.waitAvailableTimeout(100)) {
    // Should be a message available
    if (!rf69.recv(_buf, &_len)) {
      Serial.println("Failed to receive packet")
    }

    // We got a message, decode it
    uint8_t pid, src_addr, dst_addr;
    pid = _buf[0];
    src_addr = _buf[1];
    dst_addr = _buf[2];

    // If it's meant for us, pass receipt along to
    if (src_addr != _addr && (dst_addr == _addr || dst_addr == BROADCAST_ADDR)) {
      if (!rx(pid, src_addr, _len - 3, &_buf[3])) {
        Serial.println("Failed to process packet");
        Serial.print("pid: ");
        Serial.println(pid);
        Serial.print("src_addr: ");
        Serial.println(src_addr);
        Serial.print("dst_addr: ");
        Serial.println(dst_addr);
        Serial.print("Payload: ");
        for (uint8_t i = 0; i < _len - 3; i++) {
          Serial.print(_buf[i], HEX);
        }
        Serial.println();
      }
    }
  }
}

bool tx(uint8_y pid, uint8_t dst_addr, uint8_t len, uint8_t*  payload) {
  if (len + 3 > RH_RF69_MAX_MESSAGE_LEN) {
    Serial.print("Cannot broadcast packet of length ");
    Serial.println(len);
    return false;
  }

  Serial.print("Sending packet ");
  Serial.print(pid);
  Serial.print(" to address ");
  Serial.println(dst_addr);

  memcpy(_buf + 3, payload, len);
  rf69.send(&buf, len + 3);
  rf69.waitPacketSent();
  return true;
}

void Creature::setup() {
  Serial.print(F("Booting kit: "));
  Serial.println(_kit_num);
  Serial.print(F("Address: "));
  Serial.println(_addr);

  delay(100);
  pinMode(LED_PIN, OUTPUT);

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

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  // Manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init() || !rf69.setFrequency(RFM69_FREQ)) {
    Serial.println(F("RFM69 radio init failed"));
    while (1);
  }
  rf69.setTxPower(14, true);

  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  rf69.setEncryptionKey(key);

  Serial.print(F("RFM69 radio @ "));
  Serial.print(RFM69_FREQ);
  Serial.println(F("MHz"));
}
