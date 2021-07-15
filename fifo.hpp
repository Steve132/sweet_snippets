template<typename T>
class ringbuffer
{
protected:
	std::vector<T> data;
	size_t head;

public:
	ringbuffer(size_t N,const T& init={}):data(N,init),head(0)
	{}
	
	size_t size() const { return data.size(); }
	
	size_t buf2index(const size_t i) const
	{
		if(i < head) i+=data.size();
		return i-head;
	}
	size_t index2buf(const size_t i) const
	{
		i+=head;
		if(i >= data.size()) i-=data.size();
		return i;
	}
	T pushpop(const T& x)
	{
		T old=data[head];
		data[head++]=x;
		if(head == data.size()) head=0;
		return old;
	}
	T& operator[](size_t i)
	{
		return data[index2buf(i)];
	}
	const T& operator[](size_t i) const
	{
		return data[index2buf(i)];
	}
};
