/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_CONSTRAINT_REFERENCE_H__
#define CPPMH_MODEL_CONSTRAINT_REFERENCE_H__

#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintReference {
    std::vector<Constraint<T_Variable, T_Expression> *> constraint_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *>
        selection_constraint_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *>
        disabled_constraint_ptrs;

    /*************************************************************************/
    ConstraintReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~ConstraintReference(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->constraint_ptrs.clear();
        this->selection_constraint_ptrs.clear();
        this->disabled_constraint_ptrs.clear();
    }
};
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/