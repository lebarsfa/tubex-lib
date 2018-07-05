/* ============================================================================
 *  tubex-lib - Function class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2018
 * ---------------------------------------------------------------------------- */

#include "tubex_Function.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  Function::Function(const char* y)
    : tubex::Fnc(0, 1)
  {
    m_ibex_f = new ibex::Function("t", y);
  }

  Function::Function(const char* x1, const char* y)
    : tubex::Fnc(1, 1)
  {
    m_ibex_f = new ibex::Function("t", x1, y);
    // todo: check x1!="t"
  }

  Function::Function(const tubex::Function& f)
    : tubex::Fnc(f.nbVars(), f.imageDim())
  {
    *this = f;
  }

  Function::~Function()
  {
    delete m_ibex_f;
  }

  const Function& Function::operator=(const Function& f)
  {
    if(m_ibex_f != NULL)
      delete m_ibex_f;
    m_ibex_f = new ibex::Function(*f.m_ibex_f);
    Fnc::operator=(f);
  }

  const IntervalVector Function::eval(double t, const IntervalVector& x) const
  {
    return eval(Interval(t), x);
  }

  const IntervalVector Function::eval(const Interval& t, const IntervalVector& x) const
  {
    // todo: check dim x regarding f
    if(x.is_empty())
      return IntervalVector(imageDim(), Interval::EMPTY_SET);

    IntervalVector box(nbVars() + 1); // +1 for system variable (t)
    box[0] = t;
    if(nbVars() != 0)
      box.put(1, x);
    return m_ibex_f->eval_vector(box);
  }

  const TubeVector Function::eval(const TubeVector& x) const
  {
    // todo: check dim x regarding f. f.imgdim can be of 0 and then x 1 in order to keep slicing pattern
    IntervalVector box(nbVars() + 1); // +1 for system variable (t)
    TubeVector y(x, IntervalVector(imageDim()));

    const TubeSlice *x_slice = x.getFirstSlice();
    TubeSlice *y_slice = y.getFirstSlice();

    while(x_slice != NULL)
    {
      y_slice->setInputGate(eval(x_slice->domain().lb(), x_slice->inputGate()));
      y_slice->setEnvelope(eval(x_slice->domain(), x_slice->codomain()));

      x_slice = x_slice->nextSlice();
      y_slice = y_slice->nextSlice();
    }

    x_slice = x.getLastSlice();
    y_slice = y.getLastSlice();
    y_slice->setOutputGate(eval(x_slice->domain().ub(), x_slice->outputGate()));

    return y;
  }
}