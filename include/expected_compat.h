#if __has_include(<expected>)
#  include <expected>
#endif

#if !defined(__cpp_lib_expected)
#  include "../dependencies/tl_expected.hpp"
namespace std {
template <class T, class E>
using expected = tl::expected<T, E>;

template <class E>
struct unexpected : tl::unexpected<E> {
  using tl::unexpected<E>::unexpected;
};
template <class E>
unexpected(E) -> unexpected<E>;

template <class E>
using bad_expected_access = tl::bad_expected_access<E>;
} // namespace std
#endif
