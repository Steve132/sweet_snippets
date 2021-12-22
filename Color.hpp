#ifndef COLOR_HPP
#define COLOR_HPP

#include "Euclidean.hpp"

namespace 
{
    static inline float color_scalar_conv(uint8_t bv) { 
        return static_cast<float>(bv)/255.0f;
    }
    static inline uint8_t color_scalar_conv(float fv) {
        return static_cast<uint8_t>(fv*255.0f);
    }
}

template<class T,unsigned int D>
struct Color: public euclidean<T,D>
{// Convert array into a tuple
public:
	using euclidean<T,D>::operator[];
	
    using euclidean<T,D>::operator+;
    using euclidean<T,D>::operator-;
    using euclidean<T,D>::operator*;
    using euclidean<T,D>::operator/;
	using euclidean<T,D>::operator+=;
    using euclidean<T,D>::operator-=;
    using euclidean<T,D>::operator*=;
    using euclidean<T,D>::operator/=;
	
    template <typename... Args,
        typename=std::enable_if_t<
            (sizeof...(Args) != 1)
        >
    >
    Color(Args&&... args) : 
        euclidean<T,D>(std::forward<Args>(args)...) 
    {}
    
    Color(const euclidean<T,D>& c):
		euclidean<T,D>(c)
	{}

	template<class OT>
	Color(const euclidean<OT,D>& c):
    euclidean<T,D>(::tuple_index_apply<Color<T,D>,D>
            ([c](size_t i){
                return ::color_scalar_conv(c[i]);
            }))
    {}
};

using Color3b=Color<std::uint8_t,3>;
using Color4b=Color<std::uint8_t,4>;
using Color3f=Color<float,3>;
using Color4f=Color<float,4>;


inline Color3f pix2rgT(const Color3b& RGB)
{
	Color3f rgb(RGB);
	float r=rgb[0],g=rgb[1],b=rgb[2];
	float T=r+g+b;
	if(T==0.0f) return Color3f{0.0f,0.0f,0.0f};
    else return Color3f{r/T,g/T,T/3.0f};
}
inline Color3b rgT2pix(const Color3f& rgT)
{
	float r=rgT[0],g=rgT[1];
    float T=3.0f*rgT[2];
    float b=T-r-g;
	Color3f rgb{r*T,g*T,b*T};
	return Color3b(rgb);
}

#endif
