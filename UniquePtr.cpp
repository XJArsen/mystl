
#include <concepts>
#include <cstddef>
#include <utility>
template <class _Tp> struct DefaultDeleter {
  void operator()(_Tp *p) const { delete p; }
};
template <class _Tp> struct DefaultDeleter<_Tp[]> {
  void operator()(_Tp *p) const { delete[] p; }
};

template <class _Tp, class _Deleter = DefaultDeleter<_Tp>> struct UniquePtr {
private:
  _Tp *_M_p;
  _Deleter _M_deleter;

public:
  using element_type = _Tp;
  using pointer = _Tp *;
  using deleter_type = _Deleter;

  UniquePtr(std::nullptr_t = nullptr) noexcept : _M_p(nullptr) {}
  explicit UniquePtr(_Tp *p) noexcept : _M_p(p) {}

  template <class _Up, class _UDeleter>
    requires(std::convertible_to<_Up *, _Tp *>)
  UniquePtr(UniquePtr<_Up, _UDeleter> &&__that) noexcept : _M_p(__that._M_p) {
    __that._M_p = nullptr;
  }

  // 析构
  ~UniquePtr() noexcept {
    if (_M_p) {
      _M_deleter(_M_p);
    }
  }

  // 拷贝
  UniquePtr(UniquePtr const &__that) = delete;
  UniquePtr &operator=(UniquePtr const &__that) = delete;

  // 移动
  UniquePtr(UniquePtr &&__that) noexcept : _M_p(__that._M_p) {
    __that._M_p = nullptr;
  }

  UniquePtr &operator=(UniquePtr &&__that) noexcept {
    if (this != &__that) [[likely]] {
      if (_M_p) {
        _M_deleter(_M_p);
      }
      _M_p = std::exchange(__that._M_p, nullptr);
    }
    return *this;
  }

  void swap(UniquePtr &__that) noexcept { std::swap(_M_p, __that._M_p); }

  explicit operator bool() const noexcept { return _M_p != nullptr; }

  bool operator==(UniquePtr const &__that) const noexcept {
    return _M_p == __that._M_p;
  }

  bool operator!=(UniquePtr const &__that) const noexcept {
    return _M_p != __that._M_p;
  }

  bool operator<(UniquePtr const &__that) const noexcept {
    return _M_p < __that._M_p;
  }

  bool operator<=(UniquePtr const &__that) const noexcept {
    return _M_p <= __that._M_p;
  }

  bool operator>(UniquePtr const &__that) const noexcept {
    return _M_p > __that._M_p;
  }

  bool operator>=(UniquePtr const &__that) const noexcept {
    return _M_p >= __that._M_p;
  }
};

template <class _Tp, class... _Args,
          std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
UniquePtr<_Tp> makeUnique(_Args &&...__args) {
  return UniquePtr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

int main() { auto a = makeUnique<int>(2); }