#ifndef EUCLIDEAN_HPP
#define EUCLIDEAN_HPP

#include<array>
#include<cstdint>
#include<cstdlib>
#include<iostream>

namespace 
{
    template<typename TupleOut,typename Func, std::size_t... I>
	static inline TupleOut tuple_index_apply_internal(Func f, std::index_sequence<I...>)
	{
		return TupleOut{f(I)...};
	}
	template<typename TupleOut,unsigned D,typename Func,typename Indices = std::make_index_sequence<D>>
	static inline TupleOut tuple_index_apply(Func f)
	{
		return tuple_index_apply_internal<TupleOut>(f,Indices{});
	}

    template<typename Func, std::size_t... I>
    static inline void index_apply_internal(Func f,std::index_sequence<I...>)
    {
        (...,f(I));
    }
    template<unsigned D,typename Func,typename Indices = std::make_index_sequence<D>>
	static inline void index_apply(Func f)
    {
        index_apply_internal(f,Indices{});
    }
}

template<class T,unsigned int D>
struct euclidean: public std::array<T,D>
{// Convert array into a tuple
private:
	
public:
	using std::array<T,D>::array;
	using std::array<T,D>::operator[];

    template<typename... Args>
    euclidean(Args&& ...args):
        std::array<T,D>{std::forward<Args>(args)...}
    {}
    
    template<class OT>
    euclidean& operator+=(const euclidean<OT,D>& o){
        ::index_apply<D>([this,o](size_t i){ 
            this->operator[](i)+=o[i];
        });
        return *this;
    }
    template<class OT>
    euclidean& operator-=(const euclidean<OT,D>& o){
        ::index_apply<D>([this,o](size_t i){ 
            this->operator[](i)-=o[i];
        });
        return *this;
    }
    template<class OT>
    euclidean& operator*=(OT o){
        ::index_apply<D>([this,o](size_t i){ 
            this->operator[](i)*=o;
        });
        return *this;
    }
    template<class OT>
    euclidean& operator/=(OT o){
        ::index_apply<D>([this,o](size_t i){ 
            this->operator[](i)/=o;
        });
        return *this;
    }

    template<class OT>
    euclidean<decltype(T{}+OT{}),D>
    operator+(const euclidean<OT,D>& o){
        return ::tuple_index_apply<
            euclidean<decltype(T{}+OT{}),D>
        ,D>([this,o](size_t i){ 
            return this->operator[](i)+o[i];
        });
    }
    template<class OT>
    euclidean<decltype(T{}-OT{}),D>
    operator-(const euclidean<OT,D>& o){
        return ::tuple_index_apply<
            euclidean<decltype(T{}+OT{}),D>
        ,D>([this,o](size_t i){ 
            return this->operator[](i)-o[i];
        });
    }
    template<class OT>
    euclidean<decltype(T{}*OT{}),D>
    operator*(OT o){
        return ::tuple_index_apply<
            euclidean<decltype(T{}*OT{}),D>
        ,D>([this,o](size_t i){ 
            return this->operator[](i)*o;
        });
    }
    template<class OT>
    euclidean<decltype(T{}/OT{}),D>
    operator/(OT o){
        return ::tuple_index_apply<
            euclidean<decltype(T{}/OT{}),D>
        ,D>([this,o](size_t i){ 
            return this->operator[](i)/o;
        });
    }
};

template<class T,unsigned int D>
std::ostream& operator<<(std::ostream& out,const euclidean<T,D>& v)
{
	out << "{";
    ::index_apply<D>([&out,&v](size_t i){
        out << v[i];
        if(i != (D-1)){
            out << ",";
        }
	});
    return out;
}

#endif

