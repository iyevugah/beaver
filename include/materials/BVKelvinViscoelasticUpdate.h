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

#include "BVCreepUpdateBase.h"

class BVKelvinViscoelasticUpdate : public BVCreepUpdateBase
{
public:
  static InputParameters validParams();
  BVKelvinViscoelasticUpdate(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual ADReal creepRate(const ADReal & eqv_strain_incr) override;
  virtual ADReal creepRateDerivative(const ADReal & eqv_strain_incr) override;
  virtual ADReal kelvinCreepStrain(const ADReal & eqv_strain_incr);
  virtual ADReal kelvinCreepStrainDerivative(const ADReal & /*eqv_strain_incr*/);
  virtual void preReturnMap() override;
  virtual void postReturnMap(const ADReal & eqv_strain_incr) override;

  // Reference viscosity
  const Real _eta0;

  // Reference shear modulus
  const Real _G0;

  // Internal variable for creep strain
  ADMaterialProperty<Real> & _eqv_creep_strain;
  const MaterialProperty<Real> & _eqv_creep_strain_old;
};
