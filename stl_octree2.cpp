/*
...........................................KJIHGFEDCBA9876543210  
...........KJIHG................................FEDCBA9876543210		(x | x << 32) & 0x001F00000000FFFF
...........KJIHG................FEDCBA98................76543210		(x | x << 16) & 0x001F0000FF0000FF
...K........JIHG........FEDC........BA98........7654........3210		(x | x << 8)  & 0x100F00F00F00F00F
...K....JI....HG....FE....DC....BA....98....76....54....32....10		(x | x << 4)  & 0x10C3C3C3C3C3C3C3
...K..J..I..H..G..F..E..D..C..B..A..9..8..7..6..5..4..3..2..1..0		(x | x << 2)  & 0x1249249249249249*/

template<class Data>
class Node
{
public:
	static uint64_t morton(double x,double y,double z,double precision=0.0) //normalized to range 0-1
	{
		static const uint64_t mortmasks[5]={0x001F00000000FFFF,0x001F0000FF0000FF,0x100F00F00F00F00F,0x10C3C3C3C3C3C3C3,0x1249249249249249};
		uint64_t out=0;
		double p[3]={x,y,z};
		uint64_t pid[3];
		for(unsigned int d=0;d<3;d++)
		{
			pid[d]=(p[d]*0x1FFFFF) & 0x1FFFFF;
			int ex;
			frexp(min(precision,2.0),&ex);
			pid[d]&=~(0x1FFFFF >> -ex);//cut based on precision
			for(unsigned int l=0;l<5;l++)
			{
				pid[d]=(pid[d] | (pid[d] << (32 >> l)) & mortmasks[l];
			}
			out |= pid[d] << d;
		}
		return out;
	}
	const uint64_t& morton;
	Node(uint64_t mcode,shared_ptr<Data> dptr=nullptr):
		morton(m_morton),
		m_morton(mcode)
		data(dptr)
	{}
	Node(double x,double y,double z,shared_ptr<Data> dptr=nullptr):
		Node(morton(x,y,z),dptr)
	{}
	bool operator<(const Node& a,const Node& b)
	{
		return (a.m_morton != b.m_morton) ? 
			a.m_morton < b.m_morton : 
			((a && b) ? *a < *b : false);
	}
private:
	uint64_t m_morton;
};

template<class Data>
using octree=std::set<Node<Data>>;








/////////////////////////
///usagee

int main()
{
	octree<Renderable> testree;  //create an octree where each node has a position and an renderable poitner.
	
	testree.insert(Node<Renderable>(.05,.5,0.1,r)); //insert some nodes
	testree.insert(Node<Renderable>(.1,.5,.2,r));
	
	testree.find(Node<int>(0.05,.5,0.1)); //find the first node in the octree.
	
	//render all the elements within a given square boundary at a given precision.
	for(auto i=testree.lower_bound(Node<Renderable>::morton(0.1,0.1,0.1,0.125));
		i!=testree.upper_bound(Node<Renderable>::morton(0.3,0.3,0.3,0.125));
		++i)
	{
		i->data->render();
	}
	
	
	
}
