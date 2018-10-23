# CSE 475: Embedded Systems Capstone 
## Creature Spec

This repository contains starter code for the Creature project. It is a partial, but functioning implementation of the [spec](https://courses.cs.washington.edu/courses/cse475/18au/pdfs/spec.pdf). With no modification, this will initialize the radio, OLED, and neopixel. The creature is capable of sending and receiving packets. It immediately enters the `Wait` state after initializing and setting `GLOBALS` to the default values.

## Implementation
To fully implement the spec, you'll first need to fill out thee `TODO`s. After that, implement 3 `Ambient` states, 3 `Active` states, and a `Startle` state. We've also left the MIDI component unimplemented. You'll need to add MIDI support using [this](https://courses.cs.washington.edu/courses/cse475/18au/Code/M0_feather_midi_timer/M0_feather_midi_timer.ino) as inspiration. Use the way the radio, OLED, and neopixel are supported. You may also wish to make convienence methods in `Creature` for the OLED, neopixel, and MIDI like the `rx` and `tx` methods for radio communication.

## Files
```
.
├── images
│   └── creature.jpg
├── latexmkrc
├── main
│   ├── Creature.cpp
│   ├── Creature.h
│   │     Creature header and implementation
│   ├── Debug.h
│   │     Debug `defines`. Comment out `#define DEBUG` to disable debug
│   │     `printlns`, but not all `printlns`.
│   ├── State.cpp
│   │     Base implementations for some `State` methods which will be
│   │     inherited by all states.
│   ├── State.h
│   │     State class definition. This is what you will subclass to make
│   │     each of your states. Methods marked `virtual ... = 0;` are
│   │     abstract and must be implemented in your subclass. You can
│   │     instead choose to implement them in `State.cpp` to have an
│   │     inherited implementation
│   ├── Wait.cpp
│   ├── Wait.h
│   │     A sparsely implemented `Wait` state. It loops the maximum number
│   │     of times and always transitions to itself. You shouldn't need
│   │     to do much of anything here.
│   └── main.ino
│         Actual `setup` and `loop` called by Arduino.  You shouldn't
│         need to change anything here.     
└── main.tex
    Spec source
```
