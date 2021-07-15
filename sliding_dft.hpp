
//http://www.music.mcgill.ca/~ich/research/misc/papers/cr1137.pdf

//Todo:Optimize for r2c with symmetry.

//add live convolution filter (by using td buffer to writeback most recent?)
//with convolutions this has to be euclidean.  CANT do sparse.  need a euclidean one and then convolution based on a preprocess and postprocess fd and td buffer

#include <cmath>


template<class REAL>
class sliding_dft
{
protected:
	std::vector<std::complex<REAL>> selected_unity_roots;
public:
	std::vector<std::complex<REAL>> fd;
	std::vector<std::complex<REAL>> td;
	
	sliding_fft(size_t tN,const std::vector<unsigned>& selected_frequencies):
		selected_unity_roots(selected_frequencies.size()),
		fd(selected_frequencies.size(),0.0),
		td(tN,0.0)
	{
		//all frequencies must be less than tN
		static const REAL pi_2 = static_cast<REAL>(2.0)*static_cast<REAL>(3.14159265358979323846264338327950288419716939937510L);
		REAL pi2_n=pi_2/(REAL)N;
		for(size_t i=0;i<selected_frequencies.size();i++)
		{
			REAL angle=pi2_n*static_cast<REAL>(selected_frequencies[i]);
			selected_unity_roots[i]={std::cos(angle),std::sin(angle)};
		}
	}
	void push_timedomain(const std::complex<REAL>& t)
	{
		std::complex<REAL> oldest=td[0];
		std::complex<REAL> newest=t;
		for(size_t i=0;i<fd.size();i++)
		{
			fd_next[i]=selected_unity_roots[i]*(fd[i]-oldest+newest);
		}
		std::copy(td.begin()+1,td.end(),td.begin());
		td.back()=newest;
	}
};
