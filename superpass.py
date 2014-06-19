import hmac
import hashlib

def hmac_sha256(key,msg):
	return hmac.new(key,msg,hashlib.sha256).digest()

def pbkdf2_256(PRF,Password,Salt,c):
	#this assumes that hlen=dkLen...so don't concatenate
	U=PRF(Password,Salt)
	for i in range(1,c):
		U=PRF(Password,U)
	return U

from binascii import hexlify
import string
#replace .! with 01 if punctuation is false..64-character dictionary 
def makepassword(bytes,length,punctuation=False):
	character_dictionary=string.digits+string.ascii_letters
	if(punctuation):
		character_dictionary+='.!'
	else:
		character_dictionary+='01'
	intvalue=int(hexlify(bytes),16)
	outpw=''
	for x in range(length):
		outpw+=character_dictionary[intvalue & 0x3F]
		intvalue >>= 6
	return outpw
def superpass(master,domain,username="",salt="",length=10,punctuation=True):
	if(length > int(256/6)):
		raise "Error, length is too large for this hash function"
	metasalt=bytearray(username+"|"+domain+"|"+salt,'utf-8')
	master=bytearray(master,'utf-8')
	bytes=pbkdf2_256(hmac_sha256,master,metasalt,1 << 12)
	return makepassword(bytes,length,punctuation)
	
if __name__=='__main__':
	import getpass
	import argparse
	
	parser=argparse.ArgumentParser()
	parser.add_argument('--domain','-d',nargs=1,required=True,type=str)
	parser.add_argument('--username','--user','-u',nargs=1,type=str)
	parser.add_argument('--salt','-s',nargs=1,type=str)
	parser.add_argument('--length','-l',nargs=1,type=int)
	parser.add_argument('--punctuation','-p',action='store_true')
	nsan=vars(parser.parse_args())
	nsa=dict([(k,v[0]) for k,v in nsan.items() if v])
	print(nsa)
	mst=getpass.getpass("Master password:")
	print(superpass(mst,**nsa))
