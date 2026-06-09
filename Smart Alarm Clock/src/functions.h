#pragma once

// ===== STATE MACHINE =====
enum AlarmMode {
    IDLE,
    ARMED,
    RINGING,
    SNOOZED
};

struct AlarmState {
    AlarmMode mode;
    int alarmHour;
    int alarmMinute;
    int currentHour;
    int currentMinute;
    int snoozeHour;
    int snoozeMinute;
    unsigned long overflowMs;
    unsigned long lastCheckMs;
    bool waitNextDay;
};

// Global state
extern AlarmState state;


// ===== ALARM CONTROL =====
void setAlarm();
void stopAlarm();
void snoozeAlarm();
void updateAlarm();


// ===== TIME =====
void setCurrentTime();
void checkAlarmTime();
void updateCurrentTime();

// ===== INPUT =====
bool pressedSnooze();
bool pressedStop();
bool pressedSet();
void initButtons();
void dialController();
void initEncoder();
bool isEncoderPressed();
int readEncoderStep();

// ===== OUTPUT =====
void drawDisplay(int hour, int minute);
void initDisplay();
void ringAlarm();
void initAlarm();