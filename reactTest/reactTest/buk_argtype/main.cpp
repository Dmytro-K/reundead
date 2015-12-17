#include "stdafx.h"
#include "React.h"

using std::cout;
using std::cin;
using std::endl;

void print( int a, int b, int c )
{
	printf( "a = %d\tb = %d\tc = %d\n", a, b, c );
}

typedef struct
{
	int a, b, c;
} MyStruct;

typedef void( *functype )( MyStruct );

//auto funct( argtype &arg ) -> decltype( arg.a.value() + arg.b.value() )
//{
//	return arg.a.value() + arg.b.value();
//};

class SomeClass
{
public:
	SomeClass()
	{
		cout << "Called SomeClass() for     " << this << endl;
	}

	SomeClass( const SomeClass &sc )
	{
		cout << "Called SomeClass(copy) for " << this << endl;
	}

	virtual ~SomeClass()
	{
		cout << "Called ~SomeClass() for    " << this << endl;
	}
};

void main2()
{
	auto a = react( 3 );
	auto b = react( 5 );

	typedef struct
	{
		decltype( a ) & a;
		decltype( b ) & b;
	} argtype;

	auto &c = react<int, argtype>( []( argtype &arg ) /*-> decltype( arg.a.value() + arg.b.value() )*/ {
		return arg.a.value() + arg.b.value();
	}, argtype( {a, b} ) );

	cout << "a = " << a.value() << endl;

	printf( "c = %d\n", c.value() );
	
	a.set( 10 );
	printf( "c = %d\n", c.value() );
	b.set( -6 );
	printf( "c = %d\n", c.value() );

	//auto &obj = SomeClass();

	functype func = (functype)(void*)print;
	func( { 1, 2, 3 } );

}

void main()
{
	main2();
	cout << "Press any key for exit." << endl;
	//cin.get();
	_CrtDumpMemoryLeaks();
}