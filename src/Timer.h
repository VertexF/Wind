#ifndef TIMER_H
#define TIMER_H

namespace wind
{
class Timer
{
public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();

    long long getTicks();
    //This function get the global time, counts the frames it's one, works out the average duration of the frame and keeps track of the FPS
    void update();

    bool isStarted();
    bool isPaused();
    long long getCurrentTicks();

    long long _lastFrameDuration;
    double _averageFrameDuration;
    double _FPS;
private:
    long long _startTicks;
    long long _pausedTicks;

    long long _currentFrame;

    bool _start;
    bool _paused;
};
}; //wind

#endif // TIMER_H
