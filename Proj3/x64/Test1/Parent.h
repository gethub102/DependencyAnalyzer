#ifndef SCOPESTACK_H
#define SCOPESTACK_H
#pragma once


#include "Child.h"
#include "Invalid.h"
#include <list>


namespace parent1
{
	namespace parent2
	{
		template<typename T>
		class Parent {
		public:
			void push(const T& item);
			T pop();
		private:
			std::list<T> stack;
			ChildTest::Child child;
		};


		template<typename T>
		void Parent<T>::push(const T& item)
		{
			stack.push_back(item);
		}

		template<typename T>
		T Parent<T>::pop()
		{
			T item = stack.back();
			stack.pop_back();
			return item;
		}
	}

}
#endif
