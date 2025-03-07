/******************************************************************************/
/*                            This file is part of                            */
/*                       BEAVER, a MOOSE-based application                    */
/*       Multiphase Flow Poromechanics for Induced Seismicity Problems        */
/*                                                                            */
/*                  Copyright (C) 2024 by Antoine B. Jacquey                  */
/*                           Polytechnique Montréal                           */
/*                                                                            */
/*            Licensed under GNU Lesser General Public License v2.1           */
/*                       please see LICENSE for details                       */
/*                 or http://www.gnu.org/licenses/lgpl.html                   */
/******************************************************************************/

#pragma once

#include "BVDeviatoricVolumetricUpdateBase.h"

class BVRTL2020ModelUpdate : public BVDeviatoricVolumetricUpdateBase
{
public:
  static InputParameters validParams();
  BVRTL2020ModelUpdate(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual ADReal creepRate(const std::vector<ADReal> & creep_strain_incr,
                           const unsigned int i) override;
  virtual ADReal creepRateR(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal creepRateLemaitre(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal creepRateMunsonDawson(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal creepRateDerivative(const std::vector<ADReal> & creep_strain_incr,
                                     const unsigned int i,
                                     const unsigned int j) override;
  virtual ADReal creepRateRDerivative(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal creepRateLemaitreDerivative(const std::vector<ADReal> & creep_strain_incr,
                                             const unsigned int /*j*/);
  virtual ADReal creepRateMunsonDawsonDerivative(const std::vector<ADReal> & creep_strain_incr,
                                                 const unsigned int j);
  virtual ADReal lemaitreCreepStrain(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal munsondawsonCreepStrain(const std::vector<ADReal> & creep_strain_incr);
  virtual ADReal volumetricCreepStrain(const std::vector<ADReal> & v_creep_strain_incr);
  virtual void preReturnMap() override;
  virtual void postReturnMap(const std::vector<ADReal> & creep_strain_incr) override;
  virtual void preReturnMapVol(const std::vector<ADReal> & creep_strain_incr) override;
  virtual ADReal creepRateVol(const ADReal & vol_strain_incr) override;
  virtual ADReal creepRateVolDerivative(const ADReal & vol_strain_incr) override;

  // Temperature coupling
  const ADVariableValue * const _temp;
  const Real _temp_ref;
  const Real _Ar;
  ADReal _exponential;

  // Lemaitre creep strain rate parameters
  const Real _alpha;
  const Real _A2;
  const Real _n2;

  // Munson-Dawson creep strain rate parameters
  const Real _A1;
  const Real _n1;
  const Real _A;
  const Real _B;
  const Real _m;
  const Real _n;

  // Volumetric creep strain rate parameters
  const Real _z;
  const Real _Nz;
  const Real _nz;
  const Real _Mz;
  const Real _mz;

  // Internal variable for Lemaitre and Munson-Dawson creep strain
  ADMaterialProperty<Real> & _eqv_creep_strain_L;
  const MaterialProperty<Real> & _eqv_creep_strain_L_old;
  ADMaterialProperty<Real> & _eqv_creep_strain_R;
  const MaterialProperty<Real> & _eqv_creep_strain_R_old;

  // Internal variable for volumetric creep strain
  ADMaterialProperty<Real> & _vol_creep_strain;
  const MaterialProperty<Real> & _vol_creep_strain_old;

  // Scalar equivalent creep strain
  ADReal _gamma_vp;
  ADReal _gamma_dot_vp;
};
