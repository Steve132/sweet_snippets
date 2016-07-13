import math

def _ustr(u):
	s=""
	for n,v in u.items():
		if(v!=0):
			if(v==1):
				s+="%s " % (n)
			else:
				s+="%s^%d " % (n,v)
	return s

class UnitsIncompatibleError(Exception):
	def __init__(self,v1,v2):
		self.u1=u1
		self.u2=u2
		
	def __str__(self):
		return 'Units incompatible: "%s" and "%s"' % (str(self.u1),str(self.u2))

class Quantity(object):
	def __init__(self,_value=1.0,**kwargs):
		self.units={k: int(v) for k,v in kwargs.items() if int(v) != 0}
		self.value=_value
	def __str__(self):
		return str(self.value)+' '+_ustr(self.units)
	def __repr__(self):
		return str(self.value)+' '+_ustr(self.units)
		
	def __additive_op__(self,other,op):
		if(not isinstance(other,Quantity) or len(other.units)==0):
			other=Quantity(other,**self.units)
		if(self.units != other.units):
			raise UnitsIncompatibleError(self,other)
		return Quantity(op(self.value,other.value),**self.units)
	def __add__(self, other):
		return self.__additive_op__(other,lambda x,y: x+y)
	def __sub__(self, other):
		return self.__additive_op__(other,lambda x,y: x-y)
	def __radd__(self,other):
		return self.__additive_op__(other,lambda x,y: y+x)
	def __rsub__(self,other):
		return self.__additive_op__(other,lambda x,y: y-x)
	def __iadd__(self,other):
		r=self.__add__(other)
		self.value=r.value
		return self
	def __isub__(self,other):
		r=self.__sub__(other)
		self.value=r.value
		return self
	def __isub__(self,other):
		object.__neg__(self)
	def __neg__(self):
		return Quantity(-self.value,**self.units)
	def __pos__(self):
		return Quantity(+self.value,**self.units)
	def __abs__(self):
		return Quantity(abs(self.value),**self.units)
	
	def __multiplicative_op__(self,other,op,logop):
		if(not isinstance(other,Quantity)):
			other=Quantity(other)
		newunits={}
		for k in (self.units.viewkeys() | other.units.viewkeys()):
			newunits[k]=logop(self.units.get(k,0.0),other.units.get(k,0.0))	
		return Quantity(op(self.value,other.value),**newunits)
		
	def __mul__(self,other):
		return self.__multiplicative_op__(other,lambda x,y: x*y,lambda x,y:x+y)
	def __div__(self,other):
		return self.__multiplicative_op__(other,lambda x,y: x/y,lambda x,y:x-y)
	def __rmul__(self,other):
		return self.__multiplicative_op__(other,lambda x,y: y*x,lambda x,y:y+x)
	def __rdiv__(self,other):
		return self.__multiplicative_op__(other,lambda x,y: y/x,lambda x,y:y-x)
	def __imul__(self,other):
		r=self.__add__(other)
		self.value=r.value
		self.units=r.units
		return self
	def __idiv__(self,other):
		r=self.__sub__(other)
		self.value=r.value
		self.units=r.units
		return self
		
	def __pow_op__(self,other,logop):
		try:
			ov=other.value
		except:
			ov=other
		newunits={}
		for k,v in self.units.items():
			newunits[k]=logop(v,ov)	
		return Quantity(self.value**ov,**newunits)
		
	def __pow__(self,other):
		return self.__pow_op__(other,lambda x,y: x*y)
	def __ipow__(self,other):
		r=self.__pow__(other)
		self.value=r.value
		self.units=r.units
		return self
		
	def in_(self,other):
		if(not isinstance(other,Quantity) or len(other.units)==0):
			other=Quantity(other,**self.units)
		if(self.units != other.units):
			raise UnitsIncompatibleError(self,other)
		return self.value/other.value

#base+ units
meter=Quantity(m=1)
kilogram=Quantity(kg=1)
second=Quantity(s=1)
ampere=Quantity(A=1)
kelvin=Quantity(K=1)
mole=Quantity(mol=1)
candela=Quantity(cd=1)
bit=Quantity(b=1)

#derived units
hertz=1.0/second
radian=Quantity()
steradian=Quantity()
newton=kilogram*meter/(second*second)
pascal=newton/(meter*meter)
joule=newton*meter
watt=joule/second
coulomb=second*ampere
volt=watt/ampere
farad=coulomb/volt
ohm=volt/ampere
siemens=1.0/ohm
weber=joule/ampere
tesla=volt*second/(meter*meter)
henry=volt*second/ampere
lumen=candela*steradian
lux=lumen/(meter*meter)
becquerel=1.0/second
gray=joule/kilogram
sievert=joule/kilogram
katal=mole/second

