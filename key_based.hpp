#ifndef KEY_BASED_HPP
#define KEY_BASED_HPP

#include<algorithm>
#include<numeric>
#include<type_traits>
#include<utility>
#include<vector>

namespace key_based
{
	
namespace {
	template<
		class InputIt,
		class KeyFunc,
		class KeyCompFunc=std::less<
			std::invoke_result_t<KeyFunc,typename std::iterator_traits<InputIt>::reference>
		>
	>
	struct key_compare_context
	{
		using key_type=std::invoke_result_t<KeyFunc,typename std::iterator_traits<InputIt>::reference>;
		std::vector<size_t> ids;
		std::vector<key_type> keys;
		const KeyFunc& kf;
		const KeyCompFunc& kcf;
		key_compare_context(InputIt be,InputIt ed,const KeyFunc& tkf,const KeyCompFunc& tkcf=KeyCompFunc()):
			ids(ed-be),
			keys(ed-be),
			kf(tkf),
			kcf(tkcf)
		{
			std::iota(ids.begin(),ids.end(),0);
			std::transform(be,ed,keys.begin(),kf);
		}
		auto comparator() const
		{
			return [this](size_t a,size_t b)-> bool { return this->kcf(this->keys[a],this->keys[b]); };
		}
	};
}

template<
	class InputOutputIt1,
	class KeyFunc,
	class KeyCompFunc=std::less<
		std::invoke_result_t<KeyFunc,typename std::iterator_traits<InputOutputIt1>::reference>
	>
>
void sort(InputOutputIt1 be,InputOutputIt1 ed,KeyFunc kf,KeyCompFunc kcf=KeyCompFunc())
{
	key_compare_context kcc(be,ed,kf,kcf);
	std::sort(kcc.ids.begin(),kcc.ids.end(),kcc.comparator());
	std::vector<typename std::iterator_traits<InputOutputIt1>::value_type> cpval(be,ed);
	for(size_t k:kcc.ids)
	{
		*be++ = cpval[k];
	}
}

template<
	class InputIt,
	class KeyFunc,
	class KeyCompFunc=std::less<
		std::invoke_result_t<KeyFunc,typename std::iterator_traits<InputIt>::reference>
	>
>
InputIt min_element(InputIt be,InputIt ed,KeyFunc kf,KeyCompFunc kcf=KeyCompFunc())
{
	key_compare_context kcc(be,ed,kf,kcf);
	return be+(std::min_element(kcc.ids.cbegin(),kcc.ids.cend(),kcc.comparator())-kcc.ids.cbegin());
}
template<
class InputIt,
class KeyFunc,
class KeyCompFunc=std::less<
std::invoke_result_t<KeyFunc,typename std::iterator_traits<InputIt>::reference>
>
>
InputIt max_element(InputIt be,InputIt ed,KeyFunc kf,KeyCompFunc kcf=KeyCompFunc())
{
	key_compare_context kcc(be,ed,kf,kcf);
	return be+(std::max_element(kcc.ids.cbegin(),kcc.ids.cend(),kcc.comparator())-kcc.ids.cbegin());
}
}
#endif
