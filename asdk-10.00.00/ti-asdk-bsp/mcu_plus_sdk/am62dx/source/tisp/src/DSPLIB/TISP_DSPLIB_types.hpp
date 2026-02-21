#ifndef _TISP_DSPLIB_TYPES_HPP_
#define _TISP_DSPLIB_TYPES_HPP_

#include "dsplib.h"
#include <stdint.h>

template <int32_t dType> struct DSPLIB_element_type {
   using type = uint8_t;
};

template <> struct DSPLIB_element_type<DSPLIB_UINT8> {
   using type = uint8_t;
};

template <> struct DSPLIB_element_type<DSPLIB_INT8> {
   using type = int8_t;
};

template <> struct DSPLIB_element_type<DSPLIB_UINT16> {
   using type = uint16_t;
};

template <> struct DSPLIB_element_type<DSPLIB_INT16> {
   using type = int16_t;
};

template <> struct DSPLIB_element_type<DSPLIB_UINT32> {
   using type = uint32_t;
};

template <> struct DSPLIB_element_type<DSPLIB_INT32> {
   using type = int32_t;
};

template <typename T> struct DSPLIB_element_dType {
   static constexpr size_t value = DSPLIB_INT8;
};

template <> struct DSPLIB_element_dType<int8_t> {
   static constexpr size_t value = DSPLIB_INT8;
};

template <> struct DSPLIB_element_dType<int16_t> {
   static constexpr size_t value = DSPLIB_INT16;
};

template <> struct DSPLIB_element_dType<int32_t> {
   static constexpr size_t value = DSPLIB_INT32;
};

template <> struct DSPLIB_element_dType<float> {
   static constexpr size_t value = DSPLIB_FLOAT32;
};

template <> struct DSPLIB_element_dType<double> {
   static constexpr size_t value = DSPLIB_FLOAT64;
};

template <> struct DSPLIB_element_dType<uint8_t> {
   static constexpr size_t value = DSPLIB_UINT8;
};

template <> struct DSPLIB_element_dType<uint16_t> {
   static constexpr size_t value = DSPLIB_UINT16;
};

template <> struct DSPLIB_element_dType<uint32_t> {
   static constexpr size_t value = DSPLIB_UINT32;
};

#endif
