/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_ABSTRACT_MULTI_ARRAY_H__
#define CPPMH_MODEL_ABSTRACT_MULTI_ARRAY_H__

#include <vector>
#include <numeric>

namespace cppmh {
namespace model {
/**
 * This class was designed with reference to the following site.
 * https://stackoverflow.com/questions/47664127/create-a-multidimensional-array-dynamically-in-c/47664858
 */
class AbstractMultiArray {
   protected:
    int              m_id;
    std::size_t      m_number_of_dimensions;
    std::size_t      m_number_of_elements;
    int              m_max_digits;
    std::vector<int> m_shape;
    std::vector<int> m_strides;

    /*************************************************************************/
    void compute_strides(void) {
        m_strides.resize(m_number_of_dimensions);
        m_strides.at(m_number_of_dimensions - 1) = 1;
        std::partial_sum(m_shape.rbegin(), m_shape.rend() - 1,
                         m_strides.rbegin() + 1, std::multiplies<int>());
    }

    /*************************************************************************/
    AbstractMultiArray(void) {
        /// nothing to do
    }

   public:
    /*************************************************************************/
    AbstractMultiArray(const int a_ID) {
        m_id                   = a_ID;
        m_number_of_elements   = 1;
        m_number_of_dimensions = 1;
        m_shape                = {1};
        m_max_digits           = 1;

        this->compute_strides();
    }

    /*************************************************************************/
    AbstractMultiArray(const int a_ID, const int a_NUMBER_OF_ELEMENTS) {
        m_id                   = a_ID;
        m_number_of_elements   = a_NUMBER_OF_ELEMENTS;
        m_number_of_dimensions = 1;
        m_shape                = {a_NUMBER_OF_ELEMENTS};
        m_max_digits           = std::to_string(a_NUMBER_OF_ELEMENTS).size();

        this->compute_strides();
    }
    /*************************************************************************/
    AbstractMultiArray(const int a_ID, const std::vector<int> &a_SHAPE) {
        m_id                   = a_ID;
        m_number_of_elements   = std::accumulate(a_SHAPE.begin(), a_SHAPE.end(),
                                               1, std::multiplies<int>());
        m_number_of_dimensions = (a_SHAPE.size());
        m_shape                = a_SHAPE;
        m_max_digits           = std::to_string(utility::max(a_SHAPE)).size();
        this->compute_strides();
    }

    /*************************************************************************/
    virtual ~AbstractMultiArray(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline int flat_index(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        /// cannot be constexpr
        return std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                  a_MULTI_DIMENSIONAL_INDEX.end(),
                                  m_strides.begin(), 0);
    }

    /*************************************************************************/
    inline std::vector<int> multi_dimensional_index(
        const int a_FLAT_INDEX) const {
        /// cannot be constexpr
        std::vector<int> result(m_number_of_dimensions);

        int remain = a_FLAT_INDEX;
        for (std::size_t i = 0; i < m_number_of_dimensions; i++) {
            result[i] = remain / m_strides[i];
            remain    = remain % m_strides[i];
        }
        return result;
    }

    /*************************************************************************/
    inline void update_multi_dimensional_index(
        std::vector<int> *a_multi_dimensional_index,
        const int         a_FLAT_INDEX) const {
        /**
         * This method is used to reduce the number of generating
         * std::vector<int>.
         */
        int remain = a_FLAT_INDEX;
        for (std::size_t i = 0; i < m_number_of_dimensions; i++) {
            (*a_multi_dimensional_index)[i] = remain / m_strides[i];
            remain                          = remain % m_strides[i];
        }
    }

    /*************************************************************************/
    inline std::string indices_label(const int a_FLAT_INDEX) const {
        if (m_number_of_elements == 1) {
            return "";
        }

        std::vector<int> multi_dimensional_index =
            this->multi_dimensional_index(a_FLAT_INDEX);

        std::string label  = "";
        std::string format = "%" + std::to_string(m_max_digits) + "d";

        for (std::size_t i = 0; i < m_number_of_dimensions; i++) {
            label += utility::to_string(multi_dimensional_index[i], format);
            if (i != m_number_of_dimensions - 1) {
                label += ", ";
            }
        }
        return "[" + label + "]";
    }

    /*************************************************************************/
    inline constexpr int id(void) const {
        return m_id;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &shape(void) const {
        return m_shape;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &strides(void) const {
        return m_strides;
    }

    /*************************************************************************/
    inline constexpr std::size_t number_of_dimensions(void) const {
        return m_number_of_dimensions;
    }

    /*************************************************************************/
    inline constexpr std::size_t number_of_elements(void) const {
        return m_number_of_elements;
    }
};
}  // namespace model
}  // namespace cppmh
#endif

/*****************************************************************************/
// END
/*****************************************************************************/
