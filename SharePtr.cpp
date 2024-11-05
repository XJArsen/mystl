#include <cstddef>

struct _SpCounter {};

template <class _Tp, class _Deleter>
struct _SpCounterImpl final : _SpCounter {};

template <class _Tp, class _Deleter>
struct _SpCounterImplFused final : _SpCounter {};

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
};

int main() {}
