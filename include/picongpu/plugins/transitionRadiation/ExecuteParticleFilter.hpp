/* Copyright 2017-2021 Rene Widera, Finn-Ole Carstens
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

#include "picongpu/simulation_defines.hpp"

#include "picongpu/particles/Manipulate.hpp"
#include "picongpu/plugins/transitionRadiation/GammaMask.hpp"

#include <pmacc/traits/HasIdentifier.hpp>

#include <memory>


namespace picongpu
{
    namespace plugins
    {
        namespace transitionRadiation
        {
            /** read the `transitionRadiationMask` of a species */
            template<bool hasFilter>
            struct ExecuteParticleFilter
            {
                /** get the attribute value of `transitionRadiationMask`
                 *
                 * @param species buffer
                 * @param currentStep current simulation time step
                 * @return value of the attribute `transitionRadiationMask`
                 */
                template<typename T_Species>
                void operator()(std::shared_ptr<T_Species> const&, const uint32_t currentStep)
                {
                    particles::Manipulate<picongpu::plugins::transitionRadiation::GammaFilter, T_Species>{}(
                        currentStep);
                }
            };

            /** specialization
             *
             * specialization for the case that the species does not have the attribute
             * `transitionRadiationMask`
             */
            template<>
            struct ExecuteParticleFilter<false>
            {
                /** get the attribute value of `transitionRadiationMask`
                 *
                 * @param particle to be used
                 * @return always true
                 */
                template<typename T_Species>
                void operator()(const std::shared_ptr<T_Species>, const uint32_t currentStep)
                {
                }
            };

            /** execute the particle filter on a species
             *
             * It is **allowed** to call this function even if the species does not contain
             * the attribute `transitionRadiationMask`.
             * The filter is **not** executed if the species does not contain the attribute `transitionRadiationMask`.
             *
             * @tparam T_Species species type
             * @param species species to be filtered
             */
            template<typename T_Species>
            void executeParticleFilter(std::shared_ptr<T_Species>& species, const uint32_t currentStep)
            {
                constexpr bool hasRadiationFilter = pmacc::traits::
                    HasIdentifier<typename T_Species::FrameType, transitionRadiationMask>::type::value;

                return ExecuteParticleFilter<hasRadiationFilter>{}(species, currentStep);
            }

        } // namespace transitionRadiation
    } // namespace plugins
} // namespace picongpu
