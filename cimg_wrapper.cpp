#include "cimg_wrapper.hpp"
#include<CImg.h>
#include<iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef _MSC_VER
#define STBI_MSC_SECURE_CRT
#endif
#include<stb_image_write.h>

namespace cimg_wrapper
{

template<class T>
CImgWrapper<T>::CImgWrapper(size_t w, size_t h, size_t c, const T* data, bool deinterleave)
{
	if (data)
	{
		if (deinterleave)
		{
			imgptr.reset(new cimg_library::CImg<T>(data, (unsigned int)c, (unsigned int)w, (unsigned int)h, 1));
			imgptr->permute_axes("yzcx");
		}
		else
		{
			imgptr.reset(new cimg_library::CImg<T>(data, (unsigned int)w, (unsigned int)h, 1, (unsigned int)c));
		}
	}
	else
	{
		imgptr.reset(new cimg_library::CImg<T>((unsigned int)w, (unsigned int)h, 1, (unsigned int)c));
	}
}
template<class T>
CImgWrapper<T>::CImgWrapper()
{}
template<class T>
CImgWrapper<T>::CImgWrapper(const cimg_library::CImg<T>& idata):
	imgptr(new cimg_library::CImg<T>(idata))
{}
template<class T>
std::size_t CImgWrapper<T>::width() const
{
	return imgptr->width();
}
template<class T>
std::size_t CImgWrapper<T>::height() const
{
	return imgptr->height();
}
template<class T>
std::size_t CImgWrapper<T>::channels() const
{
	return imgptr->spectrum();
}

template<class T>
T& CImgWrapper<T>::operator()(size_t w, size_t h, size_t c)
{
	return imgptr->operator()((unsigned int)w, (unsigned int)h,0, (unsigned int)c);
}
template<class T>
const T& CImgWrapper<T>::operator()(size_t w, size_t h, size_t c) const
{
	return imgptr->operator()((unsigned int)w, (unsigned int)h, 0, (unsigned int)c);
}
template<class T>
void CImgWrapper<T>::save(const std::string& fn) const
{
	//imgptr->save(fn.c_str());
	std::string ext = fn.substr(fn.size()-4);
	//std::cerr << "Stuff" << ext;     
//	
//	int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
//	int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
//	int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);
//	int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
	
	if (ext == ".hdr" || ext==".HDR")
	{
		cimg_library::CImg<float> imgout = *imgptr;
		imgout.permute_axes("cxyz");
		stbi_write_hdr(fn.c_str(), imgptr->width(), imgptr->height(), imgptr->spectrum(), imgout.data());
	}
	else if (
		ext == ".png" || ext == ".PNG" || 
		ext==".bmp" || ext==".BMP" || 
		ext==".jpg" || ext==".JPG" || ext==".JPEG" || ext==".jpeg"||
		ext==".tga" || ext==".TGA")
	{
		cimg_library::CImg<unsigned char> imgout = *imgptr;
		imgout.permute_axes("cxyz");
		const char* fnc = fn.c_str(); int w = imgptr->width(); int h = imgptr->height(); int c = imgptr->spectrum();
		const unsigned char* dt = imgout.data();

		switch (ext[1])
		{
		case 'p':
		case 'P':
			stbi_write_png(fnc, w, h, c, dt, 0); break;
		case 'b':
		case 'B':
			stbi_write_bmp(fnc, w, h, c, dt); break;
		case 'j':
		case 'J':
			stbi_write_jpg(fnc, w, h, c, dt, 8); break;
		case 't':
		case 'T':
			stbi_write_tga(fnc, w, h, c, dt); break;
		};
	}
	else
	{
		imgptr->save(fn.c_str());
	}
}
template<class T>
void CImgWrapper<T>::to_array(T* dout) const
{
	cimg_library::CImg<T> d = *imgptr;
	// (pdata, data.width(), data.height(), 1, data.spectrum(), true);
	d.permute_axes("cxyz");
	std::memcpy(dout, d.data(), d.size() * sizeof(T));
}
template<class T>
void CImgWrapper<T>::display() const
{
	imgptr->display();
}
template<class T>
void CImgWrapper<T>::fill(const T& v)
{
	imgptr->fill(v);
}

template class CImgWrapper<float>;
template class CImgWrapper<unsigned char>;
template class CImgWrapper<double>;

}
