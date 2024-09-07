/******************************************************************************/
/*                            This file is part of                            */
/*                       BEAVER, a MOOSE-based application                    */
/*       Multiphase Flow Poromechanics for Induced Seismicity Problems        */
/*                                                                            */
/*                  Copyright (C) 2022 by Antoine B. Jacquey                  */
/*                  Tufts University / Polytechnique Montreal                 */
/*                                                                            */
/*            Licensed under GNU Lesser General Public License v2.1           */
/*                       please see LICENSE for details                       */
/*                 or http://www.gnu.org/licenses/lgpl.html                   */
/******************************************************************************/

#pragma once

#include "BVTwoPointFluxApproximationBase.h"

class BVFVSinglePhaseSoluteDarcy : public BVTwoPointFluxApproximationBase
{
public:
  static InputParameters validParams();
  BVFVSinglePhaseSoluteDarcy(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  const MooseVariableFV<Real> * _p_var;
  const ADMaterialProperty<Real> & _lambda;
  const ADMaterialProperty<Real> & _lambda_neighbor;
  const ADMaterialProperty<Real> & _lambda_c;
  const ADMaterialProperty<Real> & _lambda_c_neighbor;
};