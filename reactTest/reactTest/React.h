#pragma once

#ifndef IS_REACT_H
#define IS_REACT_H

#include "stdafx.h"

using std::unordered_set;
using std::initializer_list;
using std::tuple;

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

	//struct Args
	//{
	//	Args() : count( 0 ), args(nullptr)
	//	{
	//	}

	//	Args( const Args& old ) : count(old.count)
	//	{
	//		this->args = new ReactBase *[count];
	//		memcpy( args, old.args, count*sizeof( *args ) );
	//	}

	//	Args( Args&& old ) : count(old.count)
	//	{
	//		this->args = old.args;
	//		old.args = nullptr;
	//	}

	//	Args( initializer_list<ReactBase*> args ) : count( args.size() )
	//	{
	//		DBGLINE();
	//		this->args = new ReactBase*[count];
	//		int i = 0;
	//		for( auto el : args ) {
	//			this->args[i++] = el;
	//			//this->args[i++] = const_cast<std::add_pointer<std::remove_const<std::remove_pointer<decltype(&el)>::type>::type>::type>( &el );
	//		}
	//	}

	//	virtual ~Args()
	//	{
	//		delete[] args;
	//	}

	//	const size_t count;
	//	ReactBase **args;
	//};

protected:
	unordered_set<ReactBase*> slaves;

	template <typename T, typename ...Targs>
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

template <typename T, typename ...Targs>
class React : public ReactBase
{
public:
	//typedef T( *FuncType )( const ReactBase::Args& );
	typedef std::function<T( const Targs& ... )> FuncType;
	typedef T ValueType;
	React( FuncType &func, Targs& ...args ) : func( func ), args( std::forward_as_tuple(args...) )
	{
		ReactBase *ths = this;
		const std::size_t num = sizeof...( Targs );
		//auto my_tuple = std::forward_as_tuple( t... );
		auto do_sth = [ths]( auto& elem ) { elem.slaves.insert( ths ); };
		/*	for( int i = 0; i < num; ++i )
				tuple_functor<num>::run( i, this->args, do_sth );*/

		for( int i = 0; i < num; ++i ) {
			visit_at( this->args, 0, do_sth );
		}

		/*for( size_t i = 0; i < sizeof...(Targs); i++ ) {
			args.args[i]->slaves.insert( this );
		}*/
		change();
	}

	React( const T &item ) : item( item ), func( *new std::function<T()>() )
	{
	}

	React( T &&item ) : item( item ), func( nullptr )
	{
	}

	virtual ~React()
	{

	}

	const T& value() const
	{
		return item;
	}

	T valueCopy() const
	{
		return item;
	}

	void change()
	{
		if( func ) {
			item = call( func, args );
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
	FuncType &func;
	tuple<Targs&...> args;
};

/* Hope for RVO */
template <typename T>
inline React<T> react( T item )
{
	return React<T>( item ); /* Dark Magic on 2nd argument */
}

template <typename F, typename ...Targs>
//inline React<T> react( T( *func )( const ReactBase::Args& ), ReactBase::Args &args )
inline auto react( F func, Targs& ...args ) -> React<decltype( func( args... ) ), Targs...>
{
	typedef React<decltype( func( args... ) ), Targs...> rtype;
	rtype::FuncType funcal = static_cast<rtype::FuncType>( func );
	return rtype( funcal, args... );
}

//
//template <typename F, typename ...Targs>
//inline auto react( F func, initializer_list<ReactBase*> args ) -> React<decltype( func( ReactBase::Args() ) )>
//{
//	//typedef React<int> rtype;
//	//typedef React< std::result_of< func( ReactBase::Args& ) > > rtype;
//	//std::result_of<puts("")>::type a;
//	//decltype(func( ReactBase::Args() )) d;
//	return react( func, ReactBase::Args( args ),  0 );
//}

///* http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments */
//#define REACT_GET_MACRO(_1,_2,NAME,...) NAME
//#define _react(...) REACT_GET_MACRO(__VA_ARGS__, GEN_REACT1, GEN_REACT2)(__VA_ARGS__)
//
//#define GEN_REACT1( _ITEM_ ) ()
//
//#define GEN_REACT2( _FUNC_, _ITEM_ ) ()

#endif /* IS_REACT_H */