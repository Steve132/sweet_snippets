/*
 * 
 * SampleWithoutReplacement.hpp
 * An implementation of generating samples without replacement from a set of items with discrete probability weights
 * From https://timvieira.github.io/blog/post/2019/09/16/algorithms-for-sampling-without-replacement/
 * 
 */

/*
 MIT License
 
 Copyright (c) 2020 Steven Braeger
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/


#ifndef SAMPLE_WITHOUT_REPLACEMENT_HPP
#define SAMPLE_WITHOUT_REPLACEMENT_HPP
//https://timvieira.github.io/blog/post/2019/09/16/algorithms-for-sampling-without-replacement/

#include <algorithm>
#include <vector>
#include <random>
#include <numeric>
#include <iterator>

namespace 
{
	struct sample_without_replacement_context
	{
		std::uniform_real_distribution<double> u01dist;
		std::vector<double> eparams;
		
		template<class WeightIterator,class RAIndexOutputIterator,class RandomEngine>
		sample_without_replacement_context(
			WeightIterator wbe,
			WeightIterator wed,
			RandomEngine& randengine,
			RAIndexOutputIterator out
		):
			u01dist(0.0,1.0),
			eparams(wbe,wed)
		{
			std::transform(eparams.begin(),eparams.end(),eparams.begin(),[&randengine,this](double w)
			{
				return -std::log(this->u01dist(randengine))/w;
			});
			std::iota(out,out+eparams.size(),0);
		}
	};
	
}

//https://timvieira.github.io/blog/post/2019/09/16/algorithms-for-sampling-without-replacement/

//O(n log k)
template<class WeightIterator,class RAIndexOutputIterator,class RandomEngine>
void ordered_sample_without_replacement_indices(
	WeightIterator wbe,
	WeightIterator wed,
	size_t cnt,
	RandomEngine& randengine,
	RAIndexOutputIterator out)
{
	sample_without_replacement_context swrc(wbe,wed,randengine,out);
	using odexref=typename std::iterator_traits<RAIndexOutputIterator>::reference;
	const double* epp=swrc.eparams.data();
	std::partial_sort(
		out,out+cnt,out+swrc.eparams.size(),
		[epp](odexref a,odexref b)
		{
			return epp[a] < epp[b];
		}
	);
}
//O(n)
template<class WeightIterator,class RAIndexOutputIterator,class RandomEngine>
void unordered_sample_without_replacement_indices(
	WeightIterator wbe,
	WeightIterator wed,
	size_t cnt,
	RandomEngine& randengine,
	RAIndexOutputIterator out)
{
	sample_without_replacement_context swrc(wbe,wed,randengine,out);
	using odexref=typename std::iterator_traits<RAIndexOutputIterator>::reference;
	const double* epp=swrc.eparams.data();
	std::nth_element(
		out,out+cnt,out+swrc.eparams.size(),
					  [epp](odexref a,odexref b)
					  {
						  return epp[a] < epp[b];
					  }
	);
}
#endif
