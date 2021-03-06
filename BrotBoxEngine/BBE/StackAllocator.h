#pragma once

#include "../BBE/DataType.h"
#include "../BBE/UtilTest.h"
#include "../BBE/List.h"
#include <iostream>
#include <cstring>
#include "../BBE/STLCapsule.h"
#include "../BBE/Exceptions.h"
#include "../BBE/Math.h"

namespace bbe
{
	namespace INTERNAL
	{
		template <typename T>
		void executeDestructor(const void* data)
		{
			auto originalType = static_cast<const T*>(data);
			originalType->~T();
		}

		class StackAllocatorDestructor
		{
		private:
			const void* m_data;
			void(*destructor)(const void*);
		public:
			template<typename T>
			explicit StackAllocatorDestructor(const T& data) noexcept :
				m_data(bbe::addressOf(data))
			{
				destructor = executeDestructor<T>;
			}

			void operator () () noexcept
			{
				destructor(m_data);
			}
		};
	}




	template <typename T>
	class StackAllocatorMarker
	{
	public:
		T* m_markerValue;
		size_t m_destructorHandle;
		StackAllocatorMarker(T* markerValue, size_t destructorHandle) :
			m_markerValue(markerValue), m_destructorHandle(destructorHandle)
		{
			//do nothing
		}
	};

	template <typename T = byte, typename Allocator = STLAllocator<T>>
	class StackAllocator
	{
	public:
		typedef typename T                                           value_type;
		typedef typename T*                                          pointer;
		typedef typename const T*                                    const_pointer;
		typedef typename T&                                          reference;
		typedef typename const T&                                    const_reference;
		typedef typename size_t                                      size_type;
		typedef typename std::pointer_traits<T*>::difference_type    difference_type;
		typedef typename std::pointer_traits<T*>::rebind<const void> const_void_pointer;
	private:
		static constexpr size_t STACK_ALLOCATOR_DEFAULT_SIZE = 1024;
		T* m_data = nullptr;
		T* m_head = nullptr;
		size_t m_length = 0;

		Allocator* m_parentAllocator = nullptr;
		bool m_needsToDeleteParentAllocator = false;
		
		List<INTERNAL::StackAllocatorDestructor> m_destructors;

		template<typename U>
		inline typename std::enable_if<std::is_trivially_destructible<U>::value>::type
			addDestructorToList(U* object)
		{
			//do nothing
		}

		template<typename U>
		inline typename std::enable_if<!std::is_trivially_destructible<U>::value>::type
			addDestructorToList(U* object)
		{
			m_destructors.add(INTERNAL::StackAllocatorDestructor(*object));
		}

	public:
		explicit StackAllocator(size_t size = STACK_ALLOCATOR_DEFAULT_SIZE, Allocator* parentAllocator = nullptr)
			: m_length(size), m_parentAllocator(parentAllocator) 
		{
			if (m_parentAllocator == nullptr)
			{
				m_parentAllocator = new Allocator();
				m_needsToDeleteParentAllocator = true;
			}
			m_data = m_parentAllocator->allocate(m_length);
			m_head = m_data;
		}

		~StackAllocator()
		{
			if (m_data != m_head)
			{
				debugBreak();
			}
			if (m_data != nullptr && m_parentAllocator != nullptr)
			{
				m_parentAllocator->deallocate(m_data, m_length);
			}
			if (m_needsToDeleteParentAllocator)
			{
				delete m_parentAllocator;
			}
			m_data = nullptr;
			m_head = nullptr;
		}

		StackAllocator(const StackAllocator&  other) = delete; //Copy Constructor
		StackAllocator(StackAllocator&& other) = delete; //Move Constructor
		StackAllocator& operator=(const StackAllocator&  other) = delete; //Copy Assignment
		StackAllocator& operator=(StackAllocator&& other) = delete; //Move Assignment

		template <typename U, typename... arguments>
		U* allocateObjects(size_t amountOfObjects = 1, arguments&&... args)
		{
			T* allocationLocation = (T*)Math::nextMultiple(alignof(U), (size_t)m_head);
			T* newHeadPointer = allocationLocation + amountOfObjects * sizeof(U);
			if (newHeadPointer <= m_data + m_length)
			{
				U* returnPointer = reinterpret_cast<U*>(allocationLocation);
				m_head = newHeadPointer;
				for (size_t i = 0; i < amountOfObjects; i++)
				{
					U* object = bbe::addressOf(returnPointer[i]);
					new (object) U(std::forward<arguments>(args)...);
					addDestructorToList(object);
				}
				return returnPointer;
			}
			else
			{
				throw AllocatorOutOfMemoryException();
			}
		}

		template <typename U, typename... arguments>
		U* allocateObject(arguments&&... args)
		{
			return allocateObjects<U>(1, std::forward<arguments>(args)...);
		}


		void* allocate(size_t amountOfBytes, size_t alignment = 1)
		{
			T* allocationLocation = (T*)Math::nextMultiple(alignment, (size_t)m_head);
			T* newHeadPointer = allocationLocation + amountOfBytes;
			if (newHeadPointer <= m_data + m_length)
			{
				m_head = newHeadPointer;
				return allocationLocation;
			}
			else
			{
				throw AllocatorOutOfMemoryException();
			}
		}

		StackAllocatorMarker<T> getMarker()
		{
			return StackAllocatorMarker<T>(m_head, m_destructors.getLength());
		}
		
		void deallocateToMarker(StackAllocatorMarker<T> sam)
		{
			m_head = sam.m_markerValue;
			while (m_destructors.getLength() > sam.m_destructorHandle)
			{
				m_destructors.last()();
				m_destructors.popBack();
			}
		}

		void deallocateAll()
		{
			m_head = m_data;
			while (m_destructors.size() > 0)
			{
				m_destructors.back()();
				m_destructors.pop_back();
			}
		}

	};
	

}
