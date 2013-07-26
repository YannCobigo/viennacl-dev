#ifndef VIENNACL_GENERATOR_UTILS_HPP
#define VIENNACL_GENERATOR_UTILS_HPP

/* =========================================================================
   Copyright (c) 2010-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */


/** @file viennacl/generator/utils.hpp
    @brief Internal utils
*/

#include <sstream>

#include "viennacl/traits/size.hpp"

#include "viennacl/scheduler/forwards.h"

namespace viennacl{

  namespace generator{

    namespace utils{

      template<class Fun>
      typename Fun::result_type call_on_host_scalar(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::HOST_SCALAR_FLOAT_TYPE :
            return fun(element.host_float_);
          case scheduler::HOST_SCALAR_DOUBLE_TYPE :
            return fun(element.host_double_);
          default :
            throw "not implemented";
        }
      }

      template<class Fun>
      typename Fun::result_type call_on_scalar(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::SCALAR_FLOAT_TYPE :
            return fun(*element.scalar_float_);
          case scheduler::SCALAR_DOUBLE_TYPE :
            return fun(*element.scalar_double_);
          default :
            throw "not implemented";
        }
      }

      template<class Fun>
      typename Fun::result_type call_on_vector(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::VECTOR_FLOAT_TYPE :
            return fun(*element.vector_float_);
          case scheduler::VECTOR_DOUBLE_TYPE :
            return fun(*element.vector_double_);
          default :
            throw "not implemented";
        }
      }

      template<class Fun>
      typename Fun::result_type call_on_symbolic_vector(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::SYMBOLIC_VECTOR_FLOAT_TYPE :
            return fun(*element.symbolic_vector_float_);
          case scheduler::SYMBOLIC_VECTOR_DOUBLE_TYPE :
            return fun(*element.symbolic_vector_double_);
          default :
            throw "not implemented";
        }
      }

      template<class Fun>
      typename Fun::result_type call_on_matrix(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::MATRIX_ROW_FLOAT_TYPE :
            return fun(*element.matrix_row_float_);
          case scheduler::MATRIX_ROW_DOUBLE_TYPE :
            return fun(*element.matrix_row_double_);

          case scheduler::MATRIX_COL_FLOAT_TYPE :
            return fun(*element.matrix_col_float_);
          case scheduler::MATRIX_COL_DOUBLE_TYPE :
            return fun(*element.matrix_col_double_);
          default :
            throw "not implemented";
        }
      }


      template<class Fun>
      typename Fun::result_type call_on_symbolic_matrix(scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(type){
          case scheduler::SYMBOLIC_MATRIX_FLOAT_TYPE :
            return fun(*element.symbolic_matrix_float_);
          case scheduler::SYMBOLIC_MATRIX_DOUBLE_TYPE :
            return fun(*element.symbolic_matrix_double_);
          default :
            throw "not implemented";
        }
      }

      template<class Fun>
      typename Fun::result_type call_on_element(scheduler::statement_node_type_family family, scheduler::statement_node_type type, scheduler::lhs_rhs_element element, Fun const & fun){
        switch(family){
          case scheduler::HOST_SCALAR_TYPE_FAMILY:
            return call_on_host_scalar(type, element, fun);
          case scheduler::SCALAR_TYPE_FAMILY:
            return call_on_scalar(type, element, fun);
          case scheduler::VECTOR_TYPE_FAMILY :
            return call_on_vector(type, element, fun);
          case scheduler::SYMBOLIC_VECTOR_TYPE_FAMILY :
            return call_on_symbolic_vector(type, element, fun);
          case scheduler::MATRIX_ROW_TYPE_FAMILY:
            return call_on_matrix(type,element,fun);
          case scheduler::MATRIX_COL_TYPE_FAMILY:
            return call_on_matrix(type,element,fun);
          case scheduler::SYMBOLIC_MATRIX_TYPE_FAMILY :
            return call_on_symbolic_matrix(type, element, fun);
          default:
            throw "not implemented";
        }
      }

      struct size_fun{
          typedef std::size_t result_type;
          template<class T>
          result_type operator()(T const &t) const { return viennacl::traits::size(t); }
      };

      struct size1_fun{
          typedef std::size_t result_type;
          template<class T>
          result_type operator()(T const &t) const { return viennacl::traits::size1(t); }
      };

      struct size2_fun{
          typedef std::size_t result_type;
          template<class T>
          result_type operator()(T const &t) const { return viennacl::traits::size2(t); }
      };

      std::size_t size(scheduler::statement_node_type type, scheduler::lhs_rhs_element element){
        return call_on_vector(type, element, size_fun());
      }
      
      std::size_t size1(scheduler::statement_node_type type, scheduler::lhs_rhs_element element){
        return call_on_matrix(type, element, size1_fun());
      }

      std::size_t size2(scheduler::statement_node_type type, scheduler::lhs_rhs_element element){
        return call_on_matrix(type, element, size2_fun());
      }

      template<class T, class U>
      struct is_same_type { enum { value = 0 }; };

      template<class T>
      struct is_same_type<T,T> { enum { value = 1 }; };

      template <class T>
      inline std::string to_string ( T const t )
      {
        std::stringstream ss;
        ss << t;
        return ss.str();
      }

      template<class T>
      struct type_to_string;
      template<> struct type_to_string<float> { static const char * value() { return "float"; } };
      template<> struct type_to_string<double> { static const char * value() { return "double"; } };


      template<class T>
      struct first_letter_of_type;
      template<> struct first_letter_of_type<float> { static char value() { return 'f'; } };
      template<> struct first_letter_of_type<double> { static char value() { return 'd'; } };
      template<> struct first_letter_of_type<viennacl::row_major> { static char value() { return 'r'; } };
      template<> struct first_letter_of_type<viennacl::column_major> { static char value() { return 'c'; } };

      class kernel_generation_stream : public std::ostream{
        private:

          class kgenstream : public std::stringbuf{
            public:
              kgenstream(std::ostringstream& oss,unsigned int const & tab_count) : oss_(oss), tab_count_(tab_count){ }
              int sync() {
                for(unsigned int i=0 ; i<tab_count_;++i)
                  oss_ << "    ";
                oss_ << str();
                str("");
                return !oss_;
              }
              ~kgenstream() {  pubsync(); }
            private:
              std::ostream& oss_;
              unsigned int const & tab_count_;
          };

        public:
          kernel_generation_stream() : std::ostream(new kgenstream(oss,tab_count_)), tab_count_(0){ }

          std::string str(){ return oss.str(); }

          void inc_tab(){ ++tab_count_; }

          void dec_tab(){ --tab_count_; }

          ~kernel_generation_stream(){ delete rdbuf(); }

        private:
          unsigned int tab_count_;
          std::ostringstream oss;
      };


    }

  }

}
#endif
