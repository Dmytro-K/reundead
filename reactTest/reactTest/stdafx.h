#pragma once

#ifndef IS_STDAFX_H
#define IS_STDAFX_H

//#ifdef DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#endif /* _DEBUG */

#include <iostream>
#include <unordered_set>
#include <initializer_list>
#include <functional>
#include <tuple>
#include <cassert>

template <class T>
struct remove_const_in_ptr
{
	typedef std::add_pointer_t<std::remove_const_t<std::remove_pointer_t<T>>> type;
};

#define REMOVE_CONST_PTR_CAST( __PTR__ ) (const_cast<remove_const_in_ptr<decltype( __PTR__ )>::type>( __PTR__ ))

#ifdef NEED_DBGLINE
#define DBGLINE() ( printf("%s : %d\n", __FUNCTION__, __LINE__) )
#else
#define DBGLINE()
#endif

template<typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
	auto static call( F f, Tuple&& t )
	{
		return call_impl<F, Tuple, TotalArgs == 1 + sizeof...( N ),
			TotalArgs, N..., sizeof...( N )
		>::call( f, std::forward<Tuple>( t ) );
	}
};

template<typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
	auto static call( F f, Tuple&& t )
	{
		return f( std::get<N>( std::forward<Tuple>( t ) )... );
	}
};

template<typename F, typename Tuple>
auto call( F f, Tuple&& t )
{
	typedef typename std::decay<Tuple>::type type;
	return call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
		std::tuple_size<type>::value
	>::call( f, std::forward<Tuple>( t ) );
}

template<std::size_t N>
struct tuple_functor
{
	template<typename T, typename F>
	static void run( std::size_t i, T&& t, F&& f )
	{
		const std::size_t I = ( N - 1 );
		switch( i ) {
			case I:
				std::forward<F>( f )( std::get<I>( std::forward<T>( t ) ) );
				break;

			default:
				tuple_functor<I>::run( i, std::forward<T>( t ), std::forward<F>( f ) );
		}
	}
};

template<>
struct tuple_functor<0>
{
	template<typename T, typename F>
	static void run( std::size_t, T, F )
	{
	}
};





template <size_t I>
struct visit_impl
{
	template <typename T, typename F>
	static void visit( T& tup, size_t idx, F fun )
	{
		if( idx == I - 1 ) fun( std::get<I - 1>( tup ) );
		else visit_impl<I - 1>::visit( tup, idx, fun );
	}
};

template <>
struct visit_impl<0>
{
	template <typename T, typename F>
	static void visit( T& tup, size_t idx, F fun )
	{
		assert( false );
	}
};

template <typename F, typename... Ts>
void visit_at( std::tuple<Ts...> const& tup, size_t idx, F fun )
{
	visit_impl<sizeof...( Ts )>::visit( tup, idx, fun );
}

template <typename F, typename... Ts>
void visit_at( std::tuple<Ts...>& tup, size_t idx, F fun )
{
	visit_impl<sizeof...( Ts )>::visit( tup, idx, fun );
}




#endif /* IS_STDAFX_H */