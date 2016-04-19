#pragma once

#include <vector>
#include <iostream>

#include "Handle.h"

#include "Program.h"

template <typename T>
bool ChangedDispatch(void * p) {
    return static_cast<T*>(p)->Changed();
    std::cout << " " << std::endl;
}

template <typename T>
void ReloadDispatch(void * p) {
    static_cast<T*>(p)->Reload();
    std::cout << " " << std::endl;
}

template <typename T = void>
class nxResourceHandle {
public:
    bool(*ChangedDelegate)(void *);
    void(*ReloadDelegate)(void *);
    T * p;
    template <typename U>
    nxResourceHandle(U* p, 
        bool(*ChangedDelegate)(void *) = &ChangedDispatch<typename std::remove_pointer<U>::type>,
        void(*ReloadDelegate)(void *) = &ReloadDispatch<typename std::remove_pointer<U>::type>)
        : p(p), ChangedDelegate(ChangedDelegate), ReloadDelegate(ReloadDelegate)
    {}
    bool Changed() {
        return ChangedDelegate(p);
    }
    void Reload() {
        ReloadDelegate(p);
    }
    ~nxResourceHandle() {
        //deleter(p);
    }
};

template <typename T>
class nxResourceContainer {

public:

	typedef T resource_type;

	std::vector<resource_type>		m_Resources;

};

// A general NOX namespace
namespace nox {
	// A namespace for Interfaces
	namespace interfaces {

	template <typename ResourceType>
	class IsResource
	{
		// This type won't compile if the second template parameter isn't of type T,
		// so I can put a function pointer type in the first parameter and the function
		// itself in the second thus checking that the function has a specific signature.
		template <typename T, T> struct TypeCheck;

		typedef long Yes;
		typedef char No ;

		// Helper structs to hold declarations of function pointers.
		template <typename T> struct _Manage { typedef void (T::*fptr)(T resource); };
		template <typename T> struct _Save { typedef void (T::*fptr)(); };
		template <typename T> struct _Reload { typedef void (T::*fptr)(); };
		template <typename T> struct _Changed { typedef bool (T::*fptr)(); };
		template <typename T> struct _Get { typedef void (T::*fptr)(); };

		template <typename T> static Yes HasManage(TypeCheck< typename _Manage<T>::fptr, &T::Manage >*);
		template <typename T> static No  HasManage(...);

		template <typename T> static Yes HasSave(TypeCheck< typename _Save<T>::fptr, &T::Save >*);
		template <typename T> static No  HasSave(...);

		template <typename T> static Yes HasReload(TypeCheck< typename _Reload<T>::fptr, &T::Reload >*);
		template <typename T> static No  HasReload(...);

		template <typename T> static Yes HasChanged(TypeCheck< typename _Changed<T>::fptr, &T::Changed >*);
		template <typename T> static No  HasChanged(...);

		template <typename T> static Yes HasGet(TypeCheck< typename _Get<T>::fptr, &T::Get >*);
		template <typename T> static No  HasGet(...);

		// Resource Manager needs access to the private ResourceContainer class
		friend class ResourceManager;

	public:

        using value_type = ResourceType;
        using pointer_type = ResourceType*;

		//static bool const value = (sizeof(HasManage<ResourceType>(0)) == sizeof(Yes));
		static bool const value = (sizeof(HasGet<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasReload<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasManage<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasChanged<ResourceType>(0)) == sizeof(Yes))
			&& (sizeof(HasSave<ResourceType>(0)) == sizeof(Yes));
		
        typedef std::enable_if_t<value, value_type> resource_type;

		// Static member holding storage for the resources
		//static ResourceContainer<resource_type>& GetContainer() { return m_Storage; }
		static nxResourceContainer< typename ResourceType > m_Storage;

	};

	template <typename ResourceType>
    nxResourceContainer< typename ResourceType > IsResource<ResourceType>::m_Storage = { };

	} // namespace nox
} // namespace interfaces



// Every nxHandle refering toa a resource can be called an nxResourceHandle
//template<typename T>
//using nxResourceHandle = nxHandle< typename std::enable_if_t<nox::interfaces::IsResource<T>::value> >;

// A typical nxResource
// Could be considered a base class
class nxResource {

	void Manage(nxResource resource) {
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

	bool Changed() {
		std::cout << "changed" << std::endl;
	}

};