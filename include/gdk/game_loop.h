// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GAME_LOOP_H
#define GDK_GAME_LOOP_H

#include <chrono>
#include <functional>
#include <thread>

namespace gdk {
    void game_loop(const float aMaxFramesPerSecond, 
        std::function<bool /*shouldClose*/(const double /*time*/, const double /*delta time*/)> aLoopBehavior) {
        bool shouldClose(false);
        double time(0);
        double deltaTime(0);
        double frameTime(1.0 / aMaxFramesPerSecond);

        while (!shouldClose) {
            using namespace std::chrono;

            steady_clock::time_point currentFrameStartTimePoint(steady_clock::now());

            shouldClose = aLoopBehavior(time, deltaTime);

            time += deltaTime;

            while (true) {
                steady_clock::time_point currentTimePoint(steady_clock::now());

                duration<double> timeSpentOnCurrentFrame = duration_cast<duration<double>>(currentTimePoint - currentFrameStartTimePoint);

                if (deltaTime = timeSpentOnCurrentFrame.count(); deltaTime >= frameTime) break;
                
                std::this_thread::sleep_for(1s * (frameTime - deltaTime) / 10.0);
            }
        }
    }
}

#endif

