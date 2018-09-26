def topo_invert(depends):
	rdepends={}
	for m,deps in depends.items():
		for d in deps:
			rdepends.setdefault(d,set()).add(m)
	return rdepends

def topo(depends):
	rdepends=topo_invert(depends)
	L=[]
	unmarked=set(rdepends.keys()) | set(depends.keys())
	markedperm={}
	def visit(n):
		perm=markedperm.get(n,None)
		if(perm==True):
			return
		if(perm==False):
			raise Exception("The graph is not a dag")
		unmarked.discard(n)
		markedperm[n]=False
		for m in rdepends.get(n,[]):
			visit(m)
		markedperm[n]=True
		L.append(n)

	while(len(unmarked)):
		visit(unmarked.pop())

	return L[::-1]

if __name__=='__main__':
	test={'all':['binary','tests'],
		'binary':['binary.o','lib.a'],
		'binary.o':['binary.c'],
		'lib.a':['lib.c'],
		'tests':['binary','tests.o'],
		'tests.o':['tests.c']}
	print(topo(test))
		
