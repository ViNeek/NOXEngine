#pragma once

#include <vector>
#include <iostream>

#include "Handle.h"

// Each Resource has its equivalent resource container class
// used by the resource manager
template <typename T>
class ResourceContainer {

public:

	std::vector<typename T::resource_type>		m_Resources;

};

// A general NOX namespace
namespace nox {
	// A namespace for Interfaces
	namespace interfaces {

	template <class ResourceType>
	class IsResource
	{
		// This type won't compile if the second template parameter isn't of type T,
		// so I can put a function pointer type in the first parameter and the function
		// itself in the second thus checking that the function has a specific signature.
		template <typename T, T> struct TypeCheck;

		typedef char Yes;
		typedef char No ;

		// Helper structs to hold declarations of function pointers.
		template <typename T> struct _Load { typedef void (T::*fptr)(const std::string& path); };
		template <typename T> struct _Save { typedef void (T::*fptr)(); };
		template <typename T> struct _Reload { typedef void (T::*fptr)(); };
		template <typename T> struct _Get { typedef void (T::*fptr)(); };

		template <typename T> static Yes HasLoad(TypeCheck< typename _Load<T>::fptr, &T::Load >*);
		template <typename T> static No  HasLoad(...);

		template <typename T> static Yes HasSave(TypeCheck< typename _Save<T>::fptr, &T::Save >*);
		template <typename T> static No  HasSave(...);

		template <typename T> static Yes HasReload(TypeCheck< typename _Reload<T>::fptr, &T::Reload >*);
		template <typename T> static No  HasReload(...);

		template <typename T> static Yes HasGet(TypeCheck< typename _Get<T>::fptr, &T::Get >*);
		template <typename T> static No  HasGet(...);

		// Resource Manager needs access to the private ResourceContainer class
		friend class ResourceManager;

		// Static member holding storage for the resources
		static ResourceContainer<ResourceType> GetContainer() { return m_Storage; }
		static ResourceContainer<ResourceType> m_Storage;

	public:

		static bool const value = (sizeof(HasGet<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasReload<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasLoad<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasSave<ResourceType>(0)) == sizeof(Yes));

		typedef std::enable_if_t<value> resource_type;

	};

	} // namespace nox
} // namespace interfaces

// Every nxHandle refering toa a resource can be called an nxResourceHandle
template<typename T>
using nxResourceHandle = nxHandle< typename std::enable_if_t<nox::interfaces::IsResource<T>::value> >;

// A typical nxResource
// Could be considered a base class
class nxResource {

	void Load(const std::string& path) {
		std::cout << "load" << std::endl;
	}

	void Save() {
		std::cout << "save" << std::endl;
	}

	void Get() {
		std::cout << "get" << std::endl;
	}

	void Reload() {
		std::cout << "reload" << std::endl;
	}

};