# Automatic Sliding Door System
### BACSE324 — Embedded Systems Architecture · Fall 2026

> **Authors:** Meet Chavda (25BCE2491) · Sreyas S (25BCE2496)  
> **Institution:** VIT University  
> **Hardware:** Arduino Uno R3 · Servo Motor · HC-SR04 Ultrasonic Sensor · Status LEDs

---

## Overview

This project implements an automatic sliding door system using an Arduino Uno R3, an HC-SR04 ultrasonic sensor, standard status LEDs, and a servo motor drive (0° = CLOSED, 90° = OPEN). 

The controller detects approaching people within 100 cm, opens the door smoothly with position-dependent speed intervals (soft-start/stop), holds the door open for 5 seconds (resetting if someone remains in the doorway), and handles closing obstructions by pausing 500 ms before reopening. The entire operation runs on a 5-state Mealy Finite State Machine (FSM) driven non-blockingly inside the main execution loop.

---

## Hardware Pin Mapping

| Arduino Pin | Hardware Component | Function / Notes |
|---|---|---|
| Pin 9 | Servo Motor Data Line | Door position control (0° to 90°) |
| Pin 7 | HC-SR04 TRIG | Ultrasound trigger pulse output |
| Pin 8 | HC-SR04 ECHO | Echo duration return input (sampled via `pulseIn`) |
| Pin 12 | Green LED (`LED_G`) | Active during `S_OPEN_HOLD` state |
| Pin 13 | Red LED (`LED_R`) | Active during `S_OBSTRUCTED` state |

---

## FSM State Machine Model

The system implements a **5-state Mealy Finite State Machine**: