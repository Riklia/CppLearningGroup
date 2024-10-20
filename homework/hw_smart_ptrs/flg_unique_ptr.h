#pragma once
#include <utility>

namespace flg_hw
{
	// for non-array types
	template <typename T>
	class unique_ptr
	{
	public:
		unique_ptr() noexcept: _raw(nullptr) {}
		unique_ptr(nullptr_t) noexcept : _raw(nullptr) {}
		// we cannot just have unique_ptr(unique_ptr<T>&& r_unique_ptr) because such smart ptr
		// implementation doesn't work with polymorphism
		template <typename V, std::enable_if_t<std::is_convertible<V*, T*>::value, int> = 0>
		explicit unique_ptr(V* raw) noexcept: _raw(raw) {}
		template <typename V, std::enable_if_t<std::is_convertible<V*, T*>::value, int> = 0>
		unique_ptr(unique_ptr<V>&& r_unique_ptr) noexcept { _raw = r_unique_ptr.release(); }
		unique_ptr(unique_ptr&& r_unique_ptr) noexcept { _raw = r_unique_ptr.release(); }

		T* release()
		{
			T* tmp = _raw;
			_raw = nullptr;
			return tmp;
		}
		~unique_ptr() { delete _raw; }

		T& operator*() const noexcept { return *_raw; }
		T* operator->() const noexcept { return _raw; }
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;
		unique_ptr& operator=(unique_ptr&& other) noexcept
		{
			if (this != &other)
			{
				delete _raw;
				_raw = other.release();
			}
			return *this;
		}
	private:
		
		T* _raw;
	};

	// for arrays
	template <typename T>
	class unique_ptr<T[]>
	{
	public:
		unique_ptr() noexcept : _raw(nullptr) {}
		unique_ptr(nullptr_t) noexcept : _raw(nullptr) {}
		explicit unique_ptr(T* raw) noexcept : _raw(raw) {}
		unique_ptr(unique_ptr&& r_unique_ptr) noexcept { _raw = r_unique_ptr.release(); }
		T* release()
		{
			T* tmp = _raw;
			_raw = nullptr;
			return tmp;
		}
		~unique_ptr() { delete[] _raw; }
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;
		unique_ptr& operator=(unique_ptr&& other) noexcept
		{
			if (this != &other)
			{
				delete[] _raw;
				_raw = other.release();
			}
			return *this;
		}
		T& operator[](size_t index) const noexcept { return _raw[index]; }
	private:

		T* _raw;
		template <typename V> friend class unique_ptr;

	};

	// for non-array types
	template <typename T, typename... Types, std::enable_if_t<!std::is_array_v<T>, int> = 0>
	unique_ptr<T> make_unique(Types&&... args) { return unique_ptr<T>(new T(std::forward<Types>(args)...)); }
	// for array types (std::extent_v<T> == 0 used to ensure that, for example, T is not int[10], but int[]) 
	template <typename T, std::enable_if_t<std::is_array_v<T> && std::extent_v<T> == 0, int> = 0>
	unique_ptr<T> make_unique(size_t size) { return unique_ptr<T>(new std::remove_extent_t<T>[size]()); }
}
