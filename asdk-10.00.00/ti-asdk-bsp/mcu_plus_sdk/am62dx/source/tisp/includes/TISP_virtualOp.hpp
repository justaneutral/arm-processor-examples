#ifndef __VIRTUALOP_H
#define __VIRTUALOP_H

#include <vector>

namespace TISP {
class virtualOp {
 public:
   // virtual void init () = 0;
   virtual void exec() = 0;
   virtual ~virtualOp(){};
};

typedef std::vector<virtualOp *> opVec;
/* typedef std::list<virtualOp *>   opList; */
} // namespace TISP

#endif
