#pragma once

#include <vector>

#include <boost/variant.hpp>

#include "Resource.h"
#include "Program.h"

typedef boost::variant< std::enable_if<nox::IsResource<nxProgram>::value>::type > SupportedTypes;

class nxResourceManager {

private:

	template <typename T>	
	std::vector< SupportedTypes >		m_ResourceContainers;

public:

	int CountContainers() { //return m_ResourceContainers.size(); }
	
	template <typename T>
	nxHandle AddResource(const T& resource);

	template <typename T>
	nxHandle AddResource(const T* resource);

};