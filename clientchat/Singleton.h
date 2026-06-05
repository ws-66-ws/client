#ifndef SINGLETON_H
#define SINGLETON_H
#include <mutex>
#include <memory>
#include <iostream>

template<typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T> &) = delete;
    Singleton &operator = (const Singleton<T> &st) = delete;

    static std::shared_ptr<T> _instance;

public:
    static std::shared_ptr<T> getInstance() {
        static std::once_flag s_flag;
        // std::call_once函数接受一个std::once_flag对象和一个可调用对象（如函数或lambda表达式）作为参数。
        // 如果std::once_flag对象表明该函数尚未被调用，std::call_once将执行该可调用对象，
        // 并将std::once_flag对象标记为已调用。
        std::call_once(s_flag, [&](){
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void printAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H
