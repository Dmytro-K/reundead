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

	ReactBase( const ReactBase & old )
	{
		std::cout << "Called ReactBase(copy) for  " << this << std::endl;
	}

	virtual ~ReactBase()
	{
		std::cout << "Called ~ReactBase() for     " << this << std::endl;
	}

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
	typedef std::function<T( const Targs& ... )> FuncType;
	typedef T ValueType;
	React( const FuncType &func, Targs& ...args ) : func( func ), args( std::forward_as_tuple(args...) )
	{
		const std::size_t num = sizeof...( Targs );
		auto do_sth = [this]( auto& elem ) {
			elem.slaves.insert( this );
		};

		for( int i = 0; i < num; ++i ) {
			visit_at( this->args, i, do_sth );
		}

		change();
	}

	React( const T &item ) : item( item ), func( FuncType() )
	{
	}

	React( T &&item ) : item( item ), func( FuncType() )
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
			//item = call( func, args );
			delayed_dispatch();
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

	template<std::size_t ...I>
	void call_func( std::index_sequence<I...> )
	{
		item = func( std::get<I>( args )... );
	}
	void delayed_dispatch()
	{
		call_func( std::index_sequence_for<Targs...>{} );
	}

private:
	T item;
	FuncType func;
	tuple<Targs&...> args;
};

/* Hope for RVO */
template <typename T>
inline React<T> react( T item )
{
	return React<T>( item );
}

template <typename F, typename ...Targs>
inline auto react( F func, Targs& ...args ) -> React<decltype( func( args... ) ), Targs...>
{
	typedef React<decltype( func( args... ) ), Targs...> rtype;
	rtype::FuncType funcal = static_cast<rtype::FuncType>( func );
	return rtype( funcal, args... );
}

#endif /* IS_REACT_H */