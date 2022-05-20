import shelve
import hashlib
import json
import tempfile,os.path

#https://gist.github.com/tkaemming/2431447

def hashfunc(f):
	return hashlib.sha256(f.__code__.co_code).hexdigest()

def hashcall(f,args,kwargs):
	r=json.dumps({'func':hashfunc(f),'args':args,'kwargs':kwargs},sort_keys=True)
	return hashlib.sha256(r.encode('utf-8')).hexdigest()

def cacheable(f,backend={}):
	def cachefunc(*args,**kwargs):
		key=hashcall(f,args,kwargs)
		result=backend.get(key,None)
		if(result is None):
			result=f(*args,**kwargs)
			backend[key]=result
		return result
	return cachefunc


shelves={}
def persistent(f,filename=None):
	global shelves
	if(filename is None):
		filename=os.path.join(tempfile.gettempdir(),"cachify_shelve")
	rname=os.path.abspath(filename)

	db=shelves.get(rname,None)
	if(db is None): 
		db=shelve.open(filename,writeback=True)
		shelves[rname]=db
	return cacheable(f,backend=db)
