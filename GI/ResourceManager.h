#pragma once

#include <vector>

#include <type_traits>

#include "Resource.h"
#include "Program.h"

#include <boost/variant.hpp>

//typedef boost::variant< std::enable_if<nox::IsResource<nxProgram>::value>::type > SupportedTypes;
typedef boost::variant< int, nxResourceContainer<nxProgram> > SupportedTypes;

class nxResourceManager {

private:

	std::vector< SupportedTypes >				m_ResourceContainers;
	//std::vector< int, SupportedTypes >		m_ResourceContainers;

public:

										nxResourceManager();

	//int									CountContainers() { return m_ResourceContainers.size(); }
	
	template <typename T>
	void					AddResource(T* resource) {
		//(nox::interfaces::IsResource<T>::value, "resource is required");
		SupportedTypes prog(1);
		//std::pair< int, SupportedTypes > mypair = std::make_pair(1, prog);
		//m_ResourceContainers.push_back(mypair);
		nox::interfaces::IsResource<T>::m_Storage.m_Resources.push_back(*resource);
		nox::interfaces::IsResource<T>::m_Storage.m_Resources.push_back(*resource);
		nox::interfaces::IsResource<T>::m_Storage.m_Resources.push_back(*resource);
		nox::interfaces::IsResource<T>::m_Storage.m_Resources.size();

		SupportedTypes rsc(nox::interfaces::IsResource<T>::m_Storage);

		m_ResourceContainers.push_back(rsc);
		std::cout << "Vector Size : " << nox::interfaces::IsResource<T>::m_Storage.m_Resources.size() << std::endl;
		
		nxResourceHandle<typename T> hnd(resource);
	}

	//template <typename T>
	//nxResourceHandle<T>				AddResource(const std::string& file_path);

};