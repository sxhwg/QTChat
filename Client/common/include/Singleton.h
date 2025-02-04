#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T> &) = delete;
    Singleton& operator=(const Singleton<T> &) = delete;

    static std::shared_ptr<T> m_instance;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            m_instance = std::shared_ptr<T>(new T());
        });
        return m_instance;
    }

    void PrintAddress() {
        std::cout << m_instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::m_instance = nullptr;

#endif // SINGLETON_H
