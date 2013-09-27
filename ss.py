def error(name,arg):
	return globals().get(name,type(name,(Exception,)))(arg)
