#include<Eigen/Dense>
#include<unsupported/Eigen/FFT>
#include<cmath>

//Implementation of this paper
//https://arxiv.org/pdf/1202.6522.pdf

static const long double M_PI_LD=3.141592653589793238462643383279502884197169399375105820974944592307816406286208998L;
template<class RealType>
class rSHT
{
private:
	static size_t triangle_index_positive(size_t m,size_t n)
	{
		//n >= m, so triangle column offset is n-m....row offset is (m*(m+1))/2
		return (m*(m+1))/2 + n - m;
	}
	static array<size_t,size_t> triangle_unindex_positive(size_t i)
	{
		i=(m*(m+1))/2 + n - m
	}
	
	std::vector<long double> a;
	std::vector<long double> b;
	Eigen::FFT fft;
	std::vector<long double> vcos,vsin;
	std::vector<long double> vsinm; //should this be built or computed with expontentiation by squaring. 
	long double rowsize;
	
	void build_constants()
	{
		size_t Tn=triangle_index_positive(bands,bands);
		a.resize(Tn);
		b.resize(Tn);
		
		std::vector<long double> a2(bands);
		a2[0]=1.0L/(4.0L*M_PI_LD);
		for(size_t k=1;k<bands;k++)
		{
			long double p=k*2;
			long double p2=k*2+1;
			a2[k]=a2[k-1]*(p2/p);
		}

		#pragma omp parallel for
		for(size_t m=0;m<bands;m++)
		{
			//parallelize this one too?  Yes. But must implement constant value correctly.
			for(size_t n=m;n<bands;n++)
			{
				size_t tindex=triangle_index_positive(m,n);
				a[tindex]= (m==n) ? (sqrtl(a2[m])) : (sqrtl(static_cast<long double>(4*n*n-1)/static_cast<long double>(n*n-m*m)));
				size_t bnum=(2*n+1)*((n-1)*(n-1)-m*m);
				size_t bden=(2*n-3)*(n*n-m*m);
				b[tindex]=-sqrtl(static_cast<long double>(bnum)/static_cast<long double>(bden));
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
	
	
		//go ahead and parallize this one...it should be hot across the i axis...
		vsinm=std::vector<long double>(input_rows*bands,1.0L);
		rowsize=M_PI_LD/input_rows;
		#pragma omp parallel for
		for(size_t theta_i=0;theta_i < input_rows;theta_i++)
		{
			long double theta=rowsize*(static_cast<long_double>(theta_i)+0.5L);
			vcos[theta_i]=cosl(theta);
			long double stm=sinl(theta);
			vsin[theta_i]=stm;
			long double current_stm=1.0;
			for(size_t m=0;m<bands;m++)
			{
				vsinm[m*input_rows+theta_i]=current_stm;
				current_stm*=stm;
			}
		}

	}
	
	Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic> 
	fftMatrix(const Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& latlong)
	{
		size_t nfft=bands*2;
		size_t ftosize=nfft/2+1;
		size_t ffromsize=latlong.cols();
		
		Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> fftresult(latlong.rows(),ftosize);
		std::complex<RealType>* dop=fftresult.data();
		RealType* dip=latlong.data();
		
		#pragma omp parallel for
		for(size_t r=0;r<latlong.rows();r++)
		{
			fft.fwd(dop+r*sizeof(std::complex<RealType>)*ftosize,dip+r*sizeof(RealType)*ffromsize,nfft);
		}
		
		return fftresult; //possibly cast to column major version explicitly?
	}
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
		std::vector<std::complex<RealType>> data;
		
		const std::complex<RealType>& coeff(int m,int n) const
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
	
	SHT(size_t bds=1,size_t rows=0):bands(bds),num_coeffs(triangle_index_positive(bds,bds)),input_rows(rows)
	{
		fft.SetFlag(Eigen::FFT::Unscaled,Eigen::FFT::HalfSpectrum);
		build_constants();
	}
	
	//only works if row-major.
	void fwd(Coefficients& out,const Eigen::Matrix<RealType,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>& latlong)
	{
		Eigen::Matrix<std::complex<RealType>,Eigen::Dynamic,Eigen::Dynamic> f_m_theta=fftMatrix(latlong);
		out.data.fill(0);
		#pragma omp parallel for
		for(size_t m=0;m<bands;m++)
		{
			long double dtheta=rowsize;
			long double *vsinmrow;
			std::complex<RealType>* ncoeffs=&out.data[triangle_index_positive(m,m)];
			long double* alocal=&a[triangle_index_positive(m,m)];
			long double* blocal=&b[triangle_index_positive(m,m)];
			//this can also be parallel.
			for(size_t i=0;i<vcos.size();i++)
			{
				long double st=vsin[i];
				long double ct=vcos[i];
				long double slm=vsinmrow[i];
				long double Pm2=alocal[0]*slm*st*dtheta;
				ncoeffs[0]+=Pm2;
				long double Pm1;
				if(m+1 < bands)
				{
					Pm1=alocal[1]*Pm1*ct*dtheta;
					ncoeffs[1]+=Pm1;
				}
				//cannot be parallel...A and B should REALLY be interleaved for cache
				for(size_t noffset=2;noffset<(bands+m);noffset++)
				{
					long double Pc=alocal[noffset]*ct*Pc1+blocal[noffset]*Pc2;
					Pc2=Pc1;Pc1=Pc;
					ncoeffs[noffset]+=Pc;
				}
			}
		}
		return out;
	}
};
