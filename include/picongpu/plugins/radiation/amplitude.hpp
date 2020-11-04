/* Copyright 2013-2020 Heiko Burau, Rene Widera, Richard Pausch, Alexander Debus
 *
 * This file is part of PIConGPU.
 *
 * PIConGPU is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PIConGPU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PIConGPU.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <pmacc/algorithms/math/defines/pi.hpp>
#include <pmacc/math/Complex.hpp>
#include "VectorTypes.hpp"
#include <pmacc/mpi/GetMPI_StructAsArray.hpp>


namespace picongpu
{
namespace plugins
{
namespace radiation
{

/** class to store 3 complex numbers for the radiated amplitude
 */
template <typename base_dtype = picongpu::float_64 >
class Amplitude
{
public:
  //using complex_64 = pmacc::math::Complex< picongpu::float_64 >;
  // For the intermediate amplitude values we may use single precision
  // for the final accumulation we will have to use double precision
  //using complex_32 = pmacc::math::Complex< picongpu::float_32 >;
  using complex_T = pmacc::math::Complex< base_dtype >;
  /* number of scalar components in Amplitude = 3 (3D) * 2 (complex) = 6 */
  // This definition appears stilted
  static constexpr uint32_t numComponents = uint32_t(3) * uint32_t(sizeof(complex_T) / sizeof(typename complex_T::type));

  /** constructor
   *
   * Arguments:
   * - vector_64: real 3D vector
   * - float: complex phase */
  DINLINE Amplitude(vector_32 vec, picongpu::float_X phase) // Needs change!
  {
      picongpu::float_X cosValue;
      picongpu::float_X sinValue;
      pmacc::math::sincos(phase, sinValue, cosValue);
      amp_x=pmacc::math::euler(vec.x(), precisionCast<base_dtype>(sinValue), precisionCast< base_dtype >(cosValue) );
      amp_y=pmacc::math::euler(vec.y(), precisionCast<base_dtype>(sinValue), precisionCast<base_dtype>(cosValue) );
      amp_z=pmacc::math::euler(vec.z(), precisionCast<base_dtype>(sinValue), precisionCast<base_dtype>(cosValue) );
  }


  /** default constructor
   *
   * \warning does not initialize values! */
  HDINLINE Amplitude(void)
  {

  }


  /** constructor
   *
   * Arguments:
   * - 6x float: Re(x), Im(x), Re(y), Im(y), Re(z), Im(z) */
  HDINLINE Amplitude(const base_dtype x_re, const base_dtype x_im,
                     const base_dtype y_re, const base_dtype y_im,
                     const base_dtype z_re, const base_dtype z_im)
      : amp_x(x_re, x_im), amp_y(y_re, y_im), amp_z(z_re, z_im)
  {

  }


  /** returns a zero amplitude vector
   *
   * used to initialize amplitudes to zero */
  HDINLINE static Amplitude zero(void)
  {
      Amplitude result;
      result.amp_x = complex_T::zero();
      result.amp_y = complex_T::zero();
      result.amp_z = complex_T::zero();
      return result;
  }

  /** assign addition */
  HDINLINE Amplitude& operator+=(const Amplitude& other)
  {
      amp_x += other.amp_x;
      amp_y += other.amp_y;
      amp_z += other.amp_z;
      return *this;
  }


  /** assign difference */
  HDINLINE Amplitude& operator-=(const Amplitude& other)
  {
      amp_x -= other.amp_x;
      amp_y -= other.amp_y;
      amp_z -= other.amp_z;
      return *this;
  }


  /** calculate radiation from *this amplitude
   *
   * Returns: \f$\frac{d^2 I}{d \Omega d \omega} = const*Amplitude^2\f$ */
  HDINLINE base_dtype calc_radiation(void)
  {
      // const SI factor radiation
      const base_dtype factor = 1.0 /
        (16. * util::cube(pmacc::math::Pi< base_dtype >::value) * picongpu::EPS0 * picongpu::SPEED_OF_LIGHT);

      return factor * (pmacc::math::abs2(amp_x) + pmacc::math::abs2(amp_y) + pmacc::math::abs2(amp_z));
  }


  /** debugging method
   *
   * Returns: real-x-value */
  HDINLINE base_dtype debug(void)
  {
      return amp_x.get_real();
  }


private:
  complex_32 amp_x; // complex amplitude x-component
  complex_32 amp_y; // complex amplitude y-component
  complex_32 amp_z; // complex amplitude z-component

};
} // namespace radiation
} // namespace plugins
} // namespace picongpu

namespace pmacc
{
namespace mpi
{

  /** implementation of MPI transaction on Amplitude class */
  template<>
  HINLINE MPI_StructAsArray getMPI_StructAsArray< picongpu::plugins::radiation::Amplitude >()
  {
      MPI_StructAsArray result = getMPI_StructAsArray< picongpu::plugins::radiation::Amplitude::complex_T::type > ();
      result.sizeMultiplier *= picongpu::plugins::radiation::Amplitude::numComponents;
      return result;
  };

} // namespace mpi
} // namespace pmacc
