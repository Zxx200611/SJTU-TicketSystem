#pragma once

namespace utils {

template <typename T, typename Compare>
T *lowerBound(T *s, T *t, T v, Compare comp) {
  int l = 0, r = t - s - 1, m = l, res = r + 1;
  while (l <= r) {
    m = l + (r - l) / 2;
    if (!comp(s[m], v))
      res = m, r = m - 1;
    else
      l = m + 1;
  }
  return s + res;
}

} // namespace utils