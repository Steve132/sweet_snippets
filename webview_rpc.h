
/*
Copyright (c) 2022 Steve132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
    
*/

#ifndef WEBVIEW_RPC_H
#define WEBVIEW_RPC_H


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

#endif