#Constants
speed_of_light=299792458.0*meter/second
magnetic_constant=4.0*math.pi*1e-7*newton/(ampere*ampere)
electric_constant=1.0/(magnetic_constant*speed_of_light*speed_of_light)
gravitational_constant=6.6740831*1e-11*meter*meter*meter/(kilogram*second*second)
planck_constant=6.2607004081*1e-34*joule*second
planck_bar_constant=planck_constant/(4.0*math.pi)
elementary_charge=1.602176620898*1e-19*coulomb
magnetic_flux_quantum=planck_constant/(2.0*elementary_charge)
conductance_quantum=2.0*elementary_charge*elementary_charge/planck_constant
electron_mass=9.1093835611*1e-31*kilogram
proton_mass=1.67262189821*1e-27*kilogram
proton_electron_mass_ratio=proton_mass/electron_mass
fine_structure_constant1=elementary_charge*elementary_charge/(4.0*math.pi*electric_constant*planck_bar_constant*speed_of_light)  #doesn't this just equal
fine_structure_constant2=elementary_charge*elementary_charge/(electric_constant*planck_constant*speed_of_light)
fine_structure_constant3=speed_of_light*elementary_charge*elementary_charge*magnetic_constant/(planck_constant)
fine_structure_constant=fine_structure_constant1
inverse_fine_structure_constant=1.0/fine_structure_constant
rydberg_constant=fine_structure_constant*fine_structure_constant*electron_mass*speed_of_light/(2.0*planck_constant)
avogadro_constant=6.02214085774*1e23/mole
faraday_constant=elementary_charge*avogadro_constant
molar_gas_constant=8.314459848*(joule/mole)/kelvin
boltzmann_constant=molar_gas_constant/avogadro_constant
stefan_boltzmann_constant=5.67036713*1e-8*watt/(meter*meter*kelvin*kelvin*kelvin*kelvin)
#http://arxiv.org/pdf/1507.07956.pdf

#conversion units
minute=60.0*second
hour=60.0*minute
day=24.0*hour
week=7.0*day
year=365.25*day

electron_volt=1.602176620898*joule
byte=8.0 * bit
nibble=4.0 * bit
word=16 * bit
dword=32 * bit
qword=64 * bit
 
#prefixes
def _pre(scale):
	def fun(q):
		return q*(scale)
	return fun

yotta=_pre(1e24)
zetta=_pre(1e21)
exa=_pre(1e18)
peta=_pre(1e15)
tera=_pre(1e12)
giga=_pre(1e9)
mega=_pre(1e6) 
kilo=_pre(1e3)
hecto=_pre(1e2)
deca=_pre(1e1)
deci=_pre(1e-1)
centi=_pre(1e-2)
milli=_pre(1e-3)
micro=_pre(1e-6)
nano=_pre(1e-9) 
pico =_pre(1e-12)
femto=_pre(1e-15)
atto=_pre(1e-18)
zepto=_pre(1e-21)
yocto=_pre(1e-24)

#binary_prefixes
kibi=_pre(1<<10)
mebi=_pre(1<<20)
gibi=_pre(1<<30)
tebi=_pre(1<<40)
pebi=_pre(1<<50)
exbi=_pre(1<<60)
zebi=_pre(1<<70)
yobi=_pre(1<<80)

#imperial
thou=25.4*micro(meter)
inch=1000.0*thou
foot=12*inch
yard=3*foot
chain=22*yard
furlong=10*chain
mile=8*furlong
league=3*mile
fathom=6.08*foot
cable=100*fathom
nautical_mile=10*cable
link=7.92*inch
rod=25*link

perch=rod*rod
rood=furlong*rod
acre=furlong*chain

fluid_ounce=1.7339*inch**3
gill=5*fluid_ounce
pint=20*fluid_ounce
quart=40*fluid_ounce
gallon=160*fluid_ounce

pound=0.45359237*kilogram
grain=pound/7000.0
drachm=pound/256.0
ounce=pound/16.0
stone=14*pound
quarter=28*pound
hundredweight=100*pound
english_hundredweight=112*pound
ton=20*hundredweight
english_ton=20*english_hundredweight


