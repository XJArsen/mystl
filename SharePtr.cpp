struct _SpCounter {};
template <class _Tp, class _Deleter>
struct _SpCounterImpl final : _SpCounter {};

template <class _Tp, class _Deleter>
struct _SpCounterImplFused final : _SpCounter {};

template <class _Tp>
struct SharedPtr {};
int main() {}