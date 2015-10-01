#pragma once

#include <atomic>

template<typename T>
class nxHandle {

private:

	T*					m_Ptr;
	std::atomic<int>	m_RefCount;

public:

	T* operator->() {
		return m_Ptr;
	}

	int RefCount() {
		return m_RefCount.load();
	}

};