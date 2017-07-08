import hmac,hashlib
from binascii import unhexlify,hexlify

def pbkdf2_hmac(password,salt,c,digestmod=hashlib.sha256):
	digest_size=digestmod().digest_size
	formatstring="%0"+str(2*digest_size)+"X"
	def prf(pwd,nacl):
		return int(hmac.new(pwd,nacl,digestmod=digestmod).hexdigest(),16)
	U=prf(password,salt+'\x00\x00\x00\x01')
	T=0
	for i in range(c):
		T^=U
		U=prf(password,unhexlify(formatstring % (U)))
	return unhexlify(formatstring % (T))

def pbkdf2_hmac_sha256(password,salt,c):
	return pbkdf2_hmac(password,salt,c,digestmod=hashlib.sha256)
	
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
