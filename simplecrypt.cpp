#include<iostream>
#include<random>
#include<cstdint>
#include<cstdlib>
#include<iterator>
#include<algorithm>
using namespace std;

//simple key derivation function;
uint64_t skdf(const std::string& a)
{
	static const std::uint64_t hc[16]={
		0x21bd0bed5b30bd04,0x175208df9c63bb95,0x5bbd3b485350b328,0x4f38d4c2bcccc1f9,
		0xcea3fab1c600eb54,0xb558e7920e6af9df,0xfe2bb317c2c90ad7,0x1f13fe9da69f3d96,
		0x12ce0d26eebf774c,0xcfa70dd81a00f8a7,0x21e5101d0e458453,0xd19f22846ad8db25,
		0x612b1ff813b1493e,0x4dc1d0efa3e62ab7,0x525ba4978958cd4a,0x1ce87ecbbb10cafa
	};
	uint64_t hout=0;
	for(size_t i=0;i<a.size();i++)
	{
		uint_fast8_t ch=a[i];
		uint_fast8_t select=a[i] & 0xF;
		uint64_t val=hc[select];
		uint_fast8_t rot=(ch >> 2) & 0x3C + (i & 0x3F); //(ch >> 4)*4
		hout ^= (val >> rot) | (val << (64-rot)); 
	}
	return hout;
}

int main(int argc,char** argv)
{
	uniform_int_distribution<uint8_t> random_byte(0,0xFF);
	uint64_t key=0;
	for(int i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			char select=argv[i][1]=='-' ? argv[i][2] : argv[i][1];
			switch(select)
			{
			case 'k':
			case 'K':
				key=strtoull(argv[i+1],nullptr,0);
				if(key==0)
				{
					key=skdf(argv[i+1]);
				}
				break;
			};
		}		
	}
	mt19937_64 generator(key);
	transform(
		(istreambuf_iterator<char>(cin)),
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(cout),
		[&generator,&random_byte](const char& ch)
		{
			return static_cast<char>(random_byte(generator)) ^ ch;
		}                
	);
	return 0;
}
