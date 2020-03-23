#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <queue>

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase currentStatus = _messageQueue.receive();

        if (currentStatus == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device device;
    std::mt19937 mt_rand(device());
    std::uniform_int_distribution<> duration(4, 6);
    float cycleDuration = duration(mt_rand);
    std::chrono::time_point<std::chrono::system_clock> update_time;

    update_time = std::chrono::system_clock::now();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long measure_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - update_time).count();
        if (measure_time >= cycleDuration)
        {
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
            }
            else
            {
                _currentPhase == TrafficLightPhase::red;
            }
            _messageQueue.send(std::move(_currentPhase));
            update_time = std::chrono::system_clock::now();
        }
    }
}
