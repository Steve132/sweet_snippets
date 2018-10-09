from pure_pynacl import nacl
from hashlib import pbkdf2_hmac
import os

#https://nacl.cr.yp.to/secretbox.html

def kdf(pw,iters=1000):
	dk = hashlib.pbkdf2_hmac('sha512', pw, b'pycoffer',iters)
	return dk[:32]
	
crypto_secretbox_ZEROBYTES=32
crypto_secretbox_BOXZEROBYTES=16
crypto_secretbox_NONCEBYTES=24
def encrypt(message,key):
	nonce=os.urandom(crypto_secretbox_NONCEBYTES)
	padded_message="\x00"*crypto_secretbox_ZEROBYTES + message
	c=b"\x00"*len(padded_message)
	result=crypto_secretbox_xsalsa20poly1305_tweet(c,padded_message,len(padded_message),nonce,key)
	if(result != 0):
		raise Exception("Encryption Error")
	return c[crypto_secretbox_BOXZEROBYTES:],nonce
	
def decrypt(encrypted,nonce,key):
	#crypto_secretbox_xsalsa20poly1305_tweet_open(c,padded_message,len(padded_message),nonce,key)
	#if(result != 0):
		#raise Exception("Decryption Error")
	pass
	
def test(message,password):
	key=kdf(password)
	nonce,cipher=encrypt(message,key)
	outmessage=decrypt(cipher,nonce,key)
	if(outmessage != message):
		raise Exception("Decryption or Encryption Encoding problem Error!")
