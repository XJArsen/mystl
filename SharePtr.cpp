#include "UniquePtr.hpp"
#include <atomic>
#include <cstddef>
#include <sys/cdefs.h>
#include <type_traits>

struct _SpCounter {
    std::atomic_int64_t _M_refcnt;

    _SpCounter() noexcept : _M_refcnt(1) {};
    _SpCounter(_SpCounter &&) = delete;

    void _M_incref() noexcept {
        _M_refcnt.fetch_add(1, std::memory_order_relaxed);
    }

    void _M_decref() noexcept {
        if (_M_refcnt.fetch_sub(1, std::memory_order_relaxed) == 1) {
            delete this;
        }
    }

    long _M_cntref() const noexcept {
        return _M_refcnt.load(std::memory_order_relaxed);
    }

    virtual ~_SpCounter() = default;
};

template <class _Tp, class _Deleter>
struct _SpCounterImpl final : _SpCounter {
    _Tp *_M_ptr;
    [[no_unique_address]] _Deleter _M_deleter;

    explicit _SpCounterImpl(_Tp *__ptr) noexcept : _M_ptr(__ptr) {};

    explicit _SpCounterImpl(_Tp *__ptr, _Deleter __deleter) noexcept
        : _M_ptr(__ptr),
          _M_deleter(std::move(__deleter)) {}

    ~_SpCounterImpl() noexcept override {
        _M_deleter(_M_ptr);
    }
};

template <class _Tp, class _Deleter>
struct _SpCounterImplFused final : _SpCounter {
    _Tp *_M_ptr;
    void *_M_mem;
    [[no_unique_address]] _Deleter _M_deleter;

    explicit _SpCounterImplFused(_Tp *__ptr, void *__mem,
                                 _Deleter __deleter) noexcept
        : _M_ptr(__ptr),
          _M_mem(__mem),
          _M_deleter(std::move(__deleter)) {}

    ~_SpCounterImplFused() noexcept {
        _M_deleter(_M_ptr);
    }
};

template <class _Tp>
struct SharedPtr {
private:
    _Tp *_M_p;
    _SpCounter *_M_owner;
    template <class>
    friend struct SharedPtr;

    explicit SharedPtr(_Tp *__ptr, _SpCounter *__owner) noexcept
        : _M_p(__ptr),
          _M_owner(__owner) {}

public:
    using element_type = _Tp;
    using pointer = _Tp *;

    SharedPtr(std::nullptr_t = nullptr) noexcept : _M_owner(nullptr) {}

    // 构造
    template <class _Yp>
        requires std::is_convertible_v<_Yp *, _Tp *>
    explicit SharedPtr(_Yp *__ptr)
        : _M_p(__ptr),
          _M_owner(new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr)) {
        _S_setupEnableSharedFromThis(_M_p, _M_owner);
    }

    template <class _Yp, class _Deleter>
        requires std::is_convertible_v<_Yp *, _Tp *>
    explicit SharedPtr(_Yp *__ptr, _Deleter __deleter)
        : _M_p(__ptr),
          _M_owner(
              new _SpCounterImpl<_Yp, _Deleter>(__ptr, std::move(__deleter))) {
        _S_setupEnableSharedFromThis(_M_p, _M_owner);
    }

    template <class _Yp, class _Deleter>
        requires std::is_convertible_v<_Yp *, _Tp *>
    explicit SharedPtr(UniquePtr<_Yp, _Deleter> &&__ptr)
        : SharedPtr(__ptr.release(), __ptr.get_deleter()) {}

    template <class _Yp>
    inline friend SharedPtr<_Yp>
    _S_makeSharedFused(_Yp *__ptr, _SpCounter *__owner) noexcept;

    SharedPtr(SharedPtr const &__that) noexcept
        : _M_p(__that._M_ptr),
          _M_owner(__that._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref();
        }
    }

    template <class _Yp>
        requires std::is_convertible_v<_Yp *, _Tp *>
    SharedPtr(SharedPtr<_Yp> const &__that) noexcept
        : _M_p(__that._M_ptr),
          _M_owner(__that._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref();
        }
    }

    SharedPtr(SharedPtr &&__that) noexcept
        : _M_p(__that._M_ptr),
          _M_owner(__that._M_owner) {
        __that._M_ptr = nullptr;
        __that._M_owner = nullptr;
    }

    SharedPtr &operator=(SharedPtr const &__that) noexcept {
        if (this == &__that) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_p = __that._M_p;
        _M_owner = __that._M_owner;
        if (_M_owner) {
            _M_owner->_M_incref();
        }
        return *this;
    }

    SharedPtr &operator=(SharedPtr &&__that) noexcept {
        if (this == &__that) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_p = __that._M_p;
        _M_owner = __that._M_owner;
        __that._M_ptr = nullptr;
        __that._M_owner = nullptr;
        return *this;
    }

    template <class _Yp>
        requires std::is_convertible_v<_Yp *, _Tp *>
    SharedPtr &operator=(SharedPtr<_Yp> const &__that) noexcept {
        if (this == &__that) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_p = __that._M_p;
        _M_owner = __that._M_owner;
        if (_M_owner) {
            _M_owner->_M_incref();
        }
        return *this;
    }

    template <class _Yp>
        requires std::is_convertible_v<_Yp *, _Tp *>
    SharedPtr &operator=(SharedPtr<_Yp> &&__that) noexcept {
        if (this == &__that) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_p = __that._M_ptr;
        _M_owner = __that._M_owner;
        __that._M_p = nullptr;
        __that._M_owner = nullptr;
        return *this;
    }
};

template <class _Tp>
struct EnableSharedFromThis {
private:
    _SpCounter *_M_owner;
};

template <class _Tp>
void _S_setupEnableSharedFromThis(_Tp *__ptr, _SpCounter *__owner) {
    _S_setEnableSharedFromThisOwner(
        static_cast<EnableSharedFromThis<_Tp> *>(__ptr), __owner);
}

int main() {}
