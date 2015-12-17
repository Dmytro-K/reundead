#include "stdafx.h"
#include "React.h"

using std::cout;
using std::cin;
using std::endl;

void main()
{
	struct MainExit
	{
		~MainExit()
		{
			_CrtDumpMemoryLeaks();
		}
	} dummy;

	auto a = react( 3 );
	auto b = react( 5 );
	auto c = react( [&a, &b]( const ReactBase::Args &args ) -> int {
		//return ( ( React<int>* )( args.args[0] ) )->value() + ( ( React<int>* )( args.args[1] ) )->value();
		return a.value() + b.value();
	}, { &a,&b } );

	cout << "a = " << a.value() << endl;

	printf( "c = %d\n", c.value() );

	a.set( 10 );
	printf( "c = %d\n", c.value() );
	b.set( -6 );
	printf( "c = %d\n", c.value() );

}