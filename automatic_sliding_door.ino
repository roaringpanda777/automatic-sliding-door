#include <Servo.h>
Servo door;

// ---------- Pins ----------
const uint8_t PIN_SERVO = 9;
const uint8_t PIN_TRIG  = 7;
const uint8_t PIN_ECHO  = 8;
const uint8_t LED_G     = 12;   // on while door is held open
const uint8_t LED_R     = 13;   // on when obstructed

// ---------- Parameters ----------
const int  CLOSED_ANGLE = 0;
const int  OPEN_ANGLE   = 90;
const long PRESENCE_CM  = 100;               // closer than this = person present
const unsigned long HOLD_MS = 5000;          // hold-open time
const unsigned long REOPEN_PAUSE_MS = 500;   // pause after obstruction

// ---------- States ----------
const uint8_t S_CLOSED = 0, S_OPENING = 1, S_OPEN_HOLD = 2,
              S_CLOSING = 3, S_OBSTRUCTED = 4;
const char* stateName[] = { "CLOSED", "OPENING", "OPEN_HOLD",
                            "CLOSING", "OBSTRUCTED" };
uint8_t state = S_CLOSED;

int angle = CLOSED_ANGLE;
int targetAngle = CLOSED_ANGLE;
long dist = 999;
unsigned long stateStart = 0, holdStart = 0;
unsigned long lastMove = 0, lastSense = 0, lastPrint = 0;

void setState(uint8_t s) {
  state = s;
  stateStart = millis();
  Serial.print(F("EVT t="));
  Serial.print(stateStart);
  Serial.print(F(" -> "));
  Serial.println(stateName[s]);
}

long readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long us = pulseIn(PIN_ECHO, HIGH, 25000UL);
  if (us == 0) return 999;      // nothing in range
  return us / 58;
}

// Moves the servo 1 degree at a time. Slow near the ends (soft start/stop).
void moveServo() {
  if (angle == targetAngle) return;
  unsigned long interval = (angle < 15 || angle > 75) ? 40 : 12;
  if (millis() - lastMove >= interval) {
    lastMove = millis();
    if (angle < targetAngle) angle++; else angle--;
    door.write(angle);
  }
}

void runFSM() {
  bool present = (dist < PRESENCE_CM);
  unsigned long now = millis();

  switch (state) {
    case S_CLOSED:
      targetAngle = CLOSED_ANGLE;
      if (present) setState(S_OPENING);
      break;
    case S_OPENING:
      targetAngle = OPEN_ANGLE;
      if (angle == OPEN_ANGLE) { holdStart = now; setState(S_OPEN_HOLD); }
      break;
    case S_OPEN_HOLD:
      if (present) holdStart = now;                       // reset countdown
      else if (now - holdStart >= HOLD_MS) setState(S_CLOSING);
      break;
    case S_CLOSING:
      targetAngle = CLOSED_ANGLE;
      if (present) setState(S_OBSTRUCTED);                // obstruction!
      else if (angle == CLOSED_ANGLE) setState(S_CLOSED);
      break;
    case S_OBSTRUCTED:
      targetAngle = angle;                                // stop right here
      if (now - stateStart >= REOPEN_PAUSE_MS) setState(S_OPENING);
      break;
  }
  digitalWrite(LED_G, state == S_OPEN_HOLD);
  digitalWrite(LED_R, state == S_OBSTRUCTED);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  door.attach(PIN_SERVO);
  door.write(CLOSED_ANGLE);
  Serial.println(F("Door controller started"));
}

void loop() {
  if (millis() - lastSense >= 60) {          // read sensor every 60 ms
    lastSense = millis();
    dist = readDistanceCm();
  }
  runFSM();
  moveServo();

  if (millis() - lastPrint >= 500) {         // status line every 500 ms
    lastPrint = millis();
    Serial.print(F("t=")); Serial.print(millis());
    Serial.print(F(" ")); Serial.print(stateName[state]);
    Serial.print(F(" dist=")); Serial.print(dist);
    Serial.print(F("cm angle=")); Serial.print(angle);
    if (state == S_OPEN_HOLD) {
      long left = (long)HOLD_MS - (long)(millis() - holdStart);
      if (left < 0) left = 0;
      Serial.print(F(" hold_left_ms=")); Serial.print(left);
    }
    Serial.println();
  }
}