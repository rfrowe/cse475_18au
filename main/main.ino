#include "State.h"

#define KIT_NUM 10

Creature *c;

void setup() {
  c = new Creature(KIT_NUM);
}

void loop() {
  c->loop();
}
