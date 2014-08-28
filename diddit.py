from google.appengine.ext import ndb
from google.appengine.ext.ndb import polymodel

class Dependency(ndb.Model):
	reader=ndb.KeyProperty(indexed=True,repeated=True)
	writer=ndb.KeyProperty(indexed=True,repeated=True)
	executor=ndb.KeyProperty(indexed=True,repeated=True)
	notes=ndb.JsonProperty()
	active=ndb.BooleanProperty(default=True,indexed=True)
	depends_on=ndb.KeyProperty(indexed=True,repeated=True)
	visible=ndb.BooleanProperty(default=True,indexed=True)
	taskclass=ndb.JsonProperty(indexed=True,default="False") 
	#status is the name of a member-function (js or python) to call and a set of arguments in json
	creation_timestamp=ndb.DateTimeProperty(auto_add=True)
	last_updated_timestamp=ndb.DateTimeProperty(auto_add_now=True)
	#comparator=ndb.StringProperty() #comparator is the js-function to use to compare.  Default topo-sort
	
class User(ndb.Model):
	groups=ndb.KeyProperty(indexed=True,repeated=True)
	
class Group(ndb.Model):
	name=ndb.StringProperty(indexed=True)
	parentgroup=ndb.KeyProperty(indexed=True)
	
