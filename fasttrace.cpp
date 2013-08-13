#include<vector>
#include<iostream>
using namespace std;

struct ray
{
	Vector3d position;
	Vector3d direction;
};

Vector3b maxmask(Vector3d v)
{
	return max(max(v.xyz,v.yzx),v.zxy)==v.xyz;//chooses maximum dimension of intercept distance.
}	
Vector3ui next(Vector3ui current,unsigned int increment,const ray_traversal& r)
{
	Vector3ui wallincrement=increment*Vector3ui(r.direction > 0.0);//std::signbit
	Vector3ui wallcoordi=current+wallincrement; 
	Vector3d wallcoordf=wallcoordi.cast<double>(); //can we do it all in fixed point somehow?
	Vector3d intersectdist=abs(r.position-wallcoordf)/r.direction;
	return current+wallincrement*maxmask(intersectdist);
}

static uint64_t morton(uint32_t x,uint32_t y,uint32_t z); //normalized to range 0-1
struct Box
{
	Box(Vector3ui pos, //max of 21 bits or 0x1FFFFF
				uint32_t sz):
			morton(mcode),
			size(msz),
			mcode(morton(pos.x,pos.y,pos.z)),
			msz(sz)
	{}
				
	bool operator<(const Box& b)
	{
		return (mcode != b.mcode) ? (mcode < b.mcode) : (msz < b.msz); //maybe switch comparison direction?
	}
	const uint64_t& morton;
	const uint32_t& size

protected:
	uint64_t mcode;
	uint32_t msz;
};

//assume outside...haven't tested inside yet.
const Box* intersect(const ray& r,const std::set<Box>& scene) //if this is a multiset, then there can be more than one leaf to check...so scan through all possible..not for this tho
{
	const unsigned int maxsize=(0x1FFFFF+1);
	unsigned int currentsize=maxsize;
	Vector3ui currentposition(0,0,0);

	while(currentsize < maxsize*2)
	{
		Box curbox(currentposition,currentsize)
		auto data_lower=std::lower_bound(scene.begin(),scene.end(),curbox); //find to see if there is data inside the current box 
		auto data_upper=std::upper_bound(scene.begin(),scene.end(),Box(currentposition+1,currentsize)); //if data matches current prefix( (data that matches currentbox prefix))
																								     //in reality this can be done with one binary search if coded properly...note/
		if(data_lower == data_upper)  //if data does not exist with this prefix.
		{
			Vector3u nextposition=next(currentposition,currentsize,r);
			vector nextsizes=((nextposition ^ (nextposition-1) >> 1); //set trailing zeros to FFFF, 
			currentsize=(nextsizes.x | nextsizes.y | nextsizes.z)+1;  //bitwise or the trails for max, then increment this.  this is the new size.
		}
		else //if data exists
		{
			if((data_upper-data_lower)==1) //if there is exactly one element in this box.  
											//Alternately, if(*data_lower==curbox)..implying that the data level is the same so we have to check.
											//the correct thing to do here for poorly-formed data (leaves inside leaves or multiple leaves at size and level) is to scan through all the data that matches the prefix and level. (this could be done by switching to a while-loop here).
			{
				//if(collide_with_inside(r,data_lower)) 
					return &*data_lower;
			}
			else
			{
				currentsize>>1;//if you aren't moving the intersection point, then you need to change the currentposition here...this is a bug.
			}
		}
	}
	return nullptr;
}

int main()
{
	
	return 0;
}

static uint64_t morton(uint32_t x,uint32_t y,uint32_t z) //normalized to range 0-1
	{
		static const uint64_t mortmasks[5]={0x001F00000000FFFF,0x001F0000FF0000FF,0x100F00F00F00F00F,0x10C30C30C30C30C3,0x1249249249249249};
		uint64_t out=0;
		uint64_t pid[3]={x,y,z};
		for(unsigned int d=0;d<3;d++)
		{
			//pid[d]=(p[d]*0x1FFFFF) & 0x1FFFFF;
			//int ex;
			//frexp(min(precision,2.0),&ex);
			//pid[d]&=~(0x1FFFFF >> -ex);//cut based on precision
			for(unsigned int l=0;l<5;l++)
			{
				pid[d]=(pid[d] | (pid[d] << (32 >> l)) & mortmasks[l];
			}
			out |= pid[d] << d;
		}
		return out;
	}
