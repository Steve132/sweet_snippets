from pure_pynacl import tweetnacl
from hashlib import pbkdf2_hmac
import os

#https://nacl.cr.yp.to/secretbox.html

def kdf(pw,iters=100000):
	dk = pbkdf2_hmac('sha512', pw, b'pycoffer',iters)
	return dk[:32]
	
crypto_secretbox_ZEROBYTES=32
crypto_secretbox_BOXZEROBYTES=16
crypto_secretbox_NONCEBYTES=24
def encrypt(message,key):
	message=bytearray(message)
	nonce=os.urandom(crypto_secretbox_NONCEBYTES)
	padded_message=b"\x00"*crypto_secretbox_ZEROBYTES + message
	c=b"\x00"*len(padded_message)
	cInt = tweetnacl.IntArray(tweetnacl.u8, size=len(c), init=b"\x00"*len(padded_message))
	padded_messageInt = tweetnacl.IntArray(tweetnacl.u8, size=len(padded_message), init=padded_message)
	nonceInt = tweetnacl.IntArray(tweetnacl.u8, size=len(nonce), init=nonce)
	keyInt = tweetnacl.IntArray(tweetnacl.u8, size=len(key), init=key)
	result=tweetnacl.crypto_secretbox_xsalsa20poly1305_tweet(cInt,padded_messageInt,len(padded_message),nonceInt,keyInt)
	if(result != 0):
		raise Exception("Encryption Error")
	return bytes(bytearray(cInt[crypto_secretbox_BOXZEROBYTES:])),bytes(bytearray(nonceInt))
	
def decrypt(encrypted,nonce,key):
	encryptedInt= tweetnacl.IntArray(tweetnacl.u8,size=len(encrypted),init=encrypted)
	nonceInt=tweetnacl.IntArray(tweetnacl.u8,size=len(nonce),init=nonce)
	encryptedPadding = tweetnacl.IntArray(tweetnacl.u8, size=crypto_secretbox_BOXZEROBYTES, init=b"\x00"*crypto_secretbox_BOXZEROBYTES)
	encryptedInt = encryptedPadding + encryptedInt
	m=b"\x00"*len(encryptedInt)
	mInt = tweetnacl.IntArray(tweetnacl.u8, size=len(m), init=b"\x00"*len(encryptedInt))
	keyInt = tweetnacl.IntArray(tweetnacl.u8, size=len(key), init=key)
	result=tweetnacl.crypto_secretbox_xsalsa20poly1305_tweet_open(mInt, encryptedInt,len(encryptedInt),nonceInt,keyInt)
	if(result != 0):
		raise Exception("Decryption Error")
	return bytes(bytearray(mInt[crypto_secretbox_ZEROBYTES:]))
	
def test(message,password):
	key=kdf(password)
	cipher,nonce=encrypt(message,key)
	print(bytearray(messageInt))
	print("cipher:", cipher)
	print("type(cipher):", type(cipher))
	print("nonce:", nonce)
	outmessage=decrypt(cipher,nonce,key)
	print("Outmessage:", outmessage)
	# convert message to intarray as test:
	print("message:", message)
	if(outmessage != message):
		raise Exception("Decryption or Encryption Encoding problem Error!")

if __name__ == "__main__":
	test(b"I'm a super secret message!", b"password")
