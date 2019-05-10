#ifndef CIMG_WRAPPER_H
#define CIMG_WRAPPER_H

#include<string>
#include<memory>

namespace cimg_library
{

template<class T>
struct CImg;

};

namespace cimg_wrapper
{
template<class T>
class CImgWrapper
{
protected:
	std::shared_ptr<cimg_library::CImg<T> > imgptr;
public:
	CImgWrapper(size_t w, size_t h, size_t c, const T* data = nullptr,bool deinterleave=true);
	CImgWrapper(const cimg_library::CImg<T>& idata);
	CImgWrapper();

	std::size_t width() const;
	std::size_t height() const;
	std::size_t channels() const;
	
	T& operator()(size_t w, size_t h, size_t c);
	const T& operator()(size_t w, size_t h, size_t c) const;

	void save(const std::string& fn) const;
	void to_array(T* data) const;
	void fill(const T& val);
	void display() const;
};
extern template class CImgWrapper<float>;
extern template class CImgWrapper<unsigned char>;
extern template class CImgWrapper<double>;
}

#endif
