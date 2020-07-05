//
// Created by blu on 7/4/2020.
//
#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>
#include <type_traits>

template<bool> struct Range;
template<typename T, int N = 0, typename = Range<true>>
class Channel
{
public:
    Channel() {
        m_capacity = N;
        std::cout << "Buffered channel constructed (" << m_capacity <<")\n";
    }
    virtual ~Channel() {
        if (is_open())
            close();
    };

protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

public:
    T receive()
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_cv.wait(the_lock, [this]
        {
            return !is_empty()|| !is_open();
        });

        if (!is_open())
            throw std::runtime_error("Channel closed while receiving.");

        T val = std::move(m_data.front());
        m_data.pop_front();
        m_cv.notify_all();
        return val;
    };
    void send(T&& val)
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_cv.wait(the_lock, [this]
        {
            return is_open() && has_capacity();
        });

        if (!is_open())
            throw std::runtime_error("Channel closed while sending.");

        m_data.emplace_back(val);
        m_cv.notify_all();
    };

    void close() {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_open = false;
    };

    bool is_open() const {
        return m_open;
    }

    bool is_empty() const {
        return m_data.empty();
    }

    bool has_capacity() const {
        return (m_data.size() < m_capacity);
    }

    size_t capacity() const {
        return m_capacity;
    }

    size_t size() const {
        return m_data.size();
    }


protected:
    std::deque<T> m_data;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_open {true};
    size_t m_capacity {3};
};


template<typename T, int N>
class Channel<T, N, Range<(N <= 0)> >
{
public:
    Channel() {
        std::cout << "Basic channel constructed\n";
    };
    virtual ~Channel() {};

protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

public:
    T receive()
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_has_receiver = true;
        m_cv.notify_all();

        m_cv.wait(the_lock, [this]
        {
            return (m_has_receiver && m_has_value);
        });

        m_has_value = false;
        m_has_receiver = false;
        return std::move(m_val);
    };
    void send(T&& val)
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_cv.wait(the_lock, [this]
        {
            return (m_has_receiver && !m_has_value);
        });
        m_val = val;
        m_has_value = true;
        m_cv.notify_all();
    };

    void close();

protected:
    T m_val;
    bool m_has_value {false};
    bool m_has_receiver {false};

    std::mutex m_mutex;
    std::condition_variable m_cv;
};

