#include<iostream>
#include<cstdint>
#include<functional>

//C++ was a mistake.
namespace detail{
    template<class SIG>
    using sfn=std::function<SIG>;

    enum class PropTag{
        get,
        set,
        init,
        deinit,
        move,
        move_init
    };

    template<class T,PropTag p>
    struct default_functions;

    template<class T>
    struct default_functions<T,PropTag::get>{
        using base=std::function<T ()>;
        T func(){  return T{}; }
    };
    template<class T>
    struct default_functions<T,PropTag::set>{
        using base=std::function<void (const T&)>;
        void func(const T&){}
    };
    template<class T>
    struct default_functions<T,PropTag::init>{
        using base=std::function<void (const T&)>;
        void func(const T&){}
    };
    template<class T>
    struct default_functions<T,PropTag::init>{
        using base=std::function<void (const T&)>;
        void func(const T&){}
    };

    template<class T,PropTag p>
    struct propfun: 
        public default_functions<T,p>::base
    {
        using base=typename default_functions<T,p>::base;
        template<class SIG>
        propfun(SIG&& f):base(std::forward<SIG>(f))
        {}

        propfun():base(default_functions<T,p>::func)
        {}
    };

    template<class T>
    struct property_init{
        propfun<T,PropTag::get> get;
        propfun<T,PropTag::set> set;

    };
};

template<class T>
struct property{
    property(const detail::property_init<T>& a)
    {}
};

struct foo{
    property<int> k{{
        .get=[this](){return 10;}
    }};
};


int main(int argc,char** argv){

    foo f;

    return 0;
}

