#pragma once

#ifndef IS_REACT_H
#define IS_REACT_H

#include "stdafx.h"

using std::unordered_set;
using std::initializer_list;

class ReactBase
{
public:
	ReactBase()
	{
		std::cout << "Called ReactBase() for " << this << std::endl;
	}

	virtual ~ReactBase()
	{
		std::cout << "Called ~ReactBase() for " << this << std::endl;
	}

	struct Args
	{
		Args() = delete;

		Args( initializer_list<ReactBase> args )
		{
			count = args.size();
			this->args = new ReactBase*[count];
			int i = 0;
			for( auto &el : args ) {
				this->args[i++] = REMOVE_CONST_PTR_CAST( &el );
				//this->args[i++] = const_cast<std::add_pointer<std::remove_const<std::remove_pointer<decltype(&el)>::type>::type>::type>( &el );
			}
		}

		~Args()
		{
			delete args;
		}

		size_t count;
		ReactBase **args;
	};

protected:
	unordered_set<ReactBase*> slaves;

	template <class T, class Targs>
	friend class React;

protected:

	virtual void emit()
	{
		for( auto el : slaves ) {
			el->change();
			//el->func( el->args );
		}
	}

	virtual void change() = 0;
	//{
		//func( args );
	//}

};

template <class T, class Targs>
class React : ReactBase
{
public:
	typedef T( *FuncType )( Targs& );
	typedef T ValueType;
	React( FuncType func, Targs &args ) : ReactBase(), func( func ), args( args )
	{
		//if( func == nullptr ) {
		//	func = emptyFunc;
		//}
		//int count = sizeof( args ) / sizeof( ReactBase* );
		ReactBase **end = (ReactBase **)( ( &args ) + 1 );
		for( ReactBase **p = (ReactBase **)&args; p < end; p++ ) {
			( *p )->slaves.insert( this );
		}
		change();
	}

	React( const T &item, int args ) : item( item ),
		func( nullptr ),
		args( args ) /* Dark Magic */
	{
		//this->item = item;
	}

	/*
	React( const T &item )
	{
		this->item = item;
	}
	*/

	virtual ~React()
	{

	}


	const T& value()
	{
		return item;
	}

	T valueCopy()
	{
		return item;
	}

	static T emptyFunc( Targs &args )
	{
		return item;
	}

	void change()
	{
		//throw "Not implemented!";
		if( func ) {
			item = func( args );
		}
	}

	void set( T&& item )
	{
		this->item = item;
		emit();
	}

	//template<class TR>
	//React<T>& operator=( const React<TR>& rhs )
	//{
	//	rhs.slaves.insert( this );
	//	this->dependOn = &rhs;
	//	item = rhs.item;
	//}

private:
	T item;
	//T&& (*func)(void* args);
	FuncType func;
	Targs args;
};

template <class T>
React<T, int>& react( T item )
{
	return *new React<T, int>( item, 0 ); /* Dark Magic on 2nd argument */
}

template <class T, class Targs>
React<T, Targs>& react( T( *func )( Targs& ), Targs &args )
{
	return *new React<T, Targs>( func, args );
}

#endif /* IS_REACT_H */