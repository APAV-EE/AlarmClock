// "Smart" Alarm Clock
// Features:
// Set, Stop, Snooze
#include "functions.h"
#include <Wire.h>
#include <Arduino.h>

void setup()
{
  // Initialize Functions
  Wire.begin();
  initButtons();
  initDisplay();
  initEncoder();
  initAlarm();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);

  // Alarm and Time Setup
  state.mode = IDLE;
  if(state.mode == IDLE)
  {
    setCurrentTime();
    setAlarm(); 
  }

}

void loop() {
  // Update Current Time on Display and Raw
  updateCurrentTime();
  drawDisplay(state.currentHour, state.currentMinute);
  // Check If Alarm Conditions Met, If so, Keep Ringing until snooze or stop are pressed
  if((state.currentHour >= state.alarmHour && state.currentMinute >= state.alarmMinute && state.mode == ARMED && !state.waitNextDay) || state.mode == RINGING)
  {
      state.mode = RINGING;
      while(state.mode == RINGING)
      {
        ringAlarm();
        if(pressedStop())
        {
          stopAlarm();
        }

        if(pressedSnooze())
        {
          snoozeAlarm();
        }
      }
  }
  else if(pressedStop() && state.mode == ARMED)
  {
    checkAlarmTime();
  }
  
  if(pressedSet())
  {
    delay(500);
    setAlarm();
  }

  // Check Snooze
  if(state.mode == SNOOZED && state.currentHour >= state.snoozeHour && state.currentMinute >= state.snoozeMinute && !state.waitNextDay)
  {
    state.mode = RINGING; 
  }
}