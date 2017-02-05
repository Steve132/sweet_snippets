#include<Eigen/Dense>
#include<unsupported/Eigen/FFT>
#include<cmath>
#include<iostream>

//Implementation of this paper
//https://arxiv.org/pdf/1202.6522.pdf

template<class RealType,class HighPrecisionReal=long double>
class rSHT
{
private:
	static const constexpr HighPrecisionReal M_PI_LD=3.141592653589793238462643383279502884197169399375105820974944592307816406286208998L;

	static size_t triangle_index_positive(size_t m,size_t n)
	{
		//n >= m, so triangle column offset is n-m....row offset is (m*(m+1))/2
		return (m*(m+1))/2 + n - m;
	}
	/*static array<size_t,size_t> triangle_unindex_positive(size_t i)
	{
		i=(m*(m+1))/2 + n - m
	}*/
	
	std::vector<HighPrecisionReal> ab;
	Eigen::FFT<RealType> fft;
	std::vector<HighPrecisionReal> vcos,vsin;
	std::vector<HighPrecisionReal> vsinmoct; //should this be built or computed with expontentiation by squaring. 
	HighPrecisionReal rowsize;
	
	void build_constants()
	{
		size_t Tn=triangle_index_positive(bands,bands);
		ab.resize(2*Tn);
		
		std::vector<HighPrecisionReal> a2(bands);
		a2[0]=1.0L/(4.0L*M_PI_LD);
		//a2[0]/=static_cast<HighPrecisionReal>(4*bands*bands);
		std::cerr << "constant 0" << std::endl;

		for(size_t k=1;k<bands;k++)
		{
			HighPrecisionReal p=k*2;
			HighPrecisionReal p2=k*2+1;
			a2[k]=a2[k-1]*(p2/p);
		}
		std::cerr << "constant 1" << std::endl;

		#pragma omp parallel for
		for(size_t m=0;m<bands;m++)
		{
			//parallelize this one too?  Yes. But must implement constant value correctly.
			for(size_t n=m;n<bands;n++)
			{
				size_t tindex=triangle_index_positive(m,n);
				HighPrecisionReal a= (m==n) ? (sqrt(a2[m])) : (sqrt(static_cast<HighPrecisionReal>(4*n*n-1)/static_cast<HighPrecisionReal>(n*n-m*m)));
				size_t bnum=(2*n+1)*((n-1)*(n-1)-m*m);
				size_t bden=(2*n-3)*(n*n-m*m);
				HighPrecisionReal b=(n-m < 1) ? 0.0L : -sqrt(static_cast<HighPrecisionReal>(bnum)/static_cast<HighPrecisionReal>(bden));
				ab[2*tindex]=a;
				ab[2*tindex+1]=b;
				
				//Notes (obsolete):
				//https://www.wolframalpha.com/input/?i=product+from+k%3D1+to+m+of+(2k%2B1)%2F(2k)
				//is it faster or more accurate to compute this in a DP fashion instead of this						
				//these notes are obsolete.
				//this version of A does not include 1/sqrt(2 pi) because Eigen's FFT includes it.
				//a_mm = sqrt(2 gamma(m+1.5)/(sqrt(pi) gamma(m+1.0))
				//la_mm = .5 log(2.0 / sqrt(pi)) + .5 lgamma(m+1.5) - .5 lgamma(m+1.0) LGAMMA IS NOT THREAD SAFE!!
				//cooool https://www.wolframalpha.com/input/?i=(gamma(x%2B.5)%2Fgamma(x))%5E2+from+2+to+2000
				//gamma(x+c)/gamma(x)=x^c for x in natural numbers > 0?
			}
		}
	
		std::cerr << "constant 2" << std::endl;

		//go ahead and parallize this one...it should be hot across the i axis...
		vsinmoct.resize(input_rows*bands,1.0L);
		vcos.resize(input_rows);
		vsin.resize(input_rows);
		rowsize=M_PI_LD/input_rows;
		#pragma omp parallel for
		for(size_t theta_i=0;theta_i < input_rows;theta_i++)
		{
			HighPrecisionReal theta=rowsize*(static_cast<HighPrecisionReal>(theta_i)+0.5L);
			HighPrecisionReal ct=cos(theta);
			HighPrecisionReal st=sin(theta);
			vcos[theta_i]=ct;
			vsin[theta_i]=st;
			HighPrecisionReal current_stm=1.0;
			for(size_t m=0;m<bands;m++)
			{
				vsinmoct[m*input_rows+theta_i]=current_stm/ct;
				current_stm*=st;
			}
		}
		std::cerr << "constant 3" << std::endl;

	}
	
	Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> 
	fftMatrix(const Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& latlong)
	{
		size_t nfft=bands*2;
		size_t ftosize=nfft/2+1;
		size_t ffromsize=latlong.cols();
		std::cerr << "fft 0" << std::endl;

		Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> fftresult(latlong.rows(),ftosize);
		std::complex<RealType>* dop=fftresult.data();
		const RealType* dip=latlong.data();
		
		#pragma omp parallel for
		for(size_t r=0;r<latlong.rows();r++)
		{		
			//std::cerr << "fft 0.:" << r << std::endl;
			fft.fwd(dop+r*ftosize,dip+r*ffromsize,nfft);
 			
			//std::cerr << "fft 0." << r << ' ' << *(dop+r*ftosize) << std::endl;
		}
		
		std::cerr << "fft 1" << std::endl;

		
		return fftresult;
	}
		
