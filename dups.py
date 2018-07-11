#!/usr/bin/env python2
import sys,os,os.path
import hashlib

def digestfile(filename,chunksize=2**20):
	with open(filename,'rb') as fo:
		shaobj=hashlib.sha256()
		def readchunk():
			return fo.read(chunksize)
		for chunk in iter(readchunk,''):
			shaobj.update(chunk)
		return shaobj.hexdigest()


def getdigests(dirname):
	allfiles=[]
	for root, dirs, files in os.walk(dirname):
		for fn in files:
			fullname=os.path.join(root,fn)
			allfiles.append(fullname)
	sys.stderr.write("Scanning all %d files of %s...\n" % (len(allfiles),dirname))
	filehashes={}
	for i,fn in enumerate(allfiles):
		if(i % 1000 == 0):
			sys.stderr.write("Scanning files %d-%d (out of %d)...\n" % (i*1000,min((i+1)*1000,len(allfiles)),len(allfiles)))
		filehashes[fn]=digestfile(fn)
	return filehashes


def printdiffs(dir1,dir2):
	digests1=getdigests(dir1)
	digests2=getdigests(dir2)
	idigests2=set(digests2.values())
	sys.stderr.write("Printing %s items that are duplicated in %s to stdout" % (dir1,dir2))
	for fn,dg in digests1.items():
		if(dg in idigests2):
			print(fn)


if __name__=='__main__':
	if(len(sys.argv) < 3):
		sys.stderr.write("Usage: %s <dirA> <dirB> > duplicates.txt\n" % (sys.argv[0]))
		sys.stderr.write("Prints the files in <dirA> that are also in <dirB> to stdout.\n")

	else:
		printdiffs(sys.argv[1],sys.argv[2])
