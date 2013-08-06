namespace ujson
{
template<class T>
void write(ostream& out,const T& t,int l)
{
  indent(out,l);out << t;
}
template<>
void write(ostream& out,const string& t,int l)
{
	indent(out,l);out << '\"' << t << '\"';
}
template<>
void write(ostream& out,const bool& b,int l)
{
	indent(out,l);out << (b ? "true" : "false");
}
template<class T>
void write(ostream& out,const T* pt,int l)
{
	write(out,*pt,l);
}
template<class T>
void write(ostream& out,const shared_ptr<T>& pt,int l)
{
	write(out,*pt,l);
}
template<class T>
void write(ostream& out,const unique_ptr<T>& pt,int l)
{
	write(out,*pt,l);
}
template<class ListType>
void write(ostream& out,const ListType& t,int l)
{
	out << '[' << newline(l);
	for(auto i=t.cbegin();i!=(t.cend())
	{
		if(i!=cbegin())
		{
			out << ',' << newline(l);
		}
		out << write(out,*i,l+1); 
	}
	indent(out,l);
	out << ']' << newline(l);
}
template<class MapType>
 void write_map(std::ostream& out,const MapType& m,int indentlevel) const
{
	out << '{' << newline(indentlevel);
	for(auto i=m.cbegin();i!=(m.cend())
	{
		if(i!=cbegin())
		{
			out << ',' << newline(indentlevel);
		}
		indent(out,l+1);
		out << '\"' << i->first << "\":" <<write(out,i->second,indentlevel+1); 
	}
	indent(out,l);
	out << '}' << newline(indentlevel);
}
template<class T>
void write(std::ostream& out,const std::map<T>& m,int indentlevel) const
{
	write_map(out,m,indentlevel);
}
template<class T>
void write(std::ostream& out,const std::unordered_map<T>& m,int indentlevel) const
{
	write_map(out,m,indentlevel);
}

}

