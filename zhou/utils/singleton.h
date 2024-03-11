#ifndef __ZHOU_SINGLETON_H__
#define __ZHOU_SINGLETON_H__

namespace zhou {

// 单例模式
template<typename T>
class Singleton {
public:
    Singleton() {
    }
    static std::shared_ptr<T> GetInstance() {
        static const std::shared_ptr<T> m_instance(new T);
        return m_instance;
    }
private:
};


}

#endif// ! __ZHOU_SINGLETON_H__
