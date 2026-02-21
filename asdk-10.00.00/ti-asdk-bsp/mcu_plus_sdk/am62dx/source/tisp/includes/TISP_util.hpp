#ifndef TISP_UTILS_HPP
#define TISP_UTILS_HPP

#include "TISP_superNode.hpp"
#include "TISP_virtualOp.hpp"
#include <TISP_types.hpp>
#include <list>
#include <stdlib.h>
#include <vector>

namespace TISP {

/**
   \class execute
   @brief Provides the execute function of the graph
 */

class execute {
 public:
   /**
    *  @ingroup  NODE
    */

   /**@{*/

   /**
    *  @brief This function takes in alias of the opeVec (list of nodes) and iterates through all the nodes calling the
    *         exec function pertaining to that node
    *  @param [in] &myOpVec : Object to list of nodes
    *  @return None.
    *
    */

   static void graph(const opVec &myOpVec)
   {

      for (auto const &op : myOpVec) {
         op->exec();
      }

      return;
   }

   /** @} */

   /**
    *  @ingroup  SUPERNODE
    */

   /**@{*/

   /**
    *  @brief This function takes in alias of the superNodeVec (list of supernodes) and iterates through all the
    *         supernodes calling the exec function pertaining to that supernode
    *  @param [in] &mySuperNodeVec : Object to list of supernodes
    *  @return None.
    *
    */

   static void graph(const SuperNode::superNodeVec &mySuperNodeVec)

   {

      for (auto const &op : mySuperNodeVec) {
         op->exec();
      }

      return;
   }

   /** @} */
};
} // namespace TISP

#endif
