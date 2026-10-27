# Automatic Sliding Door System
### BACSE324 — Embedded Systems Architecture · Fall 2026

> **Authors:** Meet Chavda (25BCE2491) · S Sreyas  (25BCE2496)  
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

               dist < 100 cm
   ┌──────────────────────────────────────────────┐
   │                                              ▼
┌─────────┐   dist < 100 cm   ┌──────────┐   angle == 90°    ┌───────────┐
│ CLOSED  │──────────────────▶│ OPENING  │──────────────────▶ │ OPEN_HOLD │
└─────────┘                   └──────────┘                    └───────────┘
▲                                                              │ timer == 5s
│                                                              ▼
│                    angle == 0°                         ┌─────────┐
└────────────────────────────────────────────────────────│ CLOSING │
└─────────┘
│ dist < 100 cm
▼
┌────────────┐
│ OBSTRUCTED │──▶ OPENING
└────────────┘  (after 500 ms)


| State Code | State Name | Servo Angle | Indicator LED Status |
|---|---|---|---|
| `S_CLOSED` (0) | `CLOSED` | 0° (Closed) | All LEDs Off |
| `S_OPENING` (1) | `OPENING` | Ramping 0° ➔ 90° | All LEDs Off |
| `S_OPEN_HOLD` (2) | `OPEN_HOLD` | 90° (Held Open) | Green LED ON |
| `S_CLOSING` (3) | `CLOSING` | Ramping 90° ➔ 0° | All LEDs Off |
| `S_OBSTRUCTED` (4) | `OBSTRUCTED` | Stopped at current angle | Red LED ON (Pauses 500 ms) |

---

## Software Architecture & File Tree

automatic-sliding-door/
├── automatic_sliding_door.ino     # Main Arduino firmware source code
├── Door Sensor Code.rtf           # Simulation code and reference logic
├── dashboard.html                 # Web-based interactive project dashboard
└── README.md                      # Project documentation


### Main Execution Loop Functions
* `readDistanceCm()`: Triggers ultrasonic pulse and calculates target distance via acoustic echo timing (`pulseIn`).
* `moveServo()`: Moves servo 1 degree per step with variable timing intervals (40 ms near limits, 12 ms mid-travel) to provide soft acceleration/deceleration.
* `runFSM()`: Evaluates sensor distance against FSM transition rules every cycle and controls LED indicators.

---

## Implementation Ownership

| Model element (FSM state, task) | Planned file | Planned function | Owner |
| :--- | :--- | :--- | :--- |
| **Ultrasonic Sensor Reading** | `automatic_sliding_door.ino` | `readDistanceCm()` | Meet Chavda |
| **Servo Position Step & Soft Start/Stop** | `automatic_sliding_door.ino` | `moveServo()` | Sreyas S |
| **FSM States: `CLOSED`, `OPEN_HOLD`** | `automatic_sliding_door.ino` | `runFSM()` (State Logic) | Meet Chavda |
| **FSM States: `OPENING`, `CLOSING`** | `automatic_sliding_door.ino` | `runFSM()` (State Logic) | Sreyas S |
| **FSM State: `OBSTRUCTED`** | `automatic_sliding_door.ino` | `runFSM()` (State Logic) | Meet Chavda & Sreyas S |
| **State Transition Event Logger** | `automatic_sliding_door.ino` | `setState()` | Meet Chavda |
| **Main Setup & Initialization** | `automatic_sliding_door.ino` | `setup()` | Meet Chavda |
| **Cyclic Executive Dispatcher Loop** | `automatic_sliding_door.ino` | `loop()` | Meet Chavda |

---

## How to Run & Flash

1. Open **Arduino IDE**.
2. Install the standard **Servo** library (included by default in Arduino IDE).
3. Connect your Arduino Uno R3 via USB.
4. Open `automatic_sliding_door.ino`.
5. Select **Tools -> Board -> Arduino Uno** and pick your COM port.
6. Click **Upload** (`Ctrl + U`).
7. Open **Serial Monitor** (`Ctrl + Shift + M`) set to **9600 baud** to view real-time state telemetry and timing logs.
8. Open `dashboard.html` in any web browser to view the interactive project dashboard and live FSM visualizer.

---

*BACSE324 Embedded Systems Architecture · Fall 2026 · VIT Vellore*
