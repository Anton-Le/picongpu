/**
 * Copyright 2014-2017 Marco Garten
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

#include "simulation_defines.hpp"
#include "particles/traits/GetIonizationEnergies.hpp"
#include "particles/traits/GetAtomicNumbers.hpp"
#include "traits/attribute/GetChargeState.hpp"

/** @file AlgorithmBSIHydrogenLike.hpp
 *
 * IONIZATION ALGORITHM for the BSI model
 *
 * - implements the calculation of ionization probability and changes charge states
 *   by decreasing the number of bound electrons
 * - is called with the IONIZATION MODEL, specifically by setting the flag in @see speciesDefinition.param
 */

namespace picongpu
{
namespace particles
{
namespace ionization
{

    /** Calculation for the Barrier Suppression Ionization model
     */
    struct AlgorithmBSIHydrogenLike
    {

        /** Functor implementation
         *
         * \tparam EType type of electric field
         * \tparam ParticleType type of particle to be ionized
         *
         * \param eField electric field value at t=0
         * \param parentIon particle instance to be ionized with position at t=0 and momentum at t=-1/2
         *
         * and "t" being with respect to the current time step (on step/half a step backward/-""-forward)
         *
         * \return the number of electrons to produce
         * (current implementation supports only 0 or 1 per execution)
         */
        template<typename EType, typename ParticleType >
        HDINLINE uint32_t
        operator()( const EType eField, ParticleType& parentIon )
        {

            const float_X protonNumber = GetAtomicNumbers<ParticleType>::type::numberOfProtons;
            float_X chargeState = attribute::getChargeState(parentIon);

            /* verify that ion is not completely ionized */
            if (chargeState < protonNumber)
            {
                uint32_t cs = math::float2int_rd(chargeState);
                /* ionization potential in atomic units */
                const float_X iEnergy = GetIonizationEnergies<ParticleType>::type()[cs];
                /* critical field strength in atomic units */
                float_X critField = iEnergy*iEnergy / (float_X(4.0) * protonNumber);
                /* ionization condition */
                if (math::abs(eField) / ATOMIC_UNIT_EFIELD >= critField)
                {
                    /* return number of electrons to produce */
                    return 1;
                }
            }
            /* no ionization */
            return 0;
        }
    };

} // namespace ionization
} // namespace particles
} // namespace picongpu
