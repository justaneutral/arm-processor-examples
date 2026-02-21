#ifndef TISP_TYPES_HPP
#define TISP_TYPES_HPP

namespace TISP {

/* typedef std::vector<virtualOp *> opVec; */

/* typedef std::list<virtualOp *> opList; */
/* class dmaChOffset { */
/*  public: */
/*    static size_t globalChOffset; */
/* }; */

/* typedef std::vector<TISP::SuperNode::superNode *> superNodeVec; */
typedef enum { NO_DMA = 0, DMA_IN, DMA_OUT, DMA_IN_OUT } dmaDir_t;

} // namespace TISP

#endif
