/*
#pragma once

#include <array>



template <class T, size_t I, size_t... J>
class nxArray
{

	using nested_multi_array = typename nxArray<T, J...>::type;
	// typedef typename MultiDimArray<T, J...>::type Nested;
	using multi_array = std::array<nested_multi_array, I>;

	multi_array data_;


public:

	//static int offset = I * nxArray < T, J... >::offset;

	nested_multi_array& operator[] (const int nIndex) { return data_[nIndex]; }

};

template <class T, size_t I>
class nxArray<T, I>
{

	std::array<typename T, I> data_;

public:

	using type = std::array<T, I>;
	// typedef std::array<T, I> type;

	T& operator[] (const int nIndex) { return data_[nIndex]; }

	//static int offset = 1;

};
*/