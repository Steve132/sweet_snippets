#pragma once
#include<cstdint>

namespace delegation{

template<class Parent,class Derived>
const Parent& enclosing(const Derived* d) {
    size_t off=Parent::offset(std::remove_cv_t<Derived*>{});
    const std::uint8_t* toff=reinterpret_cast<const std::uint8_t*>(d)-off;
    return *reinterpret_cast<const Parent*>(toff);
}
template<class Parent,class Derived>
Parent& enclosing(Derived* d){
    constexpr size_t off=Parent::offset(std::remove_cv_t<Derived*>{});
    constexpr std::uint8_t* toff=reinterpret_cast<std::uint8_t*>(d)-off;
    return reinterpret_cast<Parent*>(toff);
}

}

#define DECLARE_DELEGATE(Parent,name) \
        friend const Parent& delegation::enclosing<Parent,decltype(name)>(const decltype(name)*); \
        friend Parent& delegation::enclosing<Parent,decltype(name)>(decltype(name)*); \
        static constexpr auto offset(decltype(name)*){ \
            return offsetof(Parent,name);  \
        } \

#ifdef DELEGATION_EXAMPLE
class Parent{
public:
    int a;

    struct {
        operator int() const {
            return delegation::enclosing<Parent>(this).a;
        }
    } a_delegate;
    
    private:
        DECLARE_DELEGATE(Parent,a_delegate);
};

int main() {
    Parent p;
    std::cout << p.a_delegate << std::endl;
    
    return 0;
}
#endif
