#pragma once

#include <iostream>
#include <chrono>

class Timer 
{
public:
    void start() 
    {
        if (!running && !paused) {
            startTime = std::chrono::high_resolution_clock::now();
            running = true;
        }
        else if (paused) {
            auto now = std::chrono::high_resolution_clock::now();
            startTime += (now - pauseTime);
            paused = false;
        }
        else {
            std::cout << "Timer is already running." << std::endl;
        }
    }

    void pause() 
    {
        if (running && !paused) {
            pauseTime = std::chrono::high_resolution_clock::now();
            paused = true;
        }
    }

    void stop() 
    {
        if (running) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
            std::cout << "Duration: " << duration << " microseconds" << std::endl;
            running = false;
            paused = false;
        }
        else {
            std::cout << "Timer is not running." << std::endl;
        }
    }

    void reset() 
    {
        running = false;
        paused = false;
        startTime = std::chrono::time_point<std::chrono::high_resolution_clock>();
    }

    long long elapsedMicroseconds() 
    {
        if (running) {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count();
        }
        return 0;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime, pauseTime;
    bool running = false;
    bool paused = false;
};