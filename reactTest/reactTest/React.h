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
		std::cout << "Called ReactBase() for      " << this << std::endl;
	}

	//ReactBase( const ReactBase & old ) = delete;
	ReactBase( const ReactBase & old )
	{
		std::cout << "Called ReactBase(copy) for  " << this << std::endl;
	}

	virtual ~ReactBase()
	{
		std::cout << "Called ~ReactBase() for     " << this << std::endl;
	}

	struct Args
	{
		Args() : count( 0 ), args(nullptr)
		{
		}

		Args( const Args& old ) : count(old.count)
		{
			this->args = new ReactBase *[count];
			memcpy( args, old.args, count*sizeof( *args ) );
		}

		Args( Args&& old ) : count(old.count)
		{
			this->args = old.args;
			old.args = nullptr;
		}

		Args( initializer_list<ReactBase*> args ) : count( args.size() )
		{
			DBGLINE();
			this->args = new ReactBase*[count];
			int i = 0;
			for( auto el : args ) {
				this->args[i++] = el;
				//this->args[i++] = const_cast<std::add_pointer<std::remove_const<std::remove_pointer<decltype(&el)>::type>::type>::type>( &el );
			}
		}

		virtual ~Args()
		{
			delete[] args;
		}

		const size_t count;
		ReactBase **args;
	};

protected:
	unordered_set<ReactBase*> slaves;

	template <class T>
	friend class React;

protected:

	virtual void emit()
	{
		for( auto el : slaves ) {
			el->change();
		}
	}

	virtual void change() = 0;

};

template <class T>
class React : public ReactBase
{
public:
	//typedef T( *FuncType )( const ReactBase::Args& );
	typedef std::function<T( const ReactBase::Args& )> FuncType;
	typedef T ValueType;
	React( FuncType func, ReactBase::Args &args ) : func( func ), args( args )
	{
		for( size_t i = 0; i < args.count; i++ ) {
			args.args[i]->slaves.insert( this );
		}
		change();
	}

	React( const T &item ) : item( item ), func( nullptr )
	{
	}

	React( T &&item ) : item( item ), func( nullptr )
	{
	}

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

	void change()
	{
		if( func ) {
			item = func( args );
		}
	}

	void set( const T& item )
	{
		this->item = item;
		emit();
	}

	void set( T&& item )
	{
		this->item = item;
		emit();
	}

private:
	T item;
	FuncType func;
	ReactBase::Args args;
};

/* Hope for RVO */
template <class T>
inline React<T> react( T item )
{
	return React<T>( item ); /* Dark Magic on 2nd argument */
}

template <class F>
//inline React<T> react( T( *func )( const ReactBase::Args& ), ReactBase::Args &args )
inline auto react( F func, ReactBase::Args &args, int ) -> React<decltype( func( ReactBase::Args() ) )>
{
	typedef React<decltype( func( ReactBase::Args() ) )> rtype;
	rtype::FuncType funcal = static_cast<rtype::FuncType>( func );
	return rtype( funcal, args );
}

template <class F>
inline auto react( F func, initializer_list<ReactBase*> args ) -> React<decltype( func( ReactBase::Args() ) )>
{
	//typedef React<int> rtype;
	//typedef React< std::result_of< func( ReactBase::Args& ) > > rtype;
	//std::result_of<puts("")>::type a;
	//decltype(func( ReactBase::Args() )) d;
	return react( func, ReactBase::Args( args ),  0 );
}

///* http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments */
//#define REACT_GET_MACRO(_1,_2,NAME,...) NAME
//#define _react(...) REACT_GET_MACRO(__VA_ARGS__, GEN_REACT1, GEN_REACT2)(__VA_ARGS__)
//
//#define GEN_REACT1( _ITEM_ ) ()
//
//#define GEN_REACT2( _FUNC_, _ITEM_ ) ()

#endif /* IS_REACT_H */