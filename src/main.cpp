#include <Arduino.h>
#include <Keypad.h>
#include <MD_YX5300.h>

enum State
{
  Idle,               // 0
  InsertCoin,         // 1
  OneMoreCoin,        // 2
  EnterWeight,        // 3
  EnterCorrectWeight, // 4
  Wait,               // 5
  EnterStds,          // 6
  WaitAMoment,        // 7
  Glycksmelodie,      // 8
  End,                // 9
};

State previousState = State::Idle;
State currentState = State::Idle;
unsigned long lastInteraction = 0;
int pressedKeys = 0;

const char KEY_YES = '#';
const char KEY_NO = '*';

// KEYPAD

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {3, 2, 1, 0};
byte colPins[COLS] = {6, 5, 4};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char key = 0;

// MOTION SENSOR

const int MOTION_PIN = 20;
boolean movement_active = false;
int movement_current = 0;

// AUDIO PLAYER

const byte AUDIO_RX = 8;
const byte AUDIO_TX = 0;

MD_YX5300 mp3(Serial2);

// COIN SLOT

const byte COINSLOT_PIN = 19;
boolean coin_active = false;
int coin_current = 0;

void setState(State state)
{
  previousState = currentState;
  currentState = state;
  lastInteraction = millis();

  Serial.printf("Next state: %i\n", state);
}

void setup()
{
  pinMode(MOTION_PIN, INPUT);
  pinMode(COINSLOT_PIN, INPUT);

  Serial.begin(9600);
  Serial2.begin(MD_YX5300::SERIAL_BPS);

  mp3.begin();
  mp3.setSynchronous(false);
  mp3.volume(15);
}

void loop()
{
  mp3.check();

  // DETECT KEY PRESS

  key = keypad.getKey();
  if (key)
  {
    mp3.playTrack(1);
    Serial.println(key);
  }

  // DETECT COIN ENTRY

  coin_current = digitalRead(COINSLOT_PIN);
  if (coin_current == LOW && coin_active == false)
  {
    coin_active = true;
    Serial.println("coin!");
  }
  else if (coin_current == HIGH && coin_active == true)
  {
    coin_active = false;
  }

  // DETECT MOTION

  movement_current = digitalRead(MOTION_PIN);
  if (movement_current == HIGH && movement_active == false)
  {
    movement_active = true;
  }
  else if (movement_current == LOW && movement_active == true)
  {
    movement_active = false;
    Serial.println("Keine Bewegung");
  }

  // ACTUAL GLYCKSMELODIE AUTOMATON

  if (currentState != State::Idle && millis() - lastInteraction >= 15000)
  {
    // Resign after 30 seconds without interaction
    setState(State::Idle);
  }

  switch (currentState)
  {
  case State::Idle:
  {
    // Wait for a motion
    if (movement_active)
    {
      Serial.println("Idle: motion detected");
      setState(State::InsertCoin);
    }

    break;
  }

  case State::InsertCoin:
  {
    // Make sure there is a motion
    if (!movement_active && millis() - lastInteraction >= 30000)
    {
      Serial.println("InsertCoin: no more motion");
      setState(State::Idle);
      break;
    }

    if (previousState != State::InsertCoin)
    {
      Serial.println("InsertCoin: Initial transition, play track");
      mp3.playTrack(6); // 7 seconds
      previousState = currentState;
    }
    else if (coin_active)
    {
      // Give the coin slot sensor time to settle before transitioning to the next coin request
      coin_active = false;
      delay(500);

      // Wait for a coin to be inserted
      setState(State::OneMoreCoin);
    }

    break;
  }

  case State::OneMoreCoin:
  {
    if (previousState != State::OneMoreCoin)
    {
      Serial.println("OneMoreCoin: Initial transition, play track");
      mp3.playTrack(7); // 3s
      previousState = currentState;
    }
    else if (coin_active)
    {
      // Wait for a coin to be inserted
      setState(State::Wait);
    }

    break;
  }

  case State::Wait:
  {
    mp3.playTrack(10); // 1s
    delay(3000);
    setState(State::EnterWeight);

    break;
  }

  case State::EnterWeight:
  {
    if (previousState != State::EnterWeight)
    {
      pressedKeys = 0;

      Serial.println("EnterWeight: Initial playback");
      mp3.playTrack(8); // 2s
      previousState = currentState;
    }
    else if (key && key != KEY_YES && key != KEY_NO)
    {
      // Count key presses
      pressedKeys += 1;
    }
    else if (pressedKeys >= 2 && key == KEY_YES)
    {
      // Wait for the YES key to be pressed after at least two numbers have been entered
      delay(200);
      mp3.playTrack(3); // 1s
      delay(1000);

      setState(State::EnterCorrectWeight);
    }

    break;
  }

  case State::EnterCorrectWeight:
  {
    if (previousState != State::EnterCorrectWeight)
    {
      pressedKeys = 0;

      Serial.println("EnterCorrectWeight: Initial playback");
      mp3.playTrack(9); // 2s
      previousState = currentState;
    }
    else if (key && key != KEY_YES && key != KEY_NO)
    {
      // Count key presses
      pressedKeys += 1;
    }
    else if (pressedKeys >= 2 && key == KEY_YES)
    {
      // Wait for the YES key to be pressed after at least two numbers have been entered
      delay(200);
      mp3.playTrack(4); // 1s
      delay(1000);

      setState(State::EnterStds);
    }

    break;
  }

  case State::EnterStds:
  {
    if (previousState != State::EnterStds)
    {
      Serial.println("EnterStds: Initial playback");
      mp3.playTrack(11); // 2s
      previousState = currentState;
    }
    else if (key == KEY_YES || key == KEY_NO)
    {
      // Wait for the YES key to be pressed
      delay(200);
      mp3.playTrack(5); // 1s
      delay(1000);

      setState(State::WaitAMoment);
    }

    break;
  }

  case State::WaitAMoment:
  {
    mp3.playTrack(12); // 2s
    delay(7000);
    setState(State::Glycksmelodie);

    break;
  }

  case State::Glycksmelodie:
  {
    mp3.playTrack(13); // 11s
    delay(13000);
    mp3.playTrack(14); // 3s
    delay(5000);
    mp3.playTrack(15); // 3s
    delay(6000);
    mp3.playTrack(16); // 5s
    delay(10000);
    setState(State::End);

    break;
  }

  case State::End:
  {
    mp3.playTrack(17); // 4s
    delay(4000);
    setState(State::Idle);

    break;
  }
  }
}