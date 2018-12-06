#include "Creature.h"
#include "Debug.h"
#include "State.h"
#include "Wait.h"
#include "Active1.h"
#include "Active2.h"
#include "Active3.h"
#include "Ambient1.h"
#include "Ambient2.h"
#include "Ambient3.h"
#include "Startle.h"
#include "Midi.h"
#include "Neopixel.h"

#include <cmath>

// TODO: put your kit number here
#define KIT_NUM 0

#define VERSION "2.3"

// Returns current battery voltage
inline float getBatteryVoltage() {
  // Update battery field with a little weighting to stop rapid changes.
  return analogRead(VBAT_PIN)  * 2 * VREF / 1024;
}

Creature::Creature() {
  // Initialize _next to be the Wait state, so we will immediately transition into it on the first loop.
  _next = getState(0);
  _prev = _state = nullptr;

  pinMode(ID_PIN, INPUT_PULLUP);
  _kitNum = KIT_NUM;
  _addr = 2 * KIT_NUM  - digitalRead(ID_PIN);
  if (_kitNum <= 0) {
    _addr = BROADCAST_ADDR;
  }

  // Parens zero initialize
  _creatureDistances = new int8_t[GLOBALS.NUM_CREATURES + 1]();
  _creatureStates = new uint8_t[GLOBALS.NUM_CREATURES + 1]();

  _battery = getBatteryVoltage();
  srand(_addr * ((int) (_battery * 100)));

  _lastStartle = millis();
  _lastLoop = millis();
}

void Creature::loop() {
  unsigned long thisLoop = millis();
  uint32_t dt = thisLoop - _lastLoop;

  _pollRadio();

  if (_next != NULL) {
    // We have a predefined next state, transition immediately
    _transition(_next);
    _next = nullptr;

    _state->loop(dt);

    _lastLoop = thisLoop;
    _updateDisplay();
  } else if (dt > GLOBALS.CYCLE_TIME) {
    // Only trigger state loops and transitions every CYCLE_TIME ms

    // Some helpful printlns
    /*for (int i = 0; i < GLOBALS.NUM_CREATURES + 1; i++) {
      dprint(_creatureStates[i]);
      dprint("\t");
    }
    dprintln();

    for (int i = 0; i < GLOBALS.NUM_CREATURES + 1; i++) {
      dprint(_creatureDistances[i]);
      dprint("\t");
    }
    dprintln();

    dprint("Threshold: ");
    dprint(_startleThreshold);
    dprint("/");
    dprintln("255");*/

    if (_remainingRepeats > 0) {
      // This state should be repeated more. Call loop and decrement
      _state->loop(dt);
      _remainingRepeats--;
    } else {
      // We're out of repeats, do a transition
      _transition(_state->transition());
      _state->loop(dt);
    }

    _lastLoop = thisLoop;
    _updateDisplay();
  }

  Neopixel::loop();

  // Poll PIR
  bool newPIR = digitalRead(PIR_PIN);
  if (newPIR && !_PIR) {
    // Rising edge trigger
    dprintln(F("PIR triggered"));
    digitalWrite(LED_PIN, HIGH);
    _state->PIR();
    _PIR = newPIR;
  } else if (!newPIR && _PIR) {
    // Falling edge
    dprintln(F("PIR reset"));
    digitalWrite(LED_PIN, LOW);
    _PIR = newPIR;
  }
}

bool Creature::_rx(uint8_t pid, uint8_t srcAddr, uint8_t len, uint8_t* payload, int8_t rssi) {
  _rxCount++;
  _updateDistance(srcAddr, rssi);

  switch (pid) {
    case PID_SET_GLOBALS:
      if (srcAddr != CONTROLLER_ADDR) return false;
      return _rxSetGlobals(len, payload);
    case PID_STOP:
      if (srcAddr != CONTROLLER_ADDR) return false;
      _rxStop();
      return true;
    case PID_START:
      if (srcAddr != CONTROLLER_ADDR) return false;
      return _rxStart(len, payload);
    case PID_PLAY_SOUND:
      return _state->rxPlaySound(len, payload);
    case PID_PLAY_EFFECT:
      return _state->rxPlayEffect(len, payload);
    case PID_BROADCAST_STATES:
      return _rxBroadcastStates(len, payload);
    case PID_STARTLE:
      return _state->rxStartle(rssi, len, payload);
    default:
      Serial.print(F("Received packet of unknown type: "));
      Serial.print(pid, HEX);
      return false;
  }
}

void Creature::_updateDistance(uint8_t addr, int8_t rssi) {
  if (addr <= GLOBALS.NUM_CREATURES) {
    // Update gradually so we don't move too much
    _creatureDistances[addr] = _creatureDistances[addr] * DISTANCE_ALPHA + rssi * (1 - DISTANCE_ALPHA);
  }
}

uint8_t Creature::updateThreshold() {
  uint8_t delta = getStartleThreshold() * (millis() - getLastStartle()) * _state->getStartleFactor() * GLOBALS.STARTLE_THRESHOLD_DECAY;
  if ((int16_t) getStartleThreshold() - (int16_t) delta < 0) {
    setStartleThreshold(0);
  } else {
    setStartleThreshold((uint8_t) round(getStartleThreshold() - delta));
  }
  setLastStartle(millis());
}

bool Creature::_rxSetGlobals(uint8_t len, uint8_t* payload) {
  if (len != sizeof(struct Globals)) {
    Serial.print(F("Received SetGlobals payload of length "));
    Serial.print(len);
    Serial.print(F(" when sizeof(Globals) = "));
    Serial.print(sizeof(struct Globals));
    Serial.println(F(" was expected"));
    return false;
  }

  struct Globals old = GLOBALS;
  memcpy(&GLOBALS, payload, sizeof(struct Globals));

  dprintln(F("Setting globals:"));
  if (old.TX_POWER != GLOBALS.TX_POWER) {
    dprint(F("\tTX_POWER: ")); dprintln(GLOBALS.TX_POWER);
  } if (old.STARTLE_RAND_MIN != GLOBALS.STARTLE_RAND_MIN) {
    dprint(F("\tSTARTLE_RAND_MIN: ")); dprintln(GLOBALS.STARTLE_RAND_MIN);
  } if (old.STARTLE_RAND_MAX != GLOBALS.STARTLE_RAND_MAX) {
    dprint(F("\tSTARTLE_RAND_MAX: ")); dprintln(GLOBALS.STARTLE_RAND_MAX);
  } if (old.STARTLE_MAX != GLOBALS.STARTLE_MAX) {
    dprint(F("\tSTARTLE_MAX: ")); dprintln(GLOBALS.STARTLE_MAX);
  } if (old.STARTLE_THRESHOLD != GLOBALS.STARTLE_THRESHOLD) {
    dprint(F("\tSTARTLE_THRESHOLD: ")); dprintln(GLOBALS.STARTLE_THRESHOLD);
  } if (old.STARTLE_THRESHOLD_DECAY != GLOBALS.STARTLE_THRESHOLD_DECAY) {
    dprint(F("\tSTARTLE_THRESHOLD_DECAY: ")); dprintln(GLOBALS.STARTLE_THRESHOLD_DECAY);
  } if (old.STARTLE_DECAY != GLOBALS.STARTLE_DECAY) {
    dprint(F("\tSTARTLE_DECAY: ")); dprintln(GLOBALS.STARTLE_DECAY);
  } if (old.NUM_CREATURES != GLOBALS.NUM_CREATURES) {
    dprint(F("\tNUM_CREATURES: ")); dprintln(GLOBALS.NUM_CREATURES);
  } if (old.CYCLE_TIME != GLOBALS.CYCLE_TIME) {
    dprint(F("\tCYCLE_TIME: ")); dprintln(GLOBALS.CYCLE_TIME);
  }

  // Handle changes in num creatures.
  if (old.NUM_CREATURES != GLOBALS.NUM_CREATURES) {
    dprint(F("Resizing creature arrays from "));
    dprint(old.NUM_CREATURES);
    dprint(F(" to "));
    dprintln(GLOBALS.NUM_CREATURES);

    delete[] _creatureStates;
    delete[] _creatureDistances;

    // Parens zero initialize.
    _creatureDistances = new int8_t[GLOBALS.NUM_CREATURES + 1]();
    _creatureStates = new uint8_t[GLOBALS.NUM_CREATURES + 1]();
  }

  // Handle change in tx power.
  if (old.TX_POWER != GLOBALS.TX_POWER) {
    _rf69.setTxPower(GLOBALS.TX_POWER, true);
  }

  return true;
}

void Creature::_rxStop() {
  setNextState(getState(0));
}

bool Creature::_rxStart(uint8_t len, uint8_t* payload) {
  if (len != 2) {
    Serial.print(F("Start packet has invalid payload length: "));
    Serial.println(len);
    return false;
  }
  uint8_t mode = payload[0];

  Serial.print(F("Mode: "));Serial.println(mode);
  if (mode) {
    Serial.print(F("Transitioning to _prev ("));Serial.print(_prev->getName());Serial.print(F(") "));Serial.println((uint32_t)_prev, HEX);
    _transition(_prev); // TODO: Fix
  } else {
    uint8_t nextStateID = payload[1];
    if (!nextStateID) {
      nextStateID = rand() % 6 + 1;
    }
    _transition(getState(nextStateID));
  }

  return true;
}

bool Creature::_rxBroadcastStates(uint8_t len, uint8_t* payload) {
  for (int i = 0; i < min(len, GLOBALS.NUM_CREATURES); i++) {
    _creatureStates[i + 1] = payload[i];
  }
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
  if (pid < 16) {
    dprint('0');
  }
  Serial.print(pid, HEX);
  Serial.print(F(" with "));
  Serial.print(len);
  Serial.print(F(" bytes to address "));
  Serial.print(dst_addr);
  Serial.print(F(" from address "));
  Serial.println(_addr);

  for (uint8_t* i = &_buf[3]; i < &_buf[len + 3]; i++) {
    if (*i < 16) {
      dprint('0');
    }
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

void Creature::_pollRadio() {
  // Poll radio for packets
  if (_rf69.waitAvailableTimeout(25)) {
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
    if (pid < 16) {
      dprint('0');
    }
    Serial.print(pid, HEX);
    Serial.print(F(" with "));
    Serial.print(len - 3);
    Serial.print(F(" bytes from address "));
    Serial.print(srcAddr);
    Serial.print(F(" for address "));
    Serial.println(dstAddr);

    for (uint8_t* i = &_buf[3]; i < &_buf[len]; i++) {
      if (*i < 16) {
        dprint('0');
      }
      dprint(*i, HEX);
      dprint(' ');
    }
    dprintln();
    if (_buf[len - 1] == 0) {
      dprintln((char*) &_buf[3]);
    }

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

 State* Creature::getState(int id) {
    switch (id) {
      case 1:
        return new Ambient1(*this);
      case 2:
        return new Active1(*this);
      case 3:
        return new Ambient2(*this);
      case 4:
        return new Active2(*this);
      case 5:
        return new Ambient3(*this);
      case 6:
        return new Active3(*this);
      case 255:
        return new Startle(*this);
      default:
        return new Wait(*this);
    }
  }

void Creature::_transition(State* const state) {
  if (state == nullptr) {
    Serial.println("Cannot transition to null state!");
    return;
  }

  State* const old = _state;

  if (old == nullptr || state->getId() != old->getId()) {
    Serial.print("Transitioning from state ");
    Serial.print(old == nullptr ? -1 : old->getId());
    Serial.print(" to ");
    Serial.println(state->getId());

    _state = state;

    if (state->getId() != WAIT && state->getId() != STARTLE) {
      updateThreshold();
    }

    if (_prev != nullptr && _prev != state) {
      // Delete the current _prev if it's not null, and it's not what we're currently transitioning to.
      delete _prev;
    }

    _txSendState(old == nullptr ? 0 : old->getId(), state->getId());
    _prev = old;
  } else if (state != old) {
    // No need to transition, free this memory if it is not the same state.
    delete state;
  }
  _remainingRepeats = _state->getNumRepeats();
}

void Creature::_updateDisplay() {
  _battery = 0.95 * _battery + 0.05 * getBatteryVoltage();

  _oled.clearDisplay();
  _oled.setBattery(_battery);
  _oled.renderBattery();

  _oled.setCursor(0, 0);
  _oled.print(_state ? _state->getName() : "None");

  _oled.setCursor(0, 11);
  _oled.print(F("TXRX:"));
  _oled.print(_txCount);
  _oled.print(F("/"));
  _oled.println(_rxCount);

  _oled.setCursor((OLED_WIDTH - 2 - (_addr > 9) - (_addr > 99) - sizeof(VERSION)) * 6, 11);
  _oled.print("#");
  _oled.print(_addr);
  _oled.print("v");
  _oled.print(VERSION);

  _oled.setCursor(0, 22);
  _oled.print(F("Sound: "));
  _oled.print(Midi::getSoundIdx());

  uint8_t lightIdx = Neopixel::getLight();
  _oled.setCursor((OLED_WIDTH - 8 - (lightIdx > 9) - (lightIdx > 99)) * 6, 22);
  _oled.print(F("Light: "));
  _oled.print(lightIdx);

  _oled.display();
}

void Creature::setup() {
  Serial.print(F("Booting kit: "));
  Serial.println(KIT_NUM);
  Serial.print(F("Address: "));
  Serial.println(_addr);

  delay(100);
  pinMode(LED_PIN, OUTPUT);

  _oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  _oled.display();
  _oled.clearDisplay();
  _oled.init();
  _oled.setBatteryVisible(true);
  _updateDisplay();

  Neopixel::setup();

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

  // Guaranteed random https://xkcd.com/221
  uint8_t key[] = {0x98, 0xe8, 0xac, 0xe6, 0xfa, 0xca, 0xc1, 0xb8,
                   0xf7, 0xf2, 0x18, 0xc3, 0x5c, 0xce, 0x96, 0x65};
  _rf69.setEncryptionKey(key);

  Serial.print(F("RFM69 radio @ "));
  Serial.print(RFM69_FREQ);
  Serial.println(F("MHz"));

  Midi::setup();

  pinMode(PIR_PIN, INPUT);
  _PIR = digitalRead(PIR_PIN);
  digitalWrite(LED_PIN, _PIR);
}

Creature::~Creature() {
  delete[] _creatureDistances;
  delete[] _creatureStates;
}
