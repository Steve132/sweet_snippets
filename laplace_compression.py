

def laplaceinterp2(indices,fValues,dims,iterations=500):
	outputim=np.zeros(dims)
	outputim[indices]=fValues %set all known values
	gaussian
	for _ in range(iterations):
		outputim=scipy.ndimage.filters.gaussian_filter(outputim,1.0,mode='constant') %spread laplacian information
		outputim[indices]=fValues %Restore known values
	return outputim
	
def laplacecompress(image,compression_ratio=0.02):
	laplacian=scipy.ndimage.filters.laplace(image,mode='constant')
	laplacian=numpy.absolute(laplacian) %alternatively, do squared?
	indices=numpy.argsort(laplacian,axis=None,kind='mergesort')
	%coords=numpy.unravel_index(indices,image.dims)
	saved_indices=indices[:int(compression_ratio*len(indices))]
	return saved_indices,image[saved_indices]
	
def run_test(ground_truth,compression_ratio):
	si,v=laplacecompress(ground_truth,compression_ratio)
	out=laplaceinterp2(si,v,ground_truth.shape)
	diff=out-ground_truth
	errormsq=np.mean(diff*diff)
	return errormsq,out

if __name__=='__main__':
	image=scipy.ndimage.imread('lena.png',flatten=True)
	err,img=run_test(image,compression_ratio=0.02)
	print("Mean squared error is %f." % (err))
	scipy.ndimage.imwrite('lena-out.png',img)
