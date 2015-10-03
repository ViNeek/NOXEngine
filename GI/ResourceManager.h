#pragma once

#include <vector>

#include <boost/variant.hpp>

#include "Resource.h"
#include "Program.h"

//typedef boost::variant< std::enable_if<nox::IsResource<nxProgram>::value>::type > SupportedTypes;
typedef boost::variant< nxProgram > SupportedTypes;

class nxResourceManager {

private:

	std::vector< SupportedTypes >		m_ResourceContainers;

public:

								nxResourceManager();

	int							CountContainers() { return m_ResourceContainers.size(); }
	
	template <typename T>
	nxResourceHandle<T>					AddResource(const T& resource);

	template <typename T>
	nxResourceHandle<T>					AddResource(const T* resource);

};