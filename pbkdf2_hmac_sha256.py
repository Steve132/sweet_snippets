import hmac,hashlib
from binascii import unhexlify,hexlify

def pbkdf2_hmac_sha256(password,salt,c):
	def prf(pwd,nacl):
		return int(hmac.new(pwd,nacl,digestmod=hashlib.sha256).hexdigest(),16)
	U=prf(password,salt+'\x00\x00\x00\x01')
	T=0
	for i in range(c):
		T^=U
		U=prf(password,unhexlify("%064X" % (U)))
	return unhexlify("%064X" %(T))
	
if __name__=='__main__':
	from hashlib import pbkdf2_hmac
	import random
	for _ in range(20):
		v=str(random.getrandbits(128))
		iters=100000
		#theirs=pbkdf2_hmac('sha256',v,'salt',iters)
		mine=pbkdf2_hmac_sha256(v,'salt',iters)
		#print(hexlify(theirs))
		print(hexlify(mine))
		print("")
