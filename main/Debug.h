#ifndef DEGBUG

// Enables serial debugging
//#define DEBUG

// Disables serial debugging if DEBUG not defined
#ifdef DEBUG
  #define dprintln(A...) Serial.println(A)
  #define dprintf(A...)  Serial.printf(A)
  #define dprint(A...)   Serial.print(A)
#else
  #define dprintln(A...)
  #define dprintf(A...)
  #define dprint(A...)
#endif

#endif

