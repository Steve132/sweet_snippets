#ifndef COLOR_TRANSFORM_HPP
#define COLOR_TRANSFORM_HPP

#include<nd/ndarray.hpp>
#include<vector>
#include<array>
#include "Image2D.hpp"
#include "Color.hpp"

class ColorTransform
{
public:
	nd::Array<uint8_t,4> lut;
	
	template<class Func>
	ColorTransform(Func f);
	
	ColorTransform();
	
	void apply_in_place(Image2D& img) const;
	void apply(Image2D& imgout,const Image2D& imgin) const;
	void apply(uint32_t* pdata32_out,const uint32_t* pdata32_in,size_t N) const;
	void apply_in_place(uint32_t* pdata32,size_t N) const;
};

template<class Func> 
inline ColorTransform::ColorTransform(Func f):
lut(nd::Shape<4>{256,256,256,4})
{	
	for(size_t r=0;r<256;r++)
	for(size_t g=0;g<256;g++)
	for(size_t b=0;b<256;b++)
	{
		Color3b RGB{(uint8_t)r,(uint8_t)g,(uint8_t)b};
		Color4b pv=f(RGB);
		for(int c=0;c<4;c++) lut(b,g,r,c)=pv[c];
	}
}


#endif
