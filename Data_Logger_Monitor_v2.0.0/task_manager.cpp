#include "task_manager.h"
#include "config.h"
#include "diagnostics.h"

TaskManager::TaskManager() {}

void TaskManager::begin() {
    // Enable watchdog timer
    enableLoopWDT();
    diag.info("TASK", "Watchdog enabled (timeout=%dms)", WDT_TIMEOUT_MS);
}

void TaskManager::createTasks() {
    // In this design we use a cooperative scheduler in loop()
    // rather than separate FreeRTOS tasks to keep things simple
    // for beginners. The loop() handles all subsystems via
    // millis()-based non-blocking scheduling.
    //
    // For advanced users: tasks can be added here using:
    //   xTaskCreatePinnedToCore(acqTask, "acq", 4096, NULL, 1, NULL, 0);
    diag.info("TASK", "Cooperative scheduler active");
}

void TaskManager::feedWatchdog() {
    // Feed the Arduino ESP32 watchdog
    // In ESP32 Arduino, the watchdog is fed automatically in loop()
    // if loop() doesn't block for > WDT_TIMEOUT_MS
}

bool TaskManager::isTime(unsigned long& lastTick, unsigned long interval) {
    unsigned long now = millis();
    if (now - lastTick >= interval) {
        lastTick = now;
        return true;
    }
    return false;
}

void TaskManager::logTaskStats() {
    #if CONFIG_FREERTOS_USE_TRACE_FACILITY
        char buf[512];
        vTaskList(buf);
        Serial.println(buf);
    #endif
    diag.info("TASK", "Free heap: %u | Max alloc: %u",
              ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}
