#pragma once

#include "StackAllocator.h"
#include <iostream>
#include <string>
#include "UtilTest.h"

namespace bbe {
	namespace test {
		void testStackAllocator() {
			bbe::StackAllocator<> sa(sizeof(Person) * 128);
			std::cout << alignof(Person) << " " << sizeof(Person);
			auto startMarker = sa.getMarker();
			Person* pOut1 = sa.allocateObject<Person>();
			Person* pOut2 = sa.allocateObject<Person>();
			Person* pOut3 = sa.allocateObject<Person>();
			Person* pOut4 = sa.allocateObject<Person>();
			Person* pOut5 = sa.allocateObject<Person>();
			for (int i = 0; i < 512; i++) {
				auto marker = sa.getMarker();
				Person* person1 = sa.allocateObject<Person>();
				Person* person2 = sa.allocateObject<Person>();
				Person* person3 = sa.allocateObject<Person>();
				Person* person4 = sa.allocateObject<Person>();
				Person* person5 = sa.allocateObject<Person>();

				person1->name = "Hugo";
				person2->name = "Ebert";
				person3->name = "Lel";
				person4->name = "Aha?";
				person5->name = "Okay";

				person1->adress = "AStr";
				person2->adress = "BStr";
				person3->adress = "CStr";
				person4->adress = "DStr";
				person5->adress = "EStr";

				person1->age = 1;
				person2->age = 2;
				person3->age = 3;
				person4->age = 4;
				person5->age = 5;

				assertEquals(person1->name, "Hugo");
				assertEquals(person2->name, "Ebert");
				assertEquals(person3->name, "Lel");
				assertEquals(person4->name, "Aha?");
				assertEquals(person5->name, "Okay");

				assertEquals(person1->adress, "AStr");
				assertEquals(person2->adress, "BStr");
				assertEquals(person3->adress, "CStr");
				assertEquals(person4->adress, "DStr");
				assertEquals(person5->adress, "EStr");

				assertEquals(person1->age, 1);
				assertEquals(person2->age, 2);
				assertEquals(person3->age, 3);
				assertEquals(person4->age, 4);
				assertEquals(person5->age, 5);

				sa.deallocateToMarker(marker);
			}

			

			sa.deallocateToMarker(startMarker);
			Person::checkIfAllPersonsWereDestroyed();

			for (int i = 0; i < 128; i++) {
				Person* inner = sa.allocateObject<Person>();
				std::cout << i << " " << inner << std::endl;
				assertUnequals(inner, nullptr);
			}
			assertEquals(sa.allocateObject<Person>(), nullptr);
			sa.deallocateToMarker(startMarker);
			Person::checkIfAllPersonsWereDestroyed();
			std::cout << "End of StackAllocatorTest" << std::endl;
		}
	}
}