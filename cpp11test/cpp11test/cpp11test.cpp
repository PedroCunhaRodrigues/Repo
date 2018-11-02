#include "pch.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <string.h>

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

int main()
{
	//testLambda();
	//testThread();
	//testMoveSemanthics();
	//testSmartPointers();
	testPolimorphism();

}

