#ifndef TISP_COMMON_UTILS_HPP
#define TISP_COMMON_UTILS_HPP

#include <TISP_types.hpp>
#include <stdlib.h>

namespace TISP {

template <dmaDir_t dmaDir> inline void setPingPongFlag(bool &pingPongFlagIn, bool &pingPongFlagOut)
{

   if (dmaDir == DMA_IN) {
      pingPongFlagIn ^= 1;
   }

   if (dmaDir == DMA_OUT) {
      pingPongFlagOut ^= 1;
   }

   if (dmaDir == DMA_IN_OUT) {
      pingPongFlagIn ^= 1;
      pingPongFlagOut ^= 1;
   }
}

} // namespace TISP

#endif
