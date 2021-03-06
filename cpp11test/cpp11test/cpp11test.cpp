#include "pch.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <deque>

#include <cstdlib>
#include <ostream>

using namespace std;

void testLambda()
{
	char s[] = "Hello World!";

	int Uppercase = 0; //modified by the lambda
	std::for_each(s, s + sizeof(s), [&Uppercase](char c) {
		if (isupper(c))
			Uppercase++; 
	});

	std::cout << Uppercase << " uppercase letters in: " << s << std::endl;
}

void testThread()
{
	std::vector<int> v = { 1, 2, 3, 4 };
	int acm = 0;
	std::mutex numberMutex;

	std::thread t1([&acm, &v, &numberMutex] {
		for (unsigned int i = 0; i < v.size() / 2; ++i)
		{
			lock_guard<mutex> lock(numberMutex);
			acm += v[i];
		}
	});
	std::thread t2([&acm, &v, &numberMutex] {
		for (unsigned int i = v.size() / 2; i < v.size(); ++i)
		{
			lock_guard<mutex> lock(numberMutex);
			acm += v[i];
		}
	});
	t1.join();
	t2.join();

	std::cout << "acm: " << acm << endl;
}

void testMoveSemanthics()
{
	class Foo
	{
		int size;
		char *buffer;
	public:
		Foo()
			: size(0), buffer(nullptr)
		{
			std::cout << "CTOR Foo" << std::endl;
		}

		~Foo()
		{
			if (buffer)
				delete buffer;

			std::cout << "DCTOR Foo" << std::endl;
		}

		Foo(Foo &other)
			: size(0), buffer(nullptr)
		{
			size = other.size;
			buffer = new char[size];
			strcpy_s(buffer, size, other.buffer);

			std::cout << "COPY CTOR Foo" << std::endl;
		}

		Foo(Foo &&other)
			: size(0), buffer(nullptr)
		{
			size = other.size;
			buffer = other.buffer;

			other.size = 0;
			other.buffer = nullptr;

			std::cout << "MOVE CTOR Foo" << std::endl;
		}

		Foo& operator=(Foo &other)
		{
			if (this != &other)
			{
				size = 0;
				delete[] buffer;

				size = other.size;
				buffer = new char[size];
				strcpy_s(buffer, size, other.buffer);
			}

			std::cout << "COPY assignment" << std::endl;
			return *this;
		}

		Foo& operator=(Foo &&other)
		{
			if (this != &other)
			{
				size = 0;
				delete[] buffer;

				size = other.size;
				buffer = other.buffer;

				other.size = 0;
				other.buffer = nullptr;
			}

			std::cout << "MOVE assignment" << std::endl;
			return *this;
		}

		Foo(int size_, char *buffer_)
		{
			size = size_;
			buffer = new char[size];
			strcpy_s(buffer, size, buffer_);
		}
	};

	std::string s = "Bla";

	Foo a( (int)s.size() + 1, (char *)s.c_str() );
	Foo b( a );
	a = b;
	b = Foo();
}

void testMoveSemanthics2()
{
	class Base
	{
	public:
		Base(int&& a)
		{
			a_ = a;
		}

		int a_;
	};

	class Son : public Base
	{
	public:
		Son(int&& a) : 
			Base(std::move(a))
		{
		}
	};

	Base base(4);
	Son son(6);

	cout << base.a_;
	cout << son.a_;
}

class Cena
{
public:
	Cena()
	{
		coisa = 123;
		std::cout << "CTR cena" << std::endl;
	}

	~Cena()
	{
		std::cout << "DCTR cena" << std::endl;
	}

	int coisa;
};

std::ostream &operator << (std::ostream &os, Cena const &obj)
{
	return os << obj.coisa;
}

void testSmartPointers()
{
	std::unique_ptr<Cena> p = std::unique_ptr<Cena>(new Cena);
		
	std::cout << *p << std::endl;

	std::shared_ptr<int> a = std::make_shared<int>(42);
	std::vector< std::shared_ptr<int> > v;
	v.push_back(a);

	std::cout << a << "->" << *a << std::endl;
}

template <typename T> class Base {
public:
	void Execute()
	{
		Execute_something();
	}
	void Execute_something()
	{
		static_cast<T *>(this)->Execute_something();
	}
};

class Derived : public Base<Derived> {
public:
	void Execute_something()
	{
		std::cout << "Derived Execute_something()\n";
	}
};

void testPolimorphism()
{
	Derived d;
	d.Execute();
}


template<typename T>
void print(T value)
{
	cout << value << endl;
}

class Calculator
{
public:
	Calculator() : value(0){}

	int Execute() {

		std::thread a(&Calculator::ExecutePart, this, 0, 10);
		std::thread b(&Calculator::ExecutePart, this, 10, 20);
		a.join();
		b.join();

		return value;
	}

private:
	int value;
	std::mutex numlock;
	void ExecutePart(int from, int to)
	{
		std::lock_guard<mutex> lock(numlock);
		for(int i = from; i < to; i++)
			value++;
	}
};

class A
{
public:
	A()
	{
		std::cout << "A::A()" << "\n";
	}

	virtual ~A()
	{
		std::cout << "A::~A()" << "\n";
	}

	virtual void DoIt()
	{
		std::cout << "A::DoIt()" << "\n";
	}

	virtual void DoIt2()
	{
		std::cout << "A::DoIt2()" << "\n";
	}
};

