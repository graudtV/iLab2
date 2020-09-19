#include <iostream>

template <class T>
struct Base {
	struct Nested { Nested() { printf("Nested construct\n");} };
};

template <class T>
struct Derived : public Base<T> {
	//using Nested = typename Base<T>::Nested;
	//void f() { typename Base<T>::Nested(); }
};

int main()
{
	Base<int> base;
	Derived<int> derived;
	derived.f();


	return 0;
}