#include "Creature.h"
#include "Debug.h"
#include "State.h"
#include "Controller.h"

#include <cmath>

// TODO: put your kit number here
#define KIT_NUM 0

// Returns current battery voltage
inline float getBatteryVoltage() {
  // Update battery field with a little weighting to stop rapid changes.
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature() {
  // Initialize _next to be the Controller state, so we will immediately transition into it on the first loop.
  _next = new Controller(*this);
  _state = nullptr;

  pinMode(ID_PIN, INPUT_PULLUP);
  _kitNum = 0;
  _addr = 0;

  // Parens zero initialize
  _creatureDistances = new int8_t[GLOBALS.NUM_CREATURES + 1]();
  _creatureStates = new uint8_t[GLOBALS.NUM_CREATURES + 1]();

  
  _battery = getBatteryVoltage();

  _lastStartle = millis();
  _lastLoop = millis();
}

int Creature::_get_int() {
  String modeStr = Serial.available() ? Serial.readStringUntil(' ') : "00";
  char modeCharStr[modeStr.length()];
  modeStr.toCharArray(modeCharStr, modeStr.length() + 1);
  
  int i1 = (int) strtol(modeCharStr, NULL, 16);
  int i2 = (int) strtol(modeCharStr, NULL, 0);

  return i1 > i2 ? i1 : i2;
}

void Creature::_processCommand() {
  if (Serial.available()) {
    String str = Serial.readStringUntil(' ');
    str.toLowerCase();
    if (str.equals("stop")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          bool res = tx(1, dst, 0, nullptr);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print("Stopping creature #");
          Serial.println(dst);
          Serial.println();
        }
      }
    } else if (str.equals("start")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint16_t mode = (uint16_t) _get_int();
          uint8_t pld[2];
          memcpy(pld, &mode, sizeof(uint16_t));
          bool res = tx(2, dst, 2, pld);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print("Starting creature #");
          Serial.print(dst);
          Serial.print(" with mode 0x");
          Serial.println(mode, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("sound")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint8_t sound = (uint8_t) _get_int();
          bool res = tx(3, dst, 1, &sound);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print("Teling creature #");
          Serial.print(dst);
          Serial.print(" to play sound 0x");
          Serial.println(sound, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("effect")) {
      if (Serial.available()) {
       uint8_t dst = (uint8_t) Serial.parseInt();
        if (dst > 0) {
          Serial.readStringUntil(' ');
          uint8_t effect = (uint8_t) _get_int();

          bool res = tx(3, dst, 1, &effect);

          if (!res) {
            Serial.print("ERROR ");
          }
          Serial.print("Teling creature #");
          Serial.print(dst);
          Serial.print(" to play effect 0x");
          Serial.println(effect, HEX);
          Serial.println();
        }
      }
    } else if (str.equals("set")) {
      if (Serial.available()) {
        uint8_t globalIndex = (uint8_t) Serial.parseInt();
        if (globalIndex >= 0 && globalIndex < GLOBALS_LEN) {
          Serial.readStringUntil(' ');

          if (!Serial.available()) {
            Serial.print("Global index #");
            Serial.print(globalIndex);
            Serial.print(" == ");
            Serial.println(GLOBALS_NAMES[globalIndex]);
            
          } else {
            Serial.print("Adjusting global ");
            Serial.print(GLOBALS_NAMES[globalIndex]);
            Serial.print(" from ");
            void* ptr = (((char*)&GLOBALS) + GLOBALS_OFFSETS[globalIndex]);
          
            if (globalIndex == 7) {
              Serial.print(*(float*) ptr);
              Serial.print(" to ");
              float val = Serial.parseFloat();
              *(float*) ptr = val;
              Serial.println(*(float*) ptr);
            } else if (globalIndex == 8 || globalIndex == 0) {
              Serial.print(*(uint16_t*) ptr);
              Serial.print(" to ");
              uint16_t val = (uint16_t) Serial.parseInt();
              *(uint16_t*) ptr = val;
              Serial.println(*(uint16_t*) ptr);
            } else {
              Serial.print(*(uint8_t*) ptr);
              Serial.print(" to ");
              uint8_t val = (uint8_t) Serial.parseInt();
              *(uint8_t*) ptr = val;
              Serial.println(*(uint8_t*) ptr);
            }

            // TODO: BROADCAST CHANGES
          }
          Serial.println();
        } else {
          Serial.print("ERROR, globals index ");
          Serial.print(globalIndex);
          Serial.print(" is greater than max index ");
          Serial.println(GLOBALS_LEN - 1);
          Serial.println();
        }
      }
    } else if (str.equals("broadcast")) {
      Serial.print("Broadcasting globals");
      // TODO: Sent broadcast packet
    } else if (str.equals("packet")) {
      if (Serial.available()) {
        uint8_t dst = (uint8_t) Serial.parseInt();
        Serial.readStringUntil(' ');
        Serial.println(RH_RF69_MAX_MESSAGE_LEN);
        uint8_t pid = _get_int();
        
        uint8_t pld[RH_RF69_MAX_MESSAGE_LEN];
        uint8_t size = 0;
        while (Serial.available() && size < RH_RF69_MAX_MESSAGE_LEN) {
          pld[size] = _get_int();
          size++;
        }

        bool res = tx(pid, dst, size, pld);
      }
    } else {
      Serial.println("ERROR: INVALID COMMAND");
      Serial.println(str);
    }
    Serial.readString();
  }
}

void Creature::loop() {
  uint32_t thisLoop = millis();
  uint32_t dt = thisLoop - _lastLoop;

  _pollRadio();

  _processCommand();

  // Only trigger state loops and transitions every CYCLE_TIME ms
  if (dt > GLOBALS.CYCLE_TIME) {
    _updateDisplay();

    if (_next != NULL) {
      // We have a predefined next state, transition immediately
      _transition(_next);
      _next = nullptr;
    } else if (_remainingRepeats > 0) {
      // This state should be repeated more. Call loop and decrement
      _state->loop(dt);
      _remainingRepeats--;
    } else {
      // We're out of repeats, do a transition
      _transition(_state->transition());
      _state->loop(dt);
    }

    _lastLoop = thisLoop;
  }
}

bool Creature::_rx(uint8_t pid, uint8_t srcAddr, uint8_t len, uint8_t* payload, int8_t rssi) {
  _rxCount++;
  _updateDistance(srcAddr, rssi);

  switch (pid) {
    case PID_SET_GLOBALS:
      return true;
    case PID_STOP:
      return true;
    case PID_START:
      return true;
    case PID_PLAY_SOUND:
      return true;
    case PID_PLAY_EFFECT:
      return true;
    case PID_BROADCAST_STATES:
      return true;
    case PID_STARTLE:
      return true;
    case PID_SEND_STATE:
      _receiveState(srcAddr, payload[0]);
      return true;
    default:
      Serial.print(F("Received packet of unknown type: "));
      Serial.print(pid, HEX);
      return false;
  }
}

void Creature::_updateDistance(uint8_t addr, int8_t rssi) {}

bool Creature::_rxSetGlobals(uint8_t len, uint8_t* payload) {
  return true;
}

void Creature::_rxStop() {}

bool Creature::_rxStart(uint8_t len, uint8_t* payload) {
  return true;
}

bool Creature::_rxBroadcastStates(uint8_t len, uint8_t* payload) {
  // TODO: implement
  return true;
}

bool Creature::tx(const uint8_t pid, const uint8_t dst_addr, const uint8_t len, uint8_t* const payload) {
  if (len + 3 > RH_RF69_MAX_MESSAGE_LEN) {
    Serial.print(F("Packet length "));
    Serial.print(len);
    Serial.print(F(" bytes is longer than max of "));
    Serial.print(RH_RF69_MAX_MESSAGE_LEN - 3);
    Serial.println(F(" bytes"));
    return false;
  }

  _buf[0] = pid;
  _buf[1] = _addr;
  _buf[2] = dst_addr;
  memcpy(_buf + 3, payload, len);

  Serial.print(F("Sending pid 0x"));
  Serial.print(pid, HEX);
  Serial.print(F(" with "));
  Serial.print(len);
  Serial.print(F(" bytes to address "));
  Serial.print(dst_addr);
  Serial.print(F(" from address "));
  Serial.println(_addr);

  for (uint8_t* i = &_buf[3]; i < &_buf[len + 3]; i++) {
    dprint(*i, HEX);
    dprint(' ');
  }
  dprintln();

  if (_buf[len + 2] == 0) {
    // NULL terminated, print as string too.
    dprintln((char*) &_buf[3]);
  }
  dprintln();

  _rf69.send((const uint8_t*) &_buf, len + 3);
  _rf69.waitPacketSent();
  _txCount++;
  return true;
}

void Creature::_txSendState(uint8_t oldState, uint8_t newState) {
  uint8_t payload[] = {oldState, newState};
  tx(PID_SEND_STATE, CONTROLLER_ADDR, sizeof(payload), payload);
}

void Creature::_receiveState(uint8_t src, uint8_t st) {
  _creatureStates[src] = st;
}

void Creature::_pollRadio() {
  // Poll radio for packets
  if (_rf69.waitAvailableTimeout(100)) {
    uint8_t len = sizeof(_buf);

    // Should be a message available
    if (!_rf69.recv(_buf, &len)) {
      Serial.println("Failed to receive packet");
      return;
    }

    uint8_t pid, srcAddr, dstAddr;
    pid = _buf[0];
    srcAddr = _buf[1];
    dstAddr = _buf[2];

    Serial.print(F("Received pid 0x"));
    Serial.print(pid, HEX);
    Serial.print(F(" with "));
    Serial.print(len - 3);
    Serial.print(F(" bytes from address "));
    Serial.print(srcAddr);
    Serial.print(F(" for address "));
    Serial.println(dstAddr);

    for (uint8_t* i = &_buf[3]; i < &_buf[len]; i++) {
      dprint(*i, HEX);
      dprint(' ');
    }
    dprintln();
    if (_buf[len - 1] == 0) {
      dprintln((char*) &_buf[3]);
    }

    Serial.println();
    Serial.println();

    if (dstAddr == _addr || dstAddr == BROADCAST_ADDR) {
      if (!_rx(pid, srcAddr, len - 3, &_buf[3], _rf69.lastRssi())) {
        Serial.println(F("Packet not handled correctly. Payload dump:"));
        for (uint8_t* i = &_buf[3]; i < &_buf[len]; i++) {
          Serial.print(*i, HEX);
          Serial.print(' ');
        }
      }
    }
  }
}

void Creature::_transition(State* const state) {
  State* const old = _state;
  _state = state;

  if (state != old) {
    if (old != nullptr) {
      delete old;
      _txSendState(old->getId(), state->getId());
    } else {
      _txSendState(0, state->getId());
    }
  }

  _remainingRepeats = _state->getNumRepeats();
}

void Creature::_updateDisplay() {
  _battery = 0.95 * _battery + 0.05 * getBatteryVoltage();

  oled.clearDisplay();
  oled.setBattery(_battery);
  oled.renderBattery();

  oled.setCursor(0, 0);
  oled.print(F("TXRX:"));
  oled.print(_txCount);
  oled.print(F("/"));
  oled.println(_rxCount);

  oled.setCursor(0, 9);
  oled.print(_state ? _state->getName() : "None");

  oled.setCursor((OLED_WIDTH - 3) * 6, 9);
  oled.print("#");
  oled.print(_addr);

  oled.display();
}

void Creature::setup() {
  Serial.print(F("Booting kit: "));
  Serial.println(KIT_NUM);
  Serial.print(F("Address: "));
  Serial.println(_addr);

  delay(100);
  pinMode(LED_PIN, OUTPUT);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setBatteryVisible(true);
  _updateDisplay();

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

  if (!_rf69.init() || !_rf69.setFrequency(RFM69_FREQ)) {
    Serial.println(F("RFM69 radio init failed"));
    while (1);
  }
  _rf69.setTxPower(GLOBALS.TX_POWER, true);

  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  _rf69.setEncryptionKey(key);

  _creatureStates = new uint8_t[GLOBALS.NUM_CREATURES + 1]();

  Serial.print(F("RFM69 radio @ "));
  Serial.print(RFM69_FREQ);
  Serial.println(F("MHz"));
  _creatureStates[25] = 10;
}

Creature::~Creature() {
  delete _creatureDistances;
  delete _creatureStates;
}
