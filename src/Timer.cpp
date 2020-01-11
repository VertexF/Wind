#include "Timer.h"

Timer::Timer()
{
    start = false;
    paused = false;

    StartTicks = 0;
    PausedTicks = 0;

    CurrentFrame = 0;
    mLastFrameDuration = 0;
    mAverageFrameDuration = 0;
}

void Timer::Start()
{
    start = true;
    paused = false;
	FPS = 0;

    StartTicks = SDL_GetTicks();
    PausedTicks = 0;
}

void Timer::Stop()
{
    start = false;
    paused = false;

    StartTicks = 0;
    PausedTicks = 0;
}

void Timer::Pause()
{
    if(start && !paused)
    {
        paused = true;

        PausedTicks = SDL_GetTicks() - StartTicks;
        StartTicks = 0;
    }
}

void Timer::Unpause()
{
    if(start && paused)
    {
        paused = false;

        StartTicks = SDL_GetTicks() - PausedTicks;
        PausedTicks = 0;
    }
}

Uint32 Timer::GetTicks()
{
    Uint32 Time = 0;

    if(start)
    {
        if(paused)
        {
            Time = PausedTicks;
        }
        else
        {
            Time = SDL_GetTicks() - StartTicks;
        }
    }

    return Time;
}

void Timer::Update()
{
    if(!is_paused())
    {
        CurrentFrame++;
    }

    unsigned int GlobalTime = SDL_GetTicks();
    mLastFrameDuration = GlobalTime - StartTicks;
    StartTicks = GlobalTime;

    if(CurrentFrame > 1)
    {
        if(mAverageFrameDuration <= 0)
        {
            mAverageFrameDuration = mLastFrameDuration;
        }
        else
        {
            mAverageFrameDuration *= 0.99;
            mAverageFrameDuration += 0.01 * StartTicks;

            FPS = 1000.0 / mAverageFrameDuration;
        }
    }
}

Uint32 Timer::GetCurrentTicks()
{
    return StartTicks;
}

bool Timer::is_started()
{
    return start;
}

bool Timer::is_paused()
{
    return paused;
}
