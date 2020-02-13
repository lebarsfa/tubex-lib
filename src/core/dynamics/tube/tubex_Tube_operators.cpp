/** 
 *  Tube class (operators)
 * ----------------------------------------------------------------------------
 *  \date       2015
 *  \author     Simon Rohou
 *  \copyright  Copyright 2020 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#include "tubex_Tube.h"
#include "tubex_Trajectory.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  #define macro_assign_scal(f) \
    \
    const Tube& Tube::f(const Interval& x) \
    { \
      Slice *s = NULL; \
      do \
      { \
        if(s == NULL) /* first iteration */ \
          s = first_slice(); \
        else \
          s = s->next_slice(); \
        \
        s->set_envelope(Interval(s->codomain()).f(x), false); \
        s->set_input_gate(Interval(s->input_gate()).f(x), false); \
        \
      } while(s->next_slice() != NULL); \
      \
      s->set_output_gate(Interval(s->output_gate()).f(x), false); \
      return *this; \
    } \
    \
    const Tube& Tube::f(const Trajectory& x) \
    { \
      assert(domain() == x.domain()); \
      \
      Slice *s = NULL; \
      do \
      { \
        if(s == NULL) /* first iteration */ \
          s = first_slice(); \
        else \
          s = s->next_slice(); \
        \
        s->set_envelope(Interval(s->codomain()).f(x(s->domain())), false); \
        s->set_input_gate(Interval(s->input_gate()).f(x(Interval(s->domain().lb()))), false); \
         \
      } while(s->next_slice() != NULL); \
      \
      s->set_output_gate(Interval(s->output_gate()).f(x(Interval(s->domain().ub()))), false); \
      return *this; \
    } \
    \
    const Tube& Tube::f(const Tube& x) \
    { \
      assert(domain() == x.domain()); \
      \
      if(Tube::same_slicing(*this, x)) /* faster */ \
      { \
        Slice *s = NULL; \
        const Slice *s_x = NULL; \
        do \
        { \
          if(s == NULL) /* first iteration */ \
          { \
            s = first_slice(); \
            s_x = x.first_slice(); \
          } \
          \
          else \
          {  \
            s = s->next_slice(); \
            s_x = s_x->next_slice(); \
          } \
          \
          s->set_envelope(Interval(s->codomain()).f(s_x->codomain()), false); \
          s->set_input_gate(Interval(s->input_gate()).f(s_x->input_gate()), false); \
        } while(s->next_slice() != NULL); \
        \
        s->set_output_gate(Interval(s->output_gate()).f((s_x->output_gate())), false); \
      } \
      \
      else \
      { \
        Slice *s = NULL; \
        do \
        { \
          if(s == NULL) /* first iteration */ \
            s = first_slice(); \
          else \
            s = s->next_slice(); \
          \
          s->set_envelope(Interval(s->codomain()).f(x(s->domain())), false); \
          s->set_input_gate(Interval(s->input_gate()).f(x(s->domain().lb())), false); \
          \
        } while(s->next_slice() != NULL); \
        \
        s->set_output_gate(Interval(s->output_gate()).f(x(s->domain().ub())), false); \
      } \
      \
      return *this; \
    } \
    \

  macro_assign_scal(operator+=);
  macro_assign_scal(operator-=);
  macro_assign_scal(operator*=);
  macro_assign_scal(operator/=);
  macro_assign_scal(operator&=);
  macro_assign_scal(operator|=);
}