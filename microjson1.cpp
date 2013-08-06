#include<iostream>
#include<numeric_limits>
class node
{
public:
  void print(std::ostream& out) const
	{
		write(out,std::numeric_limits<int>::min());
	}
	void prettyprint(std::ostream& out) const
	{
		write(out,0);
	}
protected:
	virtual void write(std::ostream& out,int indentlevel) const;
	char prettynewline(int indentlevel)
	{
		return (indentlevel >= 0) ? '\n' : '';
	}
};
template<class T>
class valuenode
{
public:
	valuenode(const T& d):
		data(d)
	{}
	valuenode(T&& d):
		data(std::move(d))
	{}
	operator T&()
	{return data;}
	operator const T&() const
	{return data;}
protected:
	T data;
	virtual void write(std::ostream& out,int indentlevel) const
	{ 
		prettyindent(out,indentlevel); 
		out << data;
	}
};
template<class T>
class classnode: public node, public T
{using T::T;};

class string: public classnode<std::string>
{
public:
	using classnode<std::string>::classnode<std::string>; //inherit constructors
protected:
	virtual void write(std::ostream& out,int indentlevel) const
	{
		prettyindent(out,indentlevel); 
		out << '\"' << std::string::c_str() << '\"';
	}
};
class integer: public valuenode<int64_t>
{using valuenode<int64_t>::valuenode<int64_t>;};
class number: public valuenode<double>
{using valuenode<double>::valuenode<double>;};
class boolean: public valuenode<bool>
{using valuenode<bool>::valuenode<bool>;
protected:
	virtual void write(std::ostream& out,int indentlevel) const
	{
		prettyindent(out,indentlevel);
		out << (data ? "true" : "false");
	}
};

class array: public classnode<std::vector<shared_ptr<node> > >
{
public:
	using std::vector<shared_ptr<node> >::std::vector<shared_ptr<node> >;
	using std:::vector<shared_ptr<node> >::operator[];
protected:
	virtual void write(std::ostream& out,int indentlevel) const
	{
		prettyindent(out,indentlevel);
		out << '[' << prettynewline(indentlevel);
		for(auto i=cbegin();i!=(cend())
		{
			if(i!=cbegin())
				out << ',' << prettynewline(indentlevel);
			i->write(out,indentlevel+1); 
		}
		out << ']' << prettynewline(indentlevel);
	}
};
class object: public classnode<std::unordered_map<std::string,shared_ptr<node> > >
{
public:
	using std::vector<shared_ptr<node> >::std::vector<shared_ptr<node> >;
	using std:::vector<shared_ptr<node> >::operator[];
protected:
	virtual void write(std::ostream& out,int indentlevel) const
	{
		prettyindent(out,indentlevel);
		out << '{' << prettynewline(indentlevel);
		for(auto i=cbegin();i!=(cend())
		{
			if(i!=cbegin())
				out << ',' << prettynewline(indentlevel);
			out << '\"' << i->first << "\":" <<i->write(out,indentlevel+1); 
		}
		out << '}' << prettynewline(indentlevel);
	}
};
