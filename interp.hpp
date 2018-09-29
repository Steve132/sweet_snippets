/*
Copyright 2018 Steven Braeger

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef INTERP_HPP
#define INTERP_CPP

#include<Eigen/Dense>
#include<Eigen/BDCSVD>

class LagrangeInterpolator
{
protected:
	Eigen::MatrixXd build_exponents(size_t max_exponent,size_t xdimensions)
	{	
		std::vector<size_t> current_exponent_scratch(xdimensions);
		std::vector<std::vector<size_t>> output_list;
		size_t maxtop=1;
		for(size_t k=0;k<xdimensions;k++)
		{
			maxtop*=max_exponent;
		}
		for(size_t ei=0;ei<maxtop;ei++)
		{
			current_exponent_scratch.resize(xdimensions,0);
			size_t cei=ei;
			size_t total=0;
			for(size_t k=0;k<xdimensions;k++)
			{
				size_t remainder=cei % max_exponent;
				current_exponent_scratch[k]=remainder;
				total+=remainder;
				cei/=max_exponent;
				if(cei == 0) break;
			}
			if(total <= max_exponent)
			{
				output_list.emplace_back(current_exponent_scratch);
			}
		}
		
		Eigen::MatrixXd exponents(xdimensions,output_list.size()); //exponent at dimension R for position col C
		for(size_t c=0;c<exponents.cols();c++)
		{
			const vector<double> srcrows&=output_list[c];
			for(size_t r=0;r<exponents.rows();r++)
			{
				exponents(r,c)=srcrows[r];
			}
		}
		return exponents;
	}

	Eigen::MatrixXd evaluate_terms(const Eigen::MatrixXd& locations)
	{
		return (locations.transposed().array().log().matrix()*exponents).array().exp(); //each row is [1 x x^2 ....]
	}
	Eigen::MatrixXd exponents;
	Eigen::MatrixXd coefficients;
	
public:
	
	//locations, one per column, 
	LagrangeInterpolator(const Eigen::MatrixXd& xlocations,const Eigen::MatrixXd& flocations,size_t max_exponent)
	{
		exponents=build_exponents(max_exponent,xlocations.rows());
		
		Eigen::MatrixXd A(xlocations.cols(),exponents.cols());
		A=evaluate_terms(xlocations,exponents);
		Eigen::BDCSVD svd(A,Eigen::ComputeThinU | Eigen::ComputeThinV);
		coefficients=svd.solve(flocations.transposed());		//A\B=X,AX=B
	}
	Eigen::MatrixXd evaluate(const Eigen::MatrixXd& xlocations)
	{
		return (evaluate_terms(xlocations,exponents)*coefficients).transposed();
	}
};
#endif
