// Copyright (C) 2009 by Thomas Moulard, AIST, CNRS, INRIA.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ROBOPTIM_TRAJECTORY_LIMIT_SPEED_HXX
# define ROBOPTIM_TRAJECTORY_LIMIT_SPEED_HXX
# include <boost/numeric/ublas/vector.hpp>
# include <boost/scoped_ptr.hpp>

# include <roboptim/core/finite-difference-gradient.hh>

# include <roboptim/trajectory/trajectory.hh>

namespace roboptim
{
  template <typename T>
  LimitSpeed<T>::LimitSpeed (StableTimePoint timePoint,
			     const T& trajectory) throw ()
    : DerivableFunction (trajectory.parameters ().size (), 1, "limit speed"),
      timePoint_ (timePoint),
      trajectory_ (trajectory)
  {}

  template <typename T>
  LimitSpeed<T>::~LimitSpeed () throw ()
  {}
  
  template <typename T>
  void
  LimitSpeed<T>::impl_compute (result_t& res, const argument_t& p) const throw ()
  {
    using namespace boost::numeric::ublas;
    res.clear ();

    boost::scoped_ptr<T> updatedTrajectory (trajectory_.clone ());
    updatedTrajectory->setParameters (p);
    res[0] = norm_2 (updatedTrajectory->derivative (timePoint_));
  }

  template <typename T>
  void
  LimitSpeed<T>::impl_gradient (gradient_t& grad, const argument_t& p, size_type i)
    const throw ()
  {
    assert (i == 0);
    grad.clear ();

    //FIXME: compute gradient analytically.
    FiniteDifferenceGradient fdfunction (*this);
    fdfunction.gradient (grad, p, 0);
  }

  template <typename T>
  template <typename F, typename CLIST>
  void
  LimitSpeed<T>::addToProblem (const T& trajectory,
			       Problem<F, CLIST>& problem,
			       typename Function::interval_t vRange,
			       unsigned nConstraints)
  {
    using namespace boost;
    if (nConstraints == 0)
      return;
    for (double i = 0; i < 1.; i += 1. / nConstraints)
      {
	shared_ptr<LimitSpeed> speed (new LimitSpeed (i * tMax, trajectory));
	problem.addConstraint
	  (static_pointer_cast<DerivableFunction> (speed),
	   vRange);
      }
  }
} // end of namespace roboptim.


#endif //! ROBOPTIM_TRAJECTORY_LIMIT_SPEED_HXX
