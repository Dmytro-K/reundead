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

// helper class
template<typename R, template<typename...> class Params, typename... Args, std::size_t... I>
R call_helper( std::function<R( Args... )> const&func, Params<Args...> const&params, std::index_sequence<I...> )
{
	return func( std::get<I>( params )... );
}

// "return func(params...)"
template<typename R, template<typename...> class Params, typename... Args>
R call( std::function<R( Args... )> const&func, Params<Args...> const&params )
{
	return call_helper( func, params, std::index_sequence_for<Args...>{} );
}

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