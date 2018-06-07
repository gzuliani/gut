#ifndef GUT_UTILS_H
#define GUT_UTILS_H

namespace gut {

// static flag, initially reset; declare an instance to set it
template<class T>
class StaticFlag {
    static bool flag_;
public:
    StaticFlag() {
        flag_ = true;
    }
    static bool enabled() {
        return flag_;
    }
};

template<class T>
bool StaticFlag<T>::flag_ = false;

} // namespace gut

#endif // GUT_UTILS_H
