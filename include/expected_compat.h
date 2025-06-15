#if __has_include(<expected>)
#  include <expected>
#endif

#if !defined(__cpp_lib_expected)
#  include "../dependencies/tl_expected.hpp"
namespace std {
template <class T, class E>
using expected = tl::expected<T, E>;

template <class E>
using unexpected = tl::unexpected<E>;

template <class E>
using bad_expected_access = tl::bad_expected_access<E>;
} // namespace std
#endif
