#ifndef _CREATURE_H_
#define _CREATURE_H_

#include <cinttypes>

#include <Adafruit_FeatherOLED.h>
#include <RH_RF69.h>
#include <Wire.h>

#include "Midi.h"

class State;

#define VREF 3.3

#define ID_PIN A0
#define PIR_PIN A1
#define VBAT_PIN A7
#define LED_PIN 13

#define OLED_WIDTH 21 // chars

#define RFM69_FREQ 915.0
#define RFM69_CS 8
#define RFM69_INT 3
#define RFM69_RST 4
#define CONTROLLER_ADDR 0x00
#define BROADCAST_ADDR 0xFF

#define PID_SET_GLOBALS 0x0
#define PID_STOP 0x1
#define PID_START 0x2
#define PID_PLAY_SOUND 0x3
#define PID_PLAY_EFFECT 0x4
#define PID_BROADCAST_STATES 0x5
#define PID_STARTLE 0x6
#define PID_SEND_STATE 0x7

struct Globals {
  uint16_t TX_POWER;
  uint8_t STARTLE_RAND_MIN;
  uint8_t STARTLE_RAND_MAX;
  uint8_t STARTLE_MAX;
  uint8_t STARTLE_THRESHOLD;
  uint8_t STARTLE_DECAY;
  uint8_t NUM_CREATURES;
  float STARTLE_THRESHOLD_DECAY;
  uint16_t CYCLE_TIME;
};

class Creature {
 public:
  Creature();
  Creature(const Creature&) = delete;
  Creature& operator=(Creature const&) = delete;
  ~Creature();

  Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();
  struct Globals GLOBALS = {
    /* TX_POWER */                  14,   // uint16_t
    /* STARTLE_RAND_MIN */          100,  // uint8_t
    /* STARTLE_RAND_MAX */          200,  // uint8_t
    /* STARTLE_MAX */               255,  // uint8_t
    /* STARTLE_THRESHOLD */         150,  // uint8_t
    /* STARTLE_DECAY */             30,   // uint8_t
    /* NUM_CREATURES */             30,   // uint8_t
    /* STARTLE_THRESHOLD_DECAY */   0.9,  // float32
    /* CYCLE_TIME in ms */          100,  // uint16_t
  };

  /**
   * Transmits a packet to dst_addr from this creature.
   *
   * @param pid Packet identifier.
   * @param dst_addr  Address of the intended recipient of this packet.
   * @param len Length of the payload, in bytes
   * @param payload An array of bytes to use as the payload.
   * @returns true iff the packet was successfully sent, false otherwise.
   */
  bool tx(const uint8_t pid, const uint8_t dstAddr, const uint8_t len, uint8_t* const payload);

  /**
   * Sets the next state to transition into. If this is set, the next loop will trigger
   * a transition into this state.
   *
   * @param next  State to transition into.
   */
  void setNextState(State* const next) {
    if (_next != nullptr && next != _next) {
      delete _next;
    }
    _next = next;
  }

  // Getters and Setters
  uint8_t getAddr() {
    return _addr;
  }

  uint8_t getLastStartleId() {
    return _lastStartleId;
  }

  void setLastStartleId(uint8_t startleId) {
    _lastStartleId = startleId;
  }

  uint32_t getLastStartle() {
    return _lastStartle;
  }

  void setLastStartlee(uint32_t lastStartle) {
    _lastStartle = lastStartle;
  }

  uint8_t* getCreatureStates() {
    return _creatureStates;
  }

  int8_t* getCreatureDistances() {
    return _creatureDistances;
  }

  // Run after construction but before loop.
  void setup();

  // Called during main loop.
  void loop();
 private:
  /**
   * Called during loop to poll radio for new received packets. Calls Creature::rx with any
   * received packets that were intended for this creature.
   */
  void _pollRadio();

  /**
   * Handles the receipt of a packet.
   *
   * @param pid Packet identifier of the received packet.
   * @param src_addr  Address of the packet's sender
   * @param len Length of the payload, in bytes.
   * @param payload Packet payload as an array of bytes.
   * @param rssi  Signal strength of received packet, in db.
   * @returns true iff the packet was handled successfully, false otherwise.
   */
  bool _rx(uint8_t pid, uint8_t srcAddr, uint8_t len, uint8_t* payload, int8_t rssi);

  /** Handles updating GLOBALS */
  bool _rxSetGlobals(uint8_t len, uint8_t* payload);

  /** Stops the creature by transitioning to WAIT */
  void _rxStop();

  /**
   *  Starts the creature by transitioning to the next appropriate state baseed on mode.
   *
   *  @params payload  Should be mode to start in. 0x8XXX for continue, 0x0000 for random start, 0x00XX for state XX.
   */
  bool _rxStart(uint8_t len, uint8_t* payload);

  /**
   * Called when all states are broadcast by the controller. Should update this->_creatureStates.
   *
   * @param payload  Should be an array of GLOBALS.NUM_CREATURES + 1 states for the states of creatures
   * 0 through NUM_CREATURES.
   */
  bool _rxBroadcastStates(uint8_t len, uint8_t* payload);

  /**
   * Transmits an SendState packet to notify the controller that this creature
   * transitioned from oldState to newState.
   *
   * @param oldState  State we're transitioning from.
   * @param newState  State we're transitioning to.
   */
  void _txSendState(uint8_t oldState, uint8_t newState);

  /**
   * Transitions this creature from it's current state to the provided state.
   * This also includes calling _txEnteredState, calling the current state's
   * destructor, and updating _remainingRepeats.
   *
   * @param state New state to transition into.
   */
  void _transition(State* const state);

  /**
   * Updates the running distance measure in _creatureDistances.
   *
   * @param addr  Address of the creature to update.
   * @param rssi  Signal strength measurement from latest tx.
   */
  void _updateDistance(uint8_t addr, int8_t rssi);

  /** Measures battery voltage and updates OLED. */
  void _updateDisplay();

  /** Current, next, and previous states, or null if no next state. */
  State *_state, *_next, *_prev;

  uint8_t _kitNum, _addr;

  /** Random id of last startle that startled this creature */
  uint8_t _lastStartleId;

  /**
   * Threshold for startles. Should be updated during startle based on time since last startle,
   * STARTLE_THRESHOLD_DECAY, and the current state's startle factor.
   */
  uint8_t _startleThreshold;

  /** Number of times the current state should be repeated */
  uint8_t _remainingRepeats;

  /** Timestamps for last startle and last loop execution */
  uint32_t _lastStartle, _lastLoop;

  /** State ids for all creatures 0 through NUM_CREATURES. Should be of size GLOBALS.NUM_CREATURES + 1 */
  uint8_t *_creatureStates;

  /** Running average of pseudodistance (RSSI) for all creatures 0 through NUM_CREATURES. Should be of size GLOBALS.NUM_CREATURES + 1 */
  int8_t *_creatureDistances;

  /** Buffer used for radio tx/rx. */
  uint8_t _buf[RH_RF69_MAX_MESSAGE_LEN] = {0};

  /** Radio object for tx/rx */
  RH_RF69 _rf69 = RH_RF69(RFM69_CS, RFM69_INT);

  uint8_t _txCount, _rxCount;

  /** Current battery voltage. */
  float _battery;

  /** Last known state of the PIR sensor. Used for duplicate detection. */
  bool _PIR;
};

#endif  // _CREATURE_H_
