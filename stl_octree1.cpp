/*...........................................KJIHGFEDCBA9876543210  
...........KJIHG................................FEDCBA9876543210		(x | x << 32) & 0x001F00000000FFFF
...........KJIHG................FEDCBA98................76543210		(x | x << 16) & 0x001F0000FF0000FF
...K........JIHG........FEDC........BA98........7654........3210		(x | x << 8)  & 0x100F00F00F00F00F
...K....JI....HG....FE....DC....BA....98....76....54....32....10		(x | x << 4)  & 0x10C30C30C30C30C3
...K..J..I..H..G..F..E..D..C..B..A..9..8..7..6..5..4..3..2..1..0		(x | x << 2)  & 0x1249249249249249*/


template<class Node>
uint64_t morton(const Node& n)
{
	static const uint64_t mortmasks[5]={0x001F00000000FFFF,0x001F0000FF0000FF,0x100F00F00F00F00F,0x10C3C3C3C3C3C3C3,0x1249249249249249};
	double p[3];
	uint64_t out=0;
	uint64_t pid[3];
	for(unsigned int d=0;d<3;d++)
	{
		p[d]=n.position(d); //must be defined
		pid[d]=(p[d]*0x1FFFFF) & 0x1FFFFF;
		for(unsigned int l=0;l<5;l++)
		{
			pid[d]=(pid[d] | (pid[d] << (32 >> l)) & mortmasks[l];
		}
		out |= pid[d] << d;
	}
	return out;
}
template<class Node>
bool treecomparator(const Node*& a,const Node*& b)
{
	uint64_t ipa=morton(*a),ipb=morton(*b);
	return (ipa == ipb) ? ipa < ipb : *a < *b;
}
template<class Node>
using octree=std::set<Node*,treecomparator<Node> > octset;
