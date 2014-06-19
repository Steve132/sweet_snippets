import hmac
import hashlib
from binascii import hexlify,unhexlify

def hmac_sha256(key,msg):
	return hmac.new(key,msg,hashlib.sha256).digest()

def pbkdf2_256(PRF,Password,Salt,c):
	#this assumes that hlen=dkLen...so don't concatenate
	U=PRF(Password,Salt)
	OUT=0
	for i in range(1,c):
		U=PRF(Password,U)
		Ui=int(hexlify(U),16)
		OUT ^= Ui
	return unhexlify("{:064X}".format(OUT))

import string
#replace .! with 01 if punctuation is false..64-character dictionary 
def makepassword(bytes,length,punctuation=False):
	character_dictionary=string.digits+string.ascii_letters+'01'
	
	intvalue=int(hexlify(bytes),16)
	outpw=''
	outpw+=string.ascii_lowercase[intvalue & 0xF]
	intvalue >>= 4
	outpw+=string.ascii_uppercase[intvalue & 0xF]
	intvalue >>= 4
	outpw+=string.digits[intvalue & 0x7]
	intvalue >>= 3
	for x in range(length-5):
		outpw+=character_dictionary[intvalue & 0x3F]
		intvalue >>= 6
	if(punctuation):
		outpw+='!'
	else:
		outpw+='d'
	outpw+=character_dictionary[intvalue & 0x3F]
	return outpw
	
def superpass(master,domain,username="",salt="",length=12,punctuation=False):
	if(length > int(256/6)):
		raise "Error, length is too large for this hash function"
	metasalt=bytearray(username+"|"+domain+"|"+salt,'utf-8')
	master=bytearray(master,'utf-8')
	bytes=pbkdf2_256(hmac_sha256,master,metasalt,(1 << 12))
	return makepassword(bytes,length,punctuation)
	
if __name__=='__main__':
	import getpass
	import argparse
	
	parser=argparse.ArgumentParser()
	parser.add_argument('--domain','-d',nargs=1,required=True,type=str,help='The domain you are accessing (like google.com)')
	parser.add_argument('--username','--user','-u',nargs=1,type=str,help='The username on the account')
	parser.add_argument('--salt','-s',nargs=1,type=str,help='A salt string or physical keycode')
	parser.add_argument('--length','-l',nargs=1,type=int,help='the length of password to generate (default: 12)')
	parser.add_argument('--punctuation','-p',action='store_true',help='if present, this password requires at least one punctuation')
	nsan=vars(parser.parse_args())
	if(nsan['punctuation']):
		nsan['punctuation']=[True]
	nsa=dict([(k,v[0]) for k,v in nsan.items() if v])
	print(nsa)
	mst=getpass.getpass("Master password:")
	print(superpass(mst,**nsa))
	
def canonical_domains():
	pass	#/use list of known domains->domain mappings
	pass	#/canonical wordlist..passwords MUST come from that list and MUST be >6 long...maybe...maybe not.
