import numpy as np
import cv2 as cv

def holefill(origimg,knowns,iterations=100):
	nhr,nhc=knowns.nonzero()
	img=np.zeros(shape=origimg.shape)
	img[nhr,nhc,:]=origimg[nhr,nhc,:]
	show(img/256.0)
	for k in range(iterations):
		img = cv.GaussianBlur(img,(3,3),0)
		img[nhr,nhc,:]=origimg[nhr,nhc,:]
		print(k)
	show(img/256.0)
	
lena = cv.imread('Lenna.png')
N=512*512
r=np.random.random_integers(0,511,N)
c=np.random.random_integers(0,511,N)
msk=np.ones(lena.shape,np.uint8)
msk=msk[:,:,0]
msk[r,c]=0

holefill(lena,msk)
