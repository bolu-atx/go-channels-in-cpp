//
// Created by Bo Lu on 6/28/20.
//
#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>

template<typename T>
class Channel {
public:
    Channel() { }
    Channel(size_t capacity)
    {
        m_capacity = capacity;
    };
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

