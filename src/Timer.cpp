#include "Timer.h"

#include <SDL.h>

namespace wind
{

/******************************************************************************/
Timer::Timer() : _start(false), _paused(false), _startTicks(0), _pausedTicks(0),
_currentFrame(0), _lastFrameDuration(0), _averageFrameDuration(0), _FPS(0)
{
}

/******************************************************************************/
void Timer::start()
{
    _start = true;
    _paused = false;
    _FPS = 0;

    _startTicks = SDL_GetTicks();
    _pausedTicks = 0;
}

/******************************************************************************/
void Timer::stop()
{
    _start = false;
    _paused = false;

    _startTicks = 0;
    _pausedTicks = 0;
}

/******************************************************************************/
void Timer::pause()
{
    if (_start && !_paused)
    {
        _paused = true;

        _pausedTicks = SDL_GetTicks() - _startTicks;
        _startTicks = 0;
    }
}

/******************************************************************************/
void Timer::unpause()
{
    if (_start && _paused)
    {
        _paused = false;

        _startTicks = SDL_GetTicks() - _pausedTicks;
        _pausedTicks = 0;
    }
}

/******************************************************************************/
long long Timer::getTicks()
{
    long long Time = 0;

    if (_start)
    {
        if (_paused)
        {
            Time = _pausedTicks;
        }
        else
        {
            Time = SDL_GetTicks() - _startTicks;
        }
    }

    return Time;
}

/******************************************************************************/
void Timer::update()
{
    if (!isPaused())
    {
        _currentFrame++;
    }

    _lastFrameDuration = getTicks();
    _startTicks = SDL_GetTicks();

    if (_currentFrame > 1)
    {
        if (_averageFrameDuration <= 0)
        {
            _averageFrameDuration = _lastFrameDuration;
        }
        else
        {
            _averageFrameDuration *= 0.99;
            _averageFrameDuration += 0.01 * _startTicks;

            _FPS = 1000.0 / _averageFrameDuration;
        }
    }
}

/******************************************************************************/
long long Timer::getCurrentTicks()
{
    return _startTicks;
}

/******************************************************************************/
bool Timer::isStarted()
{
    return _start;
}

/******************************************************************************/
bool Timer::isPaused()
{
    return _paused;
}
}; //wind