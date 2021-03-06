#pragma once

#include <thrust/random.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/transform.h>

#include <thrusting/list.h>
#include <thrusting/real.h>
#include <thrusting/tuple.h>
#include <thrusting/functional.h>
#include <thrusting/algorithm/transform.h>
#include <thrusting/random/engine.h>
#include <thrusting/random/distribution.h>
#include <thrusting/math.h>

namespace {
  using namespace thrust;
  using namespace thrusting;
}

namespace bphcuda {

namespace detail {

/*
 * (rand, rand) -> c
 */
class shell_rand :public thrust::unary_function<real2, real3> {
  real _PI;
public:
  shell_rand(real PI)
  :_PI(PI){}
  __host__ __device__
  real3 operator()(const real2 &rand) const {
    real cs = real(1) - real(2) * get<0>(rand); // cs = [-1, 1)
    real sn = sqrtr(real(1) - cs*cs);
    real b = real(2) * _PI * get<1>(rand);

    real cx = sn * sinr(b);
    real cy = sn * cosr(b);
    real cz = cs;

    return real3(cx, cy, cz);
  }
};

} // END detail

/*
 * Hard coded which to use fast rand or accurate rand
 */
template<typename Real, typename Int, typename Predicate>
void alloc_shell_rand_if(
  size_t n_particle,
  Real u, Real v, Real w,
  Int stencil,
  Predicate pred,
  size_t seed,
  real PI = 3.14
){
  thrusting::transform_if(
    n_particle,
    thrust::make_transform_iterator(
      thrusting::make_zip_iterator(
        thrust::make_transform_iterator(
          thrust::counting_iterator<size_t>(0),
          thrusting::compose(
            thrusting::make_uniform_real_distribution<real>(0,1),
            thrusting::make_fast_rng_generator(seed))),
        thrust::make_transform_iterator(
          thrust::counting_iterator<size_t>(n_particle),
          thrusting::compose(
            thrusting::make_uniform_real_distribution<real>(0,1),
            thrusting::make_fast_rng_generator(seed)))),
      detail::shell_rand(PI)),
    stencil,
    thrusting::make_zip_iterator(u, v, w),
    thrust::identity<real3>(),
    pred);       
}

template<typename Real>
void alloc_shell_rand(
  size_t n_particle,
  Real u, Real v, Real w,
  size_t seed,
  real PI = 3.14
){
  alloc_shell_rand_if(
    n_particle,
    u, v, w,
    thrust::make_constant_iterator(true),
    thrust::identity<bool>(),
    seed);
}

} // END bphcuda
