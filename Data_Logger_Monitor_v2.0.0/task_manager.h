#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>

// ============================================================
// FreeRTOS task scheduling and management
// ============================================================
// Creates dedicated tasks for:
//   - Data acquisition (PZEM reads)
//   - MQTT publishing
//   - OLED rendering
//   - Web server
//   - Watchdog monitoring
// ============================================================

class TaskManager {
public:
    TaskManager();

    void begin();

    // Create tasks (call after all managers initialized)
    void createTasks();

    // Watchdog feed (call from main loop)
    void feedWatchdog();

    // Delay helper: non-blocking millis()-style scheduling
    bool isTime(unsigned long& lastTick, unsigned long interval);

    // System health
    static void logTaskStats();
};

#endif
