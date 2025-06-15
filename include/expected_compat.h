#if __has_include(<expected>)
#include <expected>
#else
#include "../dependencies/tl_expected.hpp"
namespace std {
    using tl::expected;
    using tl::unexpected;
    template <class E>
    using bad_expected_access = tl::bad_expected_access<E>;
}
#endif
