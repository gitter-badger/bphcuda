#pragma once

#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/transform.h>

#include <thrusting/dtype/real.h>
#include <thrusting/dtype/tuple/real.h>
#include <thrusting/iterator/zip_iterator.h>

#include <bphcuda/kinetic_e.h>

namespace {
  using thrusting::real;
  using thrusting::real3;
  using thrusting::real5;
}

namespace {

/*
  (c, m, s) -> in_e
*/
struct in_e_allocator :public thrust::unary_function<real5, real> {
  __host__ __device__
  real oparator()(const real5 &in) const {
    real3 c = thrusting::make_real3(in.get<0>(), in.get<1>(), in.get<2>());
    real m = in.get<3>();
    real s = in.get<4>();
    real ratio = s / real(3.0);
    return ratio * bphcuda::calc_kinetic_e(c, m);
  }
}; 

} // END namespace

namespace bphcuda {

/*
  Initialization scheme.
  Inner energies are generated by sharing the kinetic energy
  that is allocated by distribution function initially. 
*/
template<typename RealIterator>
void alloc_in_e(
  size_t n_particle, 
  RealIterator u, RealIterator v, RealIterator w,
  RealIterator m,
  RealIterator in_e, // output
  RealIterator s, 
){
  thrust::transform(
    thrusting::make_zip_iterator(u, v, w, m, s),
    thrusting::advance(n_particle, thrusting::make_zip_iterator(u, v, w, m, s)),
    in_e,
    in_e_allocator());  
}

} 