class B : public A
{
public:
	B()
	{
		std::cout << "B::B()" << "\n";
	}

	virtual ~B()
	{
		std::cout << "B::~B()" << "\n";
	}

	void DoIt()
	{
		std::cout << "B::DoIt()" << "\n";
		throw;
	}

	void DoIt2()
	{
		std::cout << "B::DoIt()" << "\n";
	}

};

void Scenario_I()
{
	B b;
	A* a = &b;
	try
	{

		a->DoIt();
	}
	catch (...)
	{
		std::cout << "Exception cauth!" << "\n";
	}
}

void Scenario_II()
{
	try
	{
		B b;
		A* a = &b;

		a->DoIt2();
	}
	catch (...) 
	{
		std::cout << "Exception cauth!" << "\n";
	}
}

class D
{
public:
	D()
	{
		int a = 10;
		GetResult(a);

		std::cout << a;
	}


	virtual void GetResult(int& result)
	{
		result = 0;

		for (int i = 0; i < 10; i++)
		{
			if (result % 2 == 0)
				result++;
		}
	}
};

class C
{
public:
	C()
	{
		int a = 2;
		int b = 2;

		DoIt(++a);
		DoIt(b++);
	}


	virtual void DoIt(int v)
	{
		std::cout << v + 7 << "\n";
	}
};

template <typename T>
class CircArray
{
public:
	CircArray(int _maxSize)
	{
		maxSize = _maxSize;
	}

	~CircArray()
	{
	}

	void Add(T item)
	{
		if (data.size() == maxSize)
			data.pop_front();

		data.push_back(item);
	}

	std::string ToString()
	{
		std::string s;

		for (auto x : data)
		{
			s += " " + std::to_string(x);
		}

		return s;
	}

private:
	int maxSize;
	std::deque<T> data;
};


void testCircArray()
{
	CircArray<int> cr(3);
	for (int i = 0; i < 10; i++)
	{
		cr.Add(i);

		cout << cr.ToString().c_str() << endl;
	}
}

/*
int main()
{
	//testLambda();
	//testThread();
	//testMoveSemanthics();
	//testMoveSemanthics2();
	//testSmartPointers();
	//testPolimorphism();

	//Calculator calc;
	//print(calc.Execute());

	//Scenario_I();
	//Scenario_II();

	//D d;

	//C d;

	testCircArray();

	std::vector<int> v = { 1, 2, 3, 4 };

	v.erase(v.begin());

	return 1;
}
*/


template <typename T>
class CArray
{
public:
	CArray(int size_)
	{
		size = size_;
		datasize = size * 2;
		data = new T[datasize];
		start = 0;
		n = 0;
	}

	~CArray()
	{
		delete[] data;
	}

	void push(T &item)
	{
		if (n >= size)
		{
			if (start + size + 1 < datasize)
				start++;
			else
			{
				memmove(data, data + start + 1, (size - 1) * sizeof(T));
				n = size - 1;
				start = 0;
			}
		}

		data[n++] = item;
	}


	std::string ToString()
	{
		std::string s;
		for (int i = start; i < n; i++)
			s += std::string(" " + std::to_string(data[i]));

		return s;
	}

private:
	T *data;
	int size;
	int datasize;
	int start;
	int n;
};
/*
int main()
{
	CArray<int> a(5);
	for (int i = 0; i < 10; i++)
	{
		a.push(i);
		std::cout << "Result: " << a.ToString() << std::endl;
	}
}
*/
class ListQueue
{
public:
	ListQueue()
	{
		head = nullptr;
	}

	~ListQueue()
	{
		while (Dequeue() != nullptr);
	}

	void Enqueue(int value)
	{
		Node *n = new Node{ value, nullptr };
		if (head == nullptr)
			head = n;
		else
		{
			Node *ptr = head;
			for ( ; ptr->next != nullptr; ptr = ptr->next);

			ptr->next = n;
		}
	}

	int *Dequeue()
	{
		if (head != nullptr)
		{
			value = head->value;
			Node *next = head->next;
			delete head;
			head = next;
			return &value;
		}

		return nullptr;
	}
private:
	struct Node
	{
		int value;
		Node *next;
	};

	Node *head;
	int value;
};

/*int main()
{
	ListQueue lq;
	for (int i = 0; i < 10; i++)
		lq.Enqueue(i);

	for (int *p = lq.Dequeue(); p != nullptr; p = lq.Dequeue())
		cout << std::to_string(*p) << endl;
}*/



class Foo
{
public:
	Foo()
	{
		cout << "CTR" << endl;
		x = 0;
	}

	~Foo()
	{
		cout << "DTR" << endl;
		x = 0;
	}

	Foo(const Foo &other)
	{
		cout << "Copy CTR" << endl;

		x = other.x;
	}

	Foo(Foo &&other)
	{
		cout << "Move CTR" << endl;

		x = std::move( other.x );
	}

	Foo &operator = (const Foo &other)
	{
		cout << "Copy assign" << endl;
		x = other.x;
		return *this;
	}

	Foo &operator = (Foo &&other)
	{
		cout << "Move assign" << endl;
		x = std::move( other.x );
		other.x = 0;
		return *this;
	}

private:
	int x;
};


int main()
{
	Foo fn();            // function returning a MyClass object
	Foo foo;             // default constructor
	Foo bar = foo;       // copy constructor
	Foo baz = fn();      // move constructor
	foo = bar;               // copy assignment
	baz = Foo();         // move assignment 
}