#include "functions.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== GLOBAL STATE =====
AlarmState state;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

#define BTN_SNOOZE 2
#define BTN_STOP   3
#define BTN_SET    4
#define CLK        5
#define DT         6
#define SW         7
#define ALARM      8

int lastCLK = HIGH;

// ===== ALARM CONTROL =====
void setAlarm()
{
    state.mode = ARMED;
    dialController();
    if(state.alarmHour < state.currentHour)
    {
        state.waitNextDay = true;
    }
}

void stopAlarm()
{
    state.mode = ARMED;
    state.waitNextDay = true;
    digitalWrite(ALARM, LOW);

}

void snoozeAlarm()
{
    state.mode = SNOOZED;
    digitalWrite(ALARM, LOW);
         // Snooze for 15 Minutes
    state.snoozeMinute = state.alarmMinute + 15;
    if(state.snoozeMinute > 59)
    {
        state.snoozeMinute -= 60;
        state.snoozeHour += 1;
        if(state.snoozeHour > 23)
        {
            state.snoozeHour -= 24;
            state.waitNextDay = true;
        }
    }

}

// ===== TIME =====

void setCurrentTime() {
    state.lastCheckMs = millis();
    dialController();
}

void updateCurrentTime() {
    int tempPriorMs = state.lastCheckMs;
    state.lastCheckMs = millis();
    state.overflowMs += state.lastCheckMs - tempPriorMs;
    if(state.overflowMs >= 60000)
    { 
        state.currentMinute += (state.overflowMs / 60000);
        state.overflowMs %= 60000;
        if(state.currentMinute > 59)
        {
            state.currentHour++;
            state.currentMinute %= 60;
        }
        if(state.currentHour > 23)
        {
            // Detect new day
            state.currentHour -= 24;
            state.waitNextDay = false;
        }
    }
}

// ===== INPUT =====

bool pressedSnooze() {
    static unsigned long lastPress = 0;

    if (digitalRead(BTN_SNOOZE) == LOW) {
            return true;
    }
    return false;
}


bool pressedStop() {
    static unsigned long lastPress = 0;

    if (digitalRead(BTN_STOP) == LOW) {
        return true;
    }
    return false;
}


bool pressedSet() {
    static unsigned long lastPress = 0;

    if (digitalRead(BTN_SET) == LOW) {
        return true;
    }
    return false;
}


void drawDisplay(int hour, int minute)
{
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(4,0);
    if (hour < 10) display.print("0");
    display.print(hour);
    display.print(":");
    if(minute < 10) display.print("0");
    display.print(minute);
    display.display();

}

void initButtons() {
    pinMode(BTN_SNOOZE, INPUT_PULLUP);
    pinMode(BTN_STOP, INPUT_PULLUP);
    pinMode(BTN_SET, INPUT_PULLUP);
}

void initDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // failed to initialize
        for (;;);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void ringAlarm() {
    digitalWrite(ALARM, HIGH);
}

void dialController() {
    int tempHour = 0;
    int tempMinute = 0;
    bool minutes = false;
    drawDisplay(tempHour, tempMinute);

    while(!pressedSet())
    {
        if(isEncoderPressed())
        {
            delay(500);
            if(!minutes)
            {
                minutes = true;
            }
            else
            {
                minutes = false;
            }
        }
        if(minutes)
        {
            tempMinute += readEncoderStep();
        }
        else
        {
            tempHour += readEncoderStep();
        }

        if(tempMinute < 0)
        {
            tempMinute += 60;
        }
        if(tempMinute > 59)
        {
            tempMinute -= 60;
        }
        if(tempHour < 0)
        {
            tempHour += 24;
        }
        if(tempHour > 23)
        {
            tempHour -= 24;
        }
        drawDisplay(tempHour, tempMinute);
    }

    // Current Time
    if(state.mode == IDLE)
    {
        state.currentHour = tempHour;
        state.currentMinute = tempMinute;
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);

    }

    // ALARM
    if(state.mode == ARMED)
    {
        state.alarmHour = tempHour;
        state.alarmMinute = tempMinute;
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);

    }

}

void checkAlarmTime()
{
    drawDisplay(state.alarmHour, state.alarmMinute);
    delay(3000);
}

void initEncoder() {
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DT, INPUT_PULLUP);
    pinMode(SW, INPUT_PULLUP);
}


int readEncoderStep() {
    int currentCLK = digitalRead(CLK);

    if (currentCLK != lastCLK && currentCLK == LOW) {
        // direction check
        if (digitalRead(DT) != currentCLK) {
            lastCLK = currentCLK;
            return +1;
        } else {
            lastCLK = currentCLK;
            return -1;
        }
    }

    lastCLK = currentCLK;
    return 0;
}

bool isEncoderPressed()
{
    return digitalRead(SW) == LOW;
}

void initAlarm()
{
    pinMode(ALARM, OUTPUT);
}
