#pragma once

namespace flg_hw
{
	class _ptr_control_block
	{
	public:
		_ptr_control_block() noexcept : _counter(0) {}
		void increment() noexcept { _counter++; }
		void decrement() noexcept { _counter--; }
		int get_count() const noexcept { return _counter; }
	private:
		int _counter;
	};

	template <typename T>
	class shared_ptr
	{
	public:
		shared_ptr() noexcept : _raw(nullptr), _controller(nullptr) {}
		shared_ptr(nullptr_t) noexcept : _raw(nullptr), _controller(nullptr) {}
		template <typename V, std::enable_if_t<std::is_convertible<V*, T*>::value, int> = 0>
		explicit shared_ptr(V* raw) noexcept : _raw(raw), _controller(new _ptr_control_block()) { _controller->increment(); }
		shared_ptr(const shared_ptr& other) noexcept : _raw(other._raw), _controller(other._controller)
		{
			_controller->increment();
		}
		template <typename V, std::enable_if_t<std::is_convertible<V*, T*>::value, int> = 0>
		shared_ptr(shared_ptr<V>&& other) noexcept : _raw(other._raw), _controller(other._controller)
		{
			other._raw = nullptr;
			other._controller = nullptr;
		}
		shared_ptr(shared_ptr&& other) noexcept : _raw(other._raw), _controller(other._controller)
		{
			other._raw = nullptr;
			other._controller = nullptr;
		}
		void reset() noexcept
		{
			if (_controller)
			{
				_controller->decrement();
				if (!_controller->get_count())
				{
					delete _raw;
					delete _controller;
				}
			}
			_raw = nullptr;
			_controller = nullptr;
		}
		~shared_ptr()
		{
			reset();
		}
		T& operator*() const noexcept { return *_raw; }
		T* operator->() const noexcept { return _raw; }
		shared_ptr& operator=(shared_ptr&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				_raw = other._raw;
				_controller = other._controller;
				other._raw = nullptr;
				other._controller = nullptr;
			}
			return *this;
		}

	private:
		T* _raw;
		_ptr_control_block* _controller;
		template <typename V> friend class shared_ptr;
	};

	template <typename T>
	class shared_ptr<T[]>
	{
	public:
		shared_ptr() noexcept : _raw(nullptr), _controller(nullptr) {}
		shared_ptr(nullptr_t) noexcept : _raw(nullptr), _controller(nullptr) {}
		explicit shared_ptr(T* raw) noexcept : _raw(raw), _controller(new _ptr_control_block()) { _controller->increment(); }
		shared_ptr(const shared_ptr& other) noexcept : _raw(other._raw), _controller(other._controller)
		{
			_controller->increment();
		}
		shared_ptr(shared_ptr&& other) noexcept : _raw(other._raw), _controller(other._controller)
		{
			other._raw = nullptr;
			other._controller = nullptr;
		}
		void reset() noexcept
		{
			if (_controller)
			{
				_controller->decrement();
				if (!_controller->get_count())
				{
					delete[] _raw;
					delete _controller;
				}
			}
			_raw = nullptr;
			_controller = nullptr;
		}
		~shared_ptr()
		{
			reset();
		}
		T& operator*() const noexcept { return *_raw; }
		T* operator->() const noexcept { return _raw; }
		shared_ptr& operator=(shared_ptr&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				_raw = other._raw;
				_controller = other._controller;
				other._raw = nullptr;
				other._controller = nullptr;
			}
			return *this;
		}

	private:
		T* _raw;
		_ptr_control_block* _controller;
		template <typename V> friend class shared_ptr;
	};

	// for non-array types
	template <typename T, typename... Types, std::enable_if_t<!std::is_array_v<T>, int> = 0>
	shared_ptr<T> make_shared(Types&&... args) { return shared_ptr<T>(new T(std::forward<Types>(args)...)); }
	// for array types
	template <typename T, std::enable_if_t<std::is_array_v<T>&& std::extent_v<T> == 0, int> = 0>
	shared_ptr<T> make_shared(size_t size) { return shared_ptr<T>(new std::remove_extent_t<T>[size]()); }
}