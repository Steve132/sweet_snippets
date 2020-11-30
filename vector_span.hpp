#ifndef VECTOR_SPAN_HPP
#define VECTOR_SPAN_HPP

#include<span>
#include<memory>

template<class T>
class vector_span: public std::span<T,std::dynamic_extent>
{
protected:
	size_t cap;
	using base=std::span<T,std::dynamic_extent>;
	void resize(size_t N)
	{
		base::operator=(base(base::data(),N));
	}
public:
	using element_type=typename base::element_type;
	using value_type=typename base::value_type;
	using size_type=typename base::size_type;
	using difference_type=typename base::difference_type;
	using pointer=typename base::pointer;
	using const_pointer=typename base::const_pointer;
	using reference=typename base::reference;
	using const_reference=typename base::const_reference;
	using iterator=typename base::iterator;
	using reverse_iterator=typename base::reverse_iterator;

	template<typename ...Args>
	vector_span(Args&&... a):base(std::forward<Args>(a)...){
		cap=base::size();
		//resize(0);
	}

	using base::operator[];

	size_t capacity() const { return cap; }

	void push_back(const T& t)	//there's no error handling on purpose to mirror pop_back()
	{
		resize(base::size()+1);
		base::back()=t; //this is honestly a little iffy. It might genuinely be better to use copy instead of placement new because the underlying array might be initialized already
	}
	template<typename ...Args>
	reference emplace_back(Args&&... a)
	{
		resize(base::size()+1);
		base::back()=T(std::forward(a)...);
		return base::back();
	}

	void pop_back()
	{
		if(!base::empty())
		{
			resize(base::size()-1);
			//std::destroy_at(base::data()+base::size()); //placement new can cause weirdness
		}
	}


private:
	using base::last;
	using base::first;
	using base::subspan; //these don't make sense for a fifo really
};

#endif  //VECTOR_SPAN_HPP
