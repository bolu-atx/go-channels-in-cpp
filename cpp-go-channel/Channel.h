//
// Created by Bo Lu on 6/28/20.
//
#pragma once
#define VERSION2

#include <mutex>
#include <condition_variable>
#include <deque>

#ifdef VERSION1
template<class T>
class Channel {
public:
    Channel() {};
    virtual ~Channel() {};

protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

public:
    T receive_blocking()
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);

        m_value_update.wait(the_lock, [this]
            {
            return m_has_value;
        });

        return m_val;
    };
    void send(T&& val)
    {
        {
            std::unique_lock<std::mutex> the_lock(m_mutex);
            m_val = val;
            m_has_value = true;
        }
        m_value_update.notify_all();
    };

    void close();

protected:
    T m_val;
    bool m_has_value {false};

    std::mutex m_mutex;
    std::condition_variable m_value_update;
};
#endif


#ifdef VERSION2
template<class T>
class Channel {
public:
    Channel() {};
    virtual ~Channel() {};

protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

public:
    T receive_blocking()
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
#endif

#ifdef VERSION3
template<class T>
class Channel {
public:
    Channel() {};
    virtual ~Channel() {
        if (is_open())
            close();
    };

protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

public:
    T receive_blocking()
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_value_update.wait(the_lock, [this]
        {
            return !is_empty() || !is_open();
        });

        if (!is_open())
            throw std::runtime_error("Channel closed.");

        T val = std::move(m_data.front());
        m_data.pop_front();
        return val;
    };
    void send(T&& val)
    {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        if (is_open())
        {
            m_data.emplace_back(val);
            m_value_update.notify_all();
        }
    };

    void close() {
        std::unique_lock<std::mutex> the_lock(m_mutex);
        m_open = false;
    };

protected:
    bool is_open() const {
        return m_open;
    }

    bool is_empty() const {
        return m_data.empty();
    }
protected:
    std::deque<T> m_data;
    std::mutex m_mutex;
    std::condition_variable m_value_update;
    bool m_open {true};
};
#endif

