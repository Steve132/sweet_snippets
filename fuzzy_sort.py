import random

#fuzzy sort deals with a sorting algorithm where the underlying comparisons have a (less than 50%) chance of being wrong.
#https://www.reddit.com/r/algorithms/comments/901mvc/sorting_a_list_of_objects_with_a_binary/e2nqvog/?context=3

def fuzzy_sorted(orig_items,iters=None,key=lambda x:x):
	keyed_items=[key(o) for o in orig_items] #this is to keep python sorted(key=) semantics
	n=len(orig_items)
	if(not iters or iters <=0):
		iters=n.bit_length() #ilog(n) iterations 
	
	ranks=[0]*n
	
	for _ in range(iters):
		orig_locations=list(range(n))
		random.shuffle(orig_locations)
		orig_locations.sort(key=lambda d: keyed_items[d]) #sort the indices by using the fuzzy comparisons from key or the object
		for rank,location in enumerate(orig_locations):	#add the new rank to the score
			ranks[location]+=rank
	
	
	locations=list(range(n))
	locations.sort(key=lambda d: ranks[d])
	return [orig_items[d] for d in locations]


#todo: add cached version that doesn't redo any comparisons

if __name__=='__main__':
	import functools
	#TEST
	def make_flawedcmp(correctpercentage=0.8):
		def flawedcmp(a,b):
			random.seed((a,b))
			tv=random.random()
			cmpval=cmp(a,b)
			return -cmpval if (tv > correctpercentage) else cmpval
		return flawedcmp
	def rankdistfailure(intlist):
		rankdists=[abs(d-l) for l,d in enumerate(intlist)]
		return rankdists

	def test(n,m,cp):
		values=list(range(n))
		random.shuffle(values)
		fuzzy_sort=fuzzy_sorted(values,iters=m,key=functools.cmp_to_key(make_flawedcmp(cp)))  #simulated flawed cmp
		print(fuzzy_sort)
		
	def perftest(n,cp):
		values=list(range(n))
		random.shuffle(values)
		scores=[]
		for m in range(100):
			fuzzy_sort=fuzzy_sorted(values,iters=m,key=functools.cmp_to_key(make_flawedcmp(cp)))
			avgscore=float(sum(rankdistfailure(fuzzy_sort)))/float(n)
			print(avgscore)
			scores.append(avgscore)
		return scores

	print(perftest(1250,0.80))
		
