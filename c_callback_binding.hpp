
#include <functional>
#include <type_traits>
#include <utility>

namespace {
template<class R,class... Args>
struct c_callback_impl
{
    typedef R (*fptr_t)(Args...,void* userdata);
    static R body(Args... args,void* udata){
        std::function<R(Args...)>& fref=*reinterpret_cast<std::function<R(Args...)>*>(udata);
        return fref(std::forward<Args>(args)...);
    }
};
}

template<class R,class... Args>
typename c_callback_impl<R,Args...>::fptr_t c_callback_function(std::function<R(Args...)>& fref)
{
    return c_callback_impl<R,Args...>::body;
}
template<class R,class... Args>
void* c_callback_userdata(std::function<R(Args...)>& fref)
{
    return reinterpret_cast<void*>(&fref);   
}
