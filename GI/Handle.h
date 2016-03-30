#pragma once

#include <atomic>

template<typename T>
class nxHandle {

private:

	T*					m_Ptr;
	std::atomic<int>	m_RefCount;


	void AddRef() {
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void DecRef() {
		m_RefCount.fetch_sub(1, std::memory_order_relaxed);
	}

public:
    
	nxHandle(T* ref) {

		std::cout << "ctor" << std::endl;
	}

	nxHandle(nxHandle<T>&& hnd) {
	
		std::cout << "move" << std::endl;
	}

	T* operator->() {
		return m_Ptr;
	}

	int RefCount() {
		return m_RefCount;
	}

};