
/**
 * @file    typespec.hpp
 * @brief   class for type specialization with template
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TYPESPEC_HPP_
#define _OPENEDGE_CORE_TYPESPEC_HPP_

#include <type_traits>

template <typename _Type>
    using SignedOnly = typename std::enable_if<std::is_signed<_Type>::value>::value*;
template <typename _Type>
    using UnsignedOnly = typename std::enable_if<std::is_unsigned<_Type>::value>::value*;


#endif