import numpy as np
import cvxpy as cvx
import random

n = 1000  #1000 serum samples
ps = 20	   #10 people are positive
ntests = 100  #we have 100 tests
ns= 10	   #each person deposits a sample into 10 tests

A = np.zeros((ntests,n))  #this matrix is the linear blend of the samples implemented by the pipette machine.  We have to build it.

odex=[1.0/ns]*ns+[0.0]*(ntests-ns) #make an incoherent mixing matrix [1 1 1 1 .... 0 0 0 0 0 0 ] representing which vials each person chooses
gtx=[1.0]*ps+[0.0]*(n-ps)	#make a sample popultionat [1 1 1 .... 0 0 0 0] representing our unknown population of positive and negative serum samples

random.shuffle(gtx)			#randomize the status of the serum samples (so our simulation has random +/- population)
gtxv=np.array([gtx]).T 	

for ni in range(n):			#randomize the selection matrix
	random.shuffle(odex)	#choose PS vials at random
	A[:,ni]=odex			

#this part is physical
gsamps=np.dot(A,gtxv)		#mix the samples into vials according to the selected (suboptimally) incoherent distribution
gsamps=np.clip(gsamps,0.0,1.0)	#perform the test.  The clipping simulates that a test can only be +/- not 2. 
gsamps=gsamps.reshape((ntests,))


#the results of the tests are recorded and entered to the computer in gsamps. 
#do the compressed sensing
vx = cvx.Variable(n)
objective = cvx.Minimize(cvx.norm(vx, 1))
constraints = [A @ vx >= gsamps]
prob = cvx.Problem(objective, constraints)
result = prob.solve(verbose=True)

z=vx.value #z is the predicted individual tests.

#output the results
print(z)
positive_labels=[]
detected_positive_labels=[]
for i in range(n):
	if((z[i] > .9)):
		detected_positive_labels.append(i)
	if((gtxv[i,0] > .9)):
		positive_labels.append(i)

print("Actual positive individuals:")
print(positive_labels)
print("Detected positive individuals:")
print(detected_positive_labels)
