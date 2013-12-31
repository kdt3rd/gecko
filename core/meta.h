
#pragma once

template<typename A, typename B>
using disable_if_same_or_derived = typename std::enable_if<!std::is_base_of<A,typename std::remove_reference<B>::type>::value>::type;

