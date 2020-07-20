#ifndef FWK_FUNCTION_UTIL_H
#define FWK_FUNCTION_UTIL_H

// -*- C++ -*-
// author: afiq anuar
// short: a listing of free convenience functions for use in the framework

#include <type_traits>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

// fillers for the histogram class
template <typename ...Groups>
auto filler_count(const Groups &...groups)
{
  static_assert(sizeof...(groups) > 0 and sizeof...(groups) < 4, "ERROR: filler_count: currently only 1D - 3D histograms are supported!!");

  using Hist = typename std::conditional<sizeof...(groups) != 1, typename std::conditional<sizeof...(groups) != 2, TH3, TH2>::type, TH1>::type;

  return [&groups...] (Hist *hist, const double &weight) {
    hist->Fill(groups.n_elements()..., weight);
  };
}



template <typename Group, typename ...Attributes>
auto filler_first_of(const Group &group, Attributes &&...attrs)
{
  static_assert(sizeof...(attrs) > 0 and sizeof...(attrs) < 4, "ERROR: filler_first_of: currently only 1D - 3D histograms are supported!!");

  using Hist = typename std::conditional<sizeof...(attrs) != 1, typename std::conditional<sizeof...(attrs) != 2, TH3, TH2>::type, TH1>::type;

  return [&group, attrs...] (Hist *hist, const double &weight) {
    std::visit([&hist, &weight, &indices = group.ref_to_indices()] (const auto &...vec) {
        if (indices.size()) 
          hist->Fill(vec[indices[0]]..., weight); 
      }, group(attrs)...);
  };
}



template <typename Group, typename ...Attributes>
auto filler_all_of(const Group &group, Attributes &&...attrs)
{
  static_assert(sizeof...(attrs) > 0 and sizeof...(attrs) < 4, "ERROR: filler_all_of: currently only 1D - 3D histograms are supported!!");

  using Hist = typename std::conditional<sizeof...(attrs) != 1, typename std::conditional<sizeof...(attrs) != 2, TH3, TH2>::type, TH1>::type;

  return [&group, attrs...] (Hist *hist, const double &weight) {
    std::visit([&hist, &weight, &indices = group.ref_to_indices()] (const auto &...vec) {
        for (uint iE = 0; iE < indices.size(); ++iE)
          hist->Fill(vec[indices[iE]]..., weight);
      }, group(attrs)...);
  };
}



// a or b or c or ... in function form
// call any_of<N> to get a function that takes N bools and return the OR of them all
template <typename ...Bools>
bool any_of_impl(Bools ...bools)
{
  return (bools or ...);
}



template <size_t ...N>
auto any_of_helper(std::index_sequence<N...>) -> bool(*)(typename std::tuple_element<N, std::array<boolean, sizeof...(N)>>::type...)
{
  return any_of_impl<typename std::tuple_element<N, std::array<boolean, sizeof...(N)>>::type...>;
}



template <size_t N = 1>
auto any_of() -> decltype(any_of_helper(std::make_index_sequence<N>{}))
{
  return any_of_helper(std::make_index_sequence<N>{});
}



// a and b and c and ... in function form
// call all_of<N> to get a function that takes N bools and return the AND of them all
template <typename ...Bools>
bool all_of_impl(Bools ...bools)
{
  return (bools and ...);
}



template <size_t ...N>
auto all_of_helper(std::index_sequence<N...>) -> bool(*)(typename std::tuple_element<N, std::array<boolean, sizeof...(N)>>::type...)
{
  return all_of_impl<typename std::tuple_element<N, std::array<boolean, sizeof...(N)>>::type...>;
}



template <size_t N = 1>
auto all_of() -> decltype(all_of_helper(std::make_index_sequence<N>{}))
{
  return all_of_helper(std::make_index_sequence<N>{});
}



template <typename V>
int index_with_key(const std::vector<std::pair<std::string, V>> &vec, const std::string &key)
{
  for (int iE = 0; iE < vec.size(); ++iE) {
    auto &[alias, _] = vec[iE];
    (void) _;

    if (alias == key)
      return iE;
  }

  return -1;
}



template <typename T = float>
constexpr T identity(const T &t)
{
  return t;
}

#endif
