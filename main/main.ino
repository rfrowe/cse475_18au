#include "Creature.h"

#define KIT_NUM 10

Creature c(KIT_NUM);

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println(F("Initializing creature"));
  c.setup();
}

void loop() {
  c.loop();
}
