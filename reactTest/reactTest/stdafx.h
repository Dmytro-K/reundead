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

#endif /* IS_STDAFX_H */