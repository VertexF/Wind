#ifndef TIMER_H
#define TIMER_H
#include <SDL.h>

class Timer
{
    public:
        Timer();
        void Start();
        void Stop();
        void Pause();
        void Unpause();

        Uint32 GetTicks();
        //This function get the global time, counts the frames it's one, works out the average duration of the frame and keeps track of the FPS
        void Update();

        bool is_started();
        bool is_paused();
        Uint32 GetCurrentTicks();

        Uint32 mLastFrameDuration;
        double mAverageFrameDuration;
        double FPS;
    private:
        Uint32 StartTicks;
        Uint32 PausedTicks;

        Uint32 CurrentFrame;

        bool start;
        bool paused;
};

#endif // TIMER_H