	void ifftMatrix(Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& latlong,
		const Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& freqdomain)
	{
		size_t nfft=bands*2;
		size_t ffromsize=nfft/2+1;
		size_t ftosize=bands*2;
		
		std::cerr << "ifft 0" << std::endl;

		const std::complex<RealType>* dip=freqdomain.data();
		RealType* dop=latlong.data();
		
		#pragma omp parallel for
		for(size_t r=0;r<latlong.rows();r++)
		{
			fft.inv(dop+r*ftosize,dip+r*ffromsize,nfft);
		}
	}
	
	class PBandIteratorBuilder
	{
	public:
		size_t fb_index;
		HighPrecisionReal dtheta;
		const HighPrecisionReal* vsinmoctrow;
		const HighPrecisionReal* ablocal;
		
		PBandIteratorBuilder(const rSHT& parent,size_t m):
			fb_index(triangle_index_positive(m,m)),
			dtheta(parent.rowsize),
			vsinmoctrow(&parent.vsinmoct[m*parent.input_rows]),
			ablocal(&parent.ab[2*fb_index])
		{}
	};
	class PBandIterator
	{
	private:
		HighPrecisionReal slmoct;
	
		HighPrecisionReal Pm2;
		HighPrecisionReal Pm1;
		const HighPrecisionReal* ablocal;
	public:
		HighPrecisionReal st;
		HighPrecisionReal ct;
		size_t niters;
		size_t noffset;
		
		PBandIterator(const rSHT& parent,const PBandIteratorBuilder& pbuilder,size_t m,size_t i):
			st(parent.vsin[i]),
			ct(parent.vcos[i]),
			slmoct(pbuilder.vsinmoctrow[i]),
			Pm2(0.0),
			Pm1(slmoct),
			ablocal(pbuilder.ablocal),
			niters(parent.bands-m),
			noffset(0)
		{}
		HighPrecisionReal next()
		{
			//Pc[0] needs to be alocal[0]*slm..which is right as long as b[0],b[1] is 0
			//Pc[1] needs to be alocal[1]*ct*Pc[0]
			//Pc[x] = alocal[x]*ct*Pc[x-1]+b[x]*Pc[x-2]
			//ablocal is cache coherent now.
			size_t dex=2*(noffset++);
			HighPrecisionReal Pc=ablocal[dex]*ct*Pm1+ablocal[dex+1]*Pm2;
			Pm2=Pm1;Pm1=Pc;
			return Pc;
		}
		operator bool() const
		{
			return noffset < niters;
		}
		
	};
public: 

	class Coefficients
	{
	private:
		/*class writeback_writer
		{
		private:
			std::complex<RealType>& ref;
			bool negtracker;
		public:
			writeback_writer(bool neg,std::complex<RealType>& r):negtracker(neg),ref(r)
			{}
		};*/
	public:
		size_t bands;
		std::vector<std::complex<RealType> > data;
		
		std::complex<RealType> operator()(int m,int n) const
		{
			int am=(m < 0) ? -m : m;
			if(n >= bands || n > am || n < 0)
			{
				throw std::runtime_error("coefficient access is out of bounds");
			}
			size_t i=triangle_index_positive(am,n);
			if(m < 0)
			{
				return conj(data[i]);
			}
			else
			{
				return data[i];
			}
		}
		//std::complex<RealType>& coeff(int m,int n)
		//{} TODO: add writeback that correctly conjes new write
	};

	const size_t bands;
	const size_t num_coeffs;
	const size_t input_rows;
	
	rSHT(size_t bds=1,size_t rows=0):
		bands(bds),
		num_coeffs(triangle_index_positive(bds,bds)),
		input_rows(rows)
	{
		//fft.SetFlag(Eigen::FFT<RealType>::Unscaled ); //there's a dangling 1/sqrt(2 pi) that needs to be applied.
		//fft.SetFlag(Eigen::FFT<RealType>::Un
		fft.SetFlag(Eigen::FFT<RealType>::HalfSpectrum );
		build_constants();
	}
	
	//only works if row-major.
	void fwd(Coefficients& out,const Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& latlong)
	{
		std::cerr << "fwd 0" << std::endl;

		Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic> f_m_theta=fftMatrix(latlong);
		out.data.assign(num_coeffs,std::complex<RealType>(0.0,0.0));
		
		out.bands=bands;
		std::cerr << "fwd 1" << std::endl;
		
		RealType fftscal=2*bands;
		fftscal=1.0/fftscal;
		//fftscal=1.0;
		

		#pragma omp parallel for
		for(size_t m=0;m<bands;m++)
		{
			PBandIteratorBuilder piterbuilder(*this,m);
			std::complex<RealType>* ncoeffs=&out.data[piterbuilder.fb_index];
			
			
			//this can also be parallel...but you have to have an atomic sum around the writeback, which is, of course, a pain.
			for(size_t theta_i=0;theta_i<vcos.size();theta_i++)
			{
				std::complex<RealType> sample=f_m_theta(theta_i,m)*fftscal;
				//std::cerr << sample << std::endl;
				PBandIterator piter(*this,piterbuilder,m,theta_i);
			
				//cannot be parallel...
				while(piter)
				{
					size_t noffset=piter.noffset;
					double P=piter.next();
					
					if(m==0 && noffset==0)
					{
						//std::cerr << "fwd 1.:(" << m << "," << theta_i << "," << noffset << ")=" << P << std::endl;
					}
					//std::cerr << (ncoeffs-out.data.data()) << ',' << noffset << std::endl;
					ncoeffs[noffset]+=static_cast<RealType>(P*piter.st*piterbuilder.dtheta)*sample;
				}
			}
		}
		std::cerr << "fwd 2" << std::endl;
	}
	
	void inv(Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& out,const Coefficients& coeffs)
	{
		std::cerr << "inv 0" << std::endl;

		Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> f_m_theta(input_rows,bands);
		
		RealType ifftscal=1.0/(2*bands);
		
		
#ifndef SHT_DOUBLE_PARALLEL_INV
		#pragma omp parallel for
		for(size_t m=0;m<bands;m++)
		{
			PBandIteratorBuilder piterbuilder(*this,m);
			const std::complex<RealType>* clocal=&coeffs.data[piterbuilder.fb_index];
			
			//this can also be parallel...make it 2d?
			for(size_t theta_i=0;theta_i<vcos.size();theta_i++)
			{
#else
		#pragma omp parallel for
		size_t tim=bands*input_rows;
		for(size_t im_index=0;im_index<tim
		{
			size_t theta_i=im_index % input_rows;
			size_t m=im_index/input_rows;
			
			PBandIteratorBuilder piterbuilder(*this,m);
			const std::complex<RealType>* clocal=&coeffs.data[piterbuilder.fb_index];
			{
#endif
				//std::cerr << "inv 0.:" << std::endl;

				PBandIterator piter(*this,piterbuilder,m,theta_i);
				std::complex<RealType> sumvalue(0.0,0.0);
				
				//cannot be parallel...
				while(piter)
				{
					size_t noffset=piter.noffset;
					sumvalue+=clocal[noffset]*static_cast<RealType>(piter.next());
				}
				f_m_theta(theta_i,m)=sumvalue*ifftscal;
			}
		}
		std::cerr << "inv 1" << std::endl;

		ifftMatrix(out,f_m_theta);
	}
};
