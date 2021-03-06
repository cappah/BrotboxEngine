#pragma once

#include <string>
#include <cwchar>
#include <stdlib.h>
#include "../BBE/DynamicArray.h"
#include "../BBE/Array.h"
#include "../BBE/UtilDebug.h"
#include "../BBE/Hash.h"

namespace bbe
{
	template <typename T>
	class StringBase
	{
#define SSOSIZE (16)
		//TODO use allocators
	private:
		union
		{
			wchar_t *m_pdata;
			wchar_t m_ssoData[SSOSIZE];
		};
		bool   m_usesSSO  = true;
		size_t m_length   = 0;
		size_t m_capacity = 0;

		void growIfNeeded(size_t newSize) {
			if (getCapacity() < newSize) {
				size_t newCapa = newSize * 2;
				
				wchar_t *newData = new wchar_t[newCapa];
				wmemcpy(newData, getRaw(), getCapacity());

				if (!m_usesSSO) {
					delete[] m_pdata;
				}
				else {
					m_usesSSO = false;
				}
				
				m_capacity = newCapa;
				m_pdata = newData;
			}
		}

		void initializeFromWCharArr(const wchar_t *data)
		{
			//PO
			m_pdata = nullptr;
			m_capacity = 0;

			if (m_length == 0)
			{
				m_length = wcslen(data);
			}
			
			if (m_length < SSOSIZE)
			{
				wmemcpy(m_ssoData, data, m_length + 1);
				m_usesSSO = true;
				m_capacity = SSOSIZE;
			}
			else
			{
				m_pdata = new wchar_t[m_length + 1];
				wmemcpy(m_pdata, data, m_length + 1);
				m_usesSSO = false;
				m_capacity = m_length + 1;
			}
		}

		void initializeFromCharArr(const char *data)
		{
			//PO
			m_pdata = nullptr;
			m_capacity = 0;

			if (m_length == 0)
			{
				m_length = strlen(data);
			}
			
			if (m_length < SSOSIZE)
			{
				mbstowcs_s(0, m_ssoData, m_length + 1, data, m_length);
				m_usesSSO = true;
				m_capacity = SSOSIZE;
			}
			else
			{
				m_pdata = new wchar_t[m_length + 1];
				mbstowcs_s(0, m_pdata, m_length + 1, data, m_length);
				m_usesSSO = false;
				m_capacity = m_length + 1;
			}
			
		}

	public:
		StringBase()
		{
			initializeFromWCharArr(L"");
		}

		template<int size>
		StringBase(const Array<wchar_t, size>& arr)
		{
			//UNTESTED
			initializeFromWCharArr(arr.getRaw());
		}

		StringBase(const DynamicArray<wchar_t>& arr)
		{
			//UNTESTED
			initializeFromWCharArr(arr.getRaw());
		}

		template<int size>
		StringBase(const Array<char, size>& arr)
		{
			//UNTESTED
			initializeFromCharArr(arr.getRaw());
		}

		StringBase(const DynamicArray<char>& arr)
		{
			//UNTESTED
			initializeFromCharArr(arr.getRaw());
		}

		StringBase(const wchar_t *data)
		{
			//PO
			initializeFromWCharArr(data);
		}

		StringBase(const char* data)
		{
			initializeFromCharArr(data);
		}

		StringBase(const std::string &data)
		{
			m_length = data.length();
			initializeFromCharArr(data.c_str());
		}

		StringBase(const std::wstring &data)
		{
			m_length = data.length();
			initializeFromWCharArr(data.c_str());
		}

		StringBase(double number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(int number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(long long number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(long double number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(float number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(unsigned long long number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(unsigned long number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(long number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(unsigned int number)
		{
			initializeFromCharArr(std::to_string(number).c_str());
		}

		StringBase(const StringBase<T>&  other)//Copy Constructor
		{ 
			m_length = other.getLength();
			if (other.m_usesSSO)
			{
				initializeFromWCharArr(other.m_ssoData);
			}
			else
			{
				initializeFromWCharArr(other.m_pdata);
			}
		}

		StringBase(StringBase<T>&& other) //Move Constructor
		{
			m_length = other.m_length;
			m_usesSSO = other.m_usesSSO;
			if (other.m_usesSSO)
			{
				wmemcpy(m_ssoData, other.m_ssoData, SSOSIZE);
				m_capacity = SSOSIZE;
			}
			else
			{
				m_pdata = other.m_pdata;
				m_capacity = other.m_capacity;
				other.m_pdata = nullptr;
			}
			other.m_length = 0;
		}

		StringBase& operator=(const StringBase<T>&  other) //Copy Assignment
		{ 
			if (!m_usesSSO && m_pdata != nullptr)
			{
				delete[] m_pdata;
			}

			m_length = other.getLength();
			if (other.m_usesSSO)
			{
				initializeFromWCharArr(other.m_ssoData);
			}
			else
			{
				initializeFromWCharArr(other.m_pdata);
			}
			return *this;
		}

		StringBase& operator=(StringBase<T>&& other)//Move Assignment
		{ 
			if (!m_usesSSO && m_pdata != nullptr)
			{
				delete[] m_pdata;
			}
			
			m_length = other.m_length;
			m_usesSSO = other.m_usesSSO;

			if (other.m_usesSSO)
			{
				wmemcpy(m_ssoData, other.m_ssoData, SSOSIZE);
				m_capacity = SSOSIZE;
			}
			else
			{
				m_capacity = other.m_capacity;
				m_pdata = other.m_pdata;
			}
			
			other.m_pdata = nullptr;
			other.m_length = 0;
			return *this;
		}

		~StringBase()
		{
			if (!m_usesSSO && m_pdata != nullptr)
			{
				delete[] m_pdata;
				m_pdata = nullptr;
			}
		}

		bool operator==(const StringBase<T>& other) const
		{
			return wcscmp(getRaw(), other.getRaw()) == 0;
		}

		bool operator==(const wchar_t* arr) const
		{
			return wcscmp(getRaw(), arr) == 0;
		}

		bool operator==(const char* arr) const
		{
			StringBase<T> localString(arr);
			return operator==(localString);
		}

		bool operator==(const std::string& str) const
		{
			StringBase<T> localString(str);
			return operator==(localString);
		}

		bool operator==(const std::wstring& str) const
		{
			StringBase<T> localString(str);
			return operator==(localString);
		}

		friend bool operator==(const wchar_t* arr, const StringBase<T>& a)
		{
			return a.operator==(arr);
		}

		friend bool operator==(const char* arr, const StringBase<T>& a)
		{
			return a.operator==(arr);
		}

		friend bool operator==(std::string& str, const StringBase<T>& a)
		{
			return a.operator==(str);
		}

		friend bool operator==(std::wstring& str, const StringBase<T>& a)
		{
			return a.operator==(str);
		}

		bool operator!=(const StringBase<T>& other) const
		{
			return !operator==(other);
		}

		bool operator!=(const wchar_t* arr) const
		{
			return !operator==(arr);
		}

		bool operator!=(const char* arr) const
		{
			return !operator==(arr);
		}

		bool operator!=(const std::string& str) const
		{
			return !operator==(str);
		}

		bool operator!=(const std::wstring& str) const
		{
			return !operator==(str);
		}

		friend std::ostream &operator<<(std::ostream &os, const bbe::StringBase<T> &string)
		{
			return os << string.getRaw();
		}

		friend bool operator!=(const wchar_t* arr, const StringBase<T>& string)
		{
			return string.operator!=(arr);
		}

		friend bool operator!=(const char* arr, const StringBase<T>& string)
		{
			return string.operator!=(arr);
		}

		friend bool operator!=(const std::string& str, const StringBase<T>& string)
		{
			return string.operator!=(str);
		}

		friend bool operator!=(const std::wstring& str, const StringBase<T>& string)
		{
			return string.operator!=(str);
		}

		StringBase<T> operator+(const StringBase<T>& other) const
		{
			//PO
			size_t totalLength = m_length + other.m_length;
			StringBase<T> retVal;
			retVal.m_length = totalLength;

			if (totalLength < SSOSIZE)
			{
				memcpy(retVal.m_ssoData, getRaw(), sizeof(wchar_t) * m_length);
				memcpy(retVal.m_ssoData + m_length, other.getRaw(), sizeof(wchar_t) * other.m_length);
				retVal.m_ssoData[totalLength] = 0;
			}
			else
			{
				wchar_t *newData = new wchar_t[totalLength + 1];
				memcpy(newData, getRaw(), sizeof(wchar_t) * m_length);
				memcpy(newData + m_length, other.getRaw(), sizeof(wchar_t) * other.m_length);
				newData[totalLength] = 0;

				retVal.m_usesSSO = false;
				retVal.m_pdata = newData;
			}
			return retVal;
		}

		StringBase<T> operator+(const std::string& other) const
		{
			return operator+(StringBase<T>(other));
		}

		StringBase<T> operator+(const std::wstring& other) const
		{
			return operator+(StringBase<T>(other));
		}

		StringBase<T> operator+(const wchar_t* other) const
		{
			return operator+(StringBase<T>(other));
		}

		StringBase<T> operator+(const char* other) const
		{
			return operator+(StringBase<T>(other));
		}

		StringBase<T> operator+(double number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(int number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(long long number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(long double number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(float number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(unsigned long long number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(unsigned long number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(long number) const
		{
			return operator+(StringBase<T>(number));
		}

		StringBase<T> operator+(unsigned int number) const
		{
			return operator+(StringBase<T>(number));
		}

		friend StringBase<T> operator+(const std::string& other, const StringBase<T>& string)
		{
			return StringBase<T>(other) + string;
		}

		friend StringBase<T> operator+(const std::wstring& other, const StringBase<T>& string)
		{
			return StringBase<T>(other) + string;
		}

		friend StringBase<T> operator+(const wchar_t* other, const StringBase<T>& string)
		{
			return StringBase<T>(other) + string;
		}

		friend StringBase<T> operator+(const char* other, const StringBase<T>& string)
		{
			return StringBase<T>(other) + string;
		}

		friend StringBase<T> operator+(double number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(int number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(long long number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(long double number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(float number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(unsigned long long number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(unsigned long number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(long number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		friend StringBase<T> operator+(unsigned int number, const StringBase<T>& string)
		{
			return StringBase<T>(number) + string;
		}

		StringBase<T>& operator+=(const StringBase<T>& other)
		{
			size_t totalLength = m_length + other.m_length;
			size_t oldLength = m_length;
			m_length = totalLength;

			if (totalLength < SSOSIZE) {
				memcpy(m_ssoData + oldLength, other.getRaw(), sizeof(wchar_t) * other.m_length);
				m_ssoData[totalLength] = 0;
			}
			else {
				growIfNeeded(totalLength + 1);
				memcpy(getRaw() + oldLength, other.getRaw(), sizeof(wchar_t) * other.m_length);
				m_pdata[totalLength] = 0;
			}
			return *this;
		}

		StringBase<T>& operator+=(const std::string& other)
		{
			return operator+=(bbe::StringBase<T>(other));
		}

		StringBase<T>& operator+=(const std::wstring& other)
		{
			return operator+=(bbe::StringBase<T>(other));
		}

		StringBase<T>& operator+=(const wchar_t* other)
		{
			return operator+=(bbe::StringBase<T>(other));
		}

		StringBase<T>& operator+=(const char* other)
		{
			return operator+=(bbe::StringBase<T>(other));
		}

		StringBase<T>& operator+=(double number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(int number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(long long number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(long double number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(float number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(unsigned long long number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(unsigned long number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(long number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		StringBase<T>& operator+=(unsigned int number)
		{
			return operator+=(bbe::StringBase<T>(number));
		}

		void trim()
		{
			size_t start = 0;
			size_t end = m_length - 1;
			auto raw = getRaw();

			while (iswspace(raw[start]))
			{
				start++;
			}
			while (iswspace(raw[end]) && end != 0)
			{
				end--;
			}

			if (start != 0 || end != m_length)
			{
				if (end == 0) //Special Case, if the string only contains whitespace
				{ 
					m_length = 0;
					raw[m_length] = 0;
				}
				else
				{
					m_length = end - start + 1;
					memmove(raw, &raw[start], sizeof(wchar_t) * m_length);
					raw[m_length] = 0;
				}
			}
		}

		size_t count(const StringBase<T>& countand) const
		{
			size_t countandLength = countand.getLength();
			if (countandLength == 0)
			{
				return 0;
			}
			size_t amount = 0;
			const wchar_t *readHead = getRaw();

			while ((readHead = wcsstr(readHead, countand.getRaw())) != nullptr)
			{
				amount++;
				readHead += countandLength;
			}
			return amount;
		}

		size_t count(const wchar_t* countand) const
		{
			return count(StringBase<T>(countand));
		}

		size_t count(char* countand) const
		{
			return count(StringBase<T>(countand));
		}

		size_t count(std::string countand) const
		{
			return count(StringBase<T>(countand));
		}

		size_t count(std::wstring countand) const
		{
			return count(StringBase<T>(countand));
		}

		DynamicArray<StringBase<T>> split(const StringBase<T>& splitAt) const
		{
			//TODO this method is a little mess. Clean it up!
			size_t counted = count(splitAt);
			if (counted == 0)
			{
				DynamicArray<StringBase<T>> retVal(1);
				retVal[0] = getRaw();
				return retVal;
			}
			DynamicArray<StringBase<T>> retVal(counted + 1);
			const wchar_t *previousFinding = getRaw();
			for (size_t i = 0; i < retVal.getLength() - 1; i++)
			{
				const wchar_t *currentFinding = wcsstr(previousFinding, splitAt.getRaw());
				StringBase<T> currentString;
				size_t currentStringLength = currentFinding - previousFinding;
				currentString.m_usesSSO = false; //TODO make this better! current string could use SSO!
				currentString.m_pdata = new wchar_t[currentStringLength + 1];
				memcpy(currentString.m_pdata, previousFinding, currentStringLength * sizeof(wchar_t));
				currentString.m_pdata[currentStringLength] = 0;
				currentString.m_length = currentStringLength;

				retVal[i] = currentString;

				previousFinding = currentFinding + splitAt.getLength();
			}

			StringBase<T> currentString;
			size_t currentStringLength = m_pdata + m_length - previousFinding;
			currentString.m_usesSSO = false; //TODO make this better! current string could use SSO!
			currentString.m_pdata = new wchar_t[currentStringLength + 1];
			memcpy(currentString.m_pdata, previousFinding, currentStringLength * sizeof(wchar_t));
			currentString.m_pdata[currentStringLength] = 0;
			currentString.m_length = currentStringLength;
			retVal[retVal.getLength() - 1] = currentString;

			return retVal;
		}

		DynamicArray<StringBase<T>> split(const wchar_t* splitAt) const
		{
			return split(StringBase<T>(splitAt));
		}

		DynamicArray<StringBase<T>> split(const char* splitAt) const
		{
			return split(StringBase<T>(splitAt));
		}

		DynamicArray<StringBase<T>> split(std::string splitAt) const
		{
			return split(StringBase<T>(splitAt));
		}

		DynamicArray<StringBase<T>> split(std::wstring splitAt) const
		{
			return split(StringBase<T>(splitAt));
		}

		bool contains(const wchar_t* string) const
		{
			return contains(StringBase<T>(string));
		}

		bool contains(const char* string) const
		{
			return contains(StringBase<T>(string));
		}

		bool contains(const std::string& string) const
		{
			return contains(StringBase<T>(string));
		}

		bool contains(const std::wstring& string) const
		{
			return contains(StringBase<T>(string));
		}

		bool contains(const StringBase<T>& string) const
		{
			return wcsstr(getRaw(), string.getRaw()) != nullptr;
		}

		int64_t search(const wchar_t* string) const
		{
			return search(StringBase<T>(string));
		}

		int64_t search(const char* string) const
		{
			return search(StringBase<T>(string));
		}

		int64_t search(const std::string& string) const
		{
			return search(StringBase<T>(string));
		}

		int64_t search(const std::wstring& string) const
		{
			return search(StringBase<T>(string));
		}

		int64_t search(const StringBase<T>& string) const
		{
			const wchar_t *found = wcsstr(getRaw(), string.getRaw());
			if (found == nullptr)
			{
				return -1;
			}

			return found - getRaw();
		}

		long toLong(int base = 10)
		{
			return wcstol(getRaw(), nullptr, base);
		}

		double toDouble()
		{
			return wcstod(getRaw(), nullptr);
		}

		float toFloat()
		{
			return wcstof(getRaw(), nullptr);
		}

		wchar_t& operator[](size_t index)
		{
			if (index >= m_length)
			{
				debugBreak();
			}
			return getRaw()[index];
		}

		const wchar_t& operator[](size_t index) const
		{
			if (index >= m_length)
			{
				debugBreak();
			}
			return getRaw()[index];
		}

		wchar_t* getRaw()
		{
			if (m_usesSSO)
			{
				return m_ssoData;
			}
			else
			{
				return m_pdata;
			}
		}

		const wchar_t* getRaw() const
		{
			if (m_usesSSO)
			{
				return m_ssoData;
			}
			else
			{
				return m_pdata;
			}
		}

		void toUpperCase()
		{
			auto raw = getRaw();
			for (size_t i = 0; i < m_length; i++)
			{
				raw[i] = towupper(raw[i]);
			}
		}

		void toLowerCase()
		{
			auto raw = getRaw();
			for (size_t i = 0; i < m_length; i++)
			{
				raw[i] = towlower(raw[i]);
			}
		}

		size_t getLength() const
		{
			return m_length;
		}

		size_t getCapacity() const
		{
			return m_capacity;
		}
	};

	typedef StringBase<int> String;

	template<>
	uint32_t hash(const String &t);
}
