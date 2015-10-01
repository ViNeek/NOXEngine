#pragma once

#include <vector>
#include <iostream>

#include "Handle.h"

template <typename T>
class ResourceContainer {

public:

	typedef T resource_type;

	std::vector<T>		m_Resources;

};

namespace nox {

	template <class ResourceType>
	class IsResource
	{
		// This type won't compile if the second template parameter isn't of type T,
		// so I can put a function pointer type in the first parameter and the function
		// itself in the second thus checking that the function has a specific signature.
		template <typename T, T> struct TypeCheck;

		typedef char Yes;
		typedef long No;

		// Helper structs to hold declarations of function pointers.
		template <typename T> struct _Load { typedef void (T::*fptr)(); };
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

		// Resource Manager needs access to the private storafe class
		friend class ResourceManager;

		// Static member holding storage for the resources
		static ResourceContainer<ResourceType> GetContainer() { return m_Storage; }
		static ResourceContainer<ResourceType> m_Storage;

	public:

		static bool const value = (sizeof(HasGet<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasReload<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasLoad<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasSave<ResourceType>(0)) == sizeof(Yes));

	};

	template<typename T>
	typedef Handle< typename std::enable_if<IsResource<T>::value>::type > ResourceHandle;

} // namespace NOX

class nxResource {

	void Load() {
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