#include<nlohmann/json.hpp>
//#include "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"

#include<tuple>


//https://stackoverflow.com/questions/22630832/get-argument-type-of-template-callable-object

namespace webview
{
namespace detail
{
template<class F>
struct ft;

template <class Tuple, std::size_t... I>
static inline Tuple internal_json_unpack(const nlohmann::json& js,std::index_sequence<I...>)
{
    Tuple t{
        js.at(I).get<
            std::tuple_element_t<I,Tuple>
            >()...};
    return t;
}

template<class R,class ...ArgType>
struct ft<R (ArgType...)>
{
    template<class FUNC>
    static auto build(FUNC&& func){
        auto rebindlambda = [&func](std::string args) -> std::string {
                nlohmann::json jsa=nlohmann::json::parse(args);
                using isequence=std::index_sequence_for<ArgType...>;
                using tuple_type=std::tuple<std::remove_reference_t<ArgType>...>;
                tuple_type tup=internal_json_unpack<tuple_type>(jsa,isequence{});
                auto result=std::apply(std::forward<FUNC>(func),tup);
                nlohmann::json jout = result;
                return jout.dump();
            };
    return rebindlambda;
    }
};

template<class R, class... Args>
struct ft<R(*)(Args...)> : public ft<R(Args...)>
{};
// member function pointer
template<class C, class R, class... Args>
struct ft<R(C::*)(Args...)> : public ft<R(C&,Args...)>
{};
// const member function pointer
template<class C, class R, class... Args>
struct ft<R(C::*)(Args...) const> : public ft<R(C&,Args...)>
{};
// member object pointer
template<class C, class R>
struct ft<R(C::*)> : public ft<R(C&)> {};

template<class F>
struct ft : public ft<decltype(&F::operator())> {
};

template<class F>
struct ft<F&> : public ft<F>
{};
template<class F>
struct ft<F&&> : public ft<F>
{};

}

template<class FUNC>
static inline auto jsrpc(FUNC&& f){
    return detail::ft<FUNC>::build(f);
}

}
