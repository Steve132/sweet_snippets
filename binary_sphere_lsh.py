import numpy as np

class binary_sphere_lsh(object):

	def __init__(self,num_bits=32,feature_size=64,orthnormalize=False,dtype=np.float64):
		self.planes=np.random.uniform(low=-1.0,high=1.0,size=(num_bits,feature_size)).astype(dtype)
		self.num_bits=num_bits
		self.feature_size=feature_size
	
	def buckets(self,x):
		x-=x.mean(axis=1,keepdims=True)
		
		result=np.dot(self.planes,self.x)
		bitwise=result > 0.0
		intbuckets=[]		
		for col in bitwise:
			bucketindex=0L
			for i,v in enumerate(col):
				if(v):
					bucketindex|=1L << i
			intbuckets.append(bucketindex)
		return intbuckets
			
class matchset(object):
	def __init__(self,masklength=None):
		self.mask=None
		self.matches={}
		if(masklength):
			self.mask=(1L << masklength)-1

	def add_matches(buckets,label):
		for i,b in enumerate(buckets):
			bid=b & self.mask if self.mask else b
			self.matches.setdefault(bid,[]).append((label,i))
	
"""
better LSH


multi-dimensional normal distribution is perfectly spherically symmetric..  Normalize to get distribution over an N ball, n-ball times 

https://www.mat.ulaval.ca/fileadmin/mat/documents/lrivest/Publications/77-LeonMasseRivest2006.pdf
http://www.sciencedirect.com/science/article/pii/S0047259X10001211

First, normalize with PCA, as in, mean-center the data, then find a find a low-dimensional orthornormal version of the mean-centered data.

Then do LSH projections and bitwise sampling.

sort all points by gray codes to minimize number of adjacent transitions.
"""
