template<class Iterator, class KeyFunc>
void radix_internal(Iterator b, Iterator e, unsigned int logkp1, KeyFunc kf)
{
	size_t n = std::distance(b, e);
	if (n <= 1 || logkp1 == 0) return;
	unsigned int kmsk = 1UL << (logkp1-1);
	Iterator curiter = b;
	Iterator curb = b, cure = e - 1;
	for (size_t i = 0; i < n; i++)
	{
		if (kf(*curiter) & kmsk)
		{
			std::iter_swap(curiter, cure--);
			curiter = cure;
		}
		else
		{
			std::iter_swap(curiter, curb++);
			curiter = curb;
		}
	}
	radix_internal(b, curb, logkp1 - 1, kf);
	radix_internal(curb, e, logkp1 - 1, kf);
}
