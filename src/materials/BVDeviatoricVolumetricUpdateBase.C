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

#include "BVDeviatoricVolumetricUpdateBase.h"
#include "BVElasticityTensorTools.h"

InputParameters
BVDeviatoricVolumetricUpdateBase::validParams()
{
  InputParameters params = BVTwoCreepUpdateBase::validParams();
  params.addClassDescription("Base material for computing multiple creep stress" 
  "updates with Two flow directions (or rules). This class inherits from BVTwoCreepUpdateBase.");
  params.addParam<std::string>(
      "base_name",
      "Optional parameter that defines a prefix for all material "
      "properties related to this stress update model. This allows for "
      "multiple models of the same type to be used without naming conflicts.");
  params.addRequiredParam<Real>("num_cm", "number of creep models");
  params.addRequiredParam<bool>("dev_vol", "Whether to perform deviatoric and volumetric calculations or only deviatoric calculations");
  return params;
}

BVDeviatoricVolumetricUpdateBase::BVDeviatoricVolumetricUpdateBase(const InputParameters & parameters)
  : BVTwoCreepUpdateBase(parameters),
    _identity_two(RankTwoTensor::initIdentity),
    _num_cm(getParam<Real>("num_cm")),
    _dev_vol(parameters.get<bool>("dev_vol"))
{
}

void
BVDeviatoricVolumetricUpdateBase::inelasticUpdate(ADRankTwoTensor & stress, const RankFourTensor & Cijkl)
{
  // Here we do an iterative update with a single variable being the scalar effective stress
  // We are trying to find the zero of the function F which is defined as:
  // $F\left(\sigma_{e}\right) = \sigma_{e}^{tr} - \sigme_{e} - 3 * \eta * \dot{gamma}$
  // $\dot{\gamma}$: scalar creep strain rate
  // $\sigma_{e}$: scalar effective stress
  // $\eta$: the viscosity
  // flow rule: $\dot{\gamma} = \frac{\sigma_{e}}{3 \eta}$

  // Trial stress
  _stress_tr = stress;
  // Trial effective stress
  _eqv_stress_tr = std::sqrt(1.5) * _stress_tr.deviatoric().L2norm();
  _avg_stress_tr = -_stress_tr.trace() / 3.0;
  // Shear and bulk modulus
  _G = BVElasticityTensorTools::getIsotropicShearModulus(Cijkl);
  _K = BVElasticityTensorTools::getIsotropicBulkModulus(Cijkl);

  // Initialize creep strain increment
  _creep_strain_incr[_qp].zero();

  // Pre return map calculations (model specific)
  preReturnMap();

  // initialize the equivalent creep strains
  std::vector<ADReal> eqv_strain_incr(_num_cm, 0.0);
  ADReal Vol_eqv_strain_incr = 0.0;
  // Viscoelastic update
  dvreturnMap(eqv_strain_incr, Vol_eqv_strain_incr);

  _creep_strain_incr[_qp] = dvreformPlasticStrainTensor(eqv_strain_incr, Vol_eqv_strain_incr);
  stress -= 2.0 * _G * _creep_strain_incr[_qp];
  postReturnMap(eqv_strain_incr);
}


//Deviatoric-Volumetric updates
void
BVDeviatoricVolumetricUpdateBase::dvreturnMap(std::vector<ADReal>& creep_strain_incr_out, 
                                              ADReal & Vcreep_strain_incr_out)
{
    if (_dev_vol)
    {
        // Initialize scalar deviatoric creep strain increment
        std::vector<ADReal> creep_strain_incr(_num_cm, 0.0);

        // Initial residual
        std::vector<ADReal> res_ini = residual(creep_strain_incr); // initialize residual
        std::vector<ADReal> res = res_ini;

        std::vector<std::vector<ADReal>> jac = jacobian(creep_strain_incr); // initialize Jacobian
        // Initialize scalar volumetric creep strain increment
        ADReal Vcreep_strain_incr = 0.0; 

        // Initial volumetric residual
        ADReal Vres_ini = vresidual(creep_strain_incr, Vcreep_strain_incr); // initialize volumetric residual
        ADReal Vres = Vres_ini;

        // Initial volumetric jacobian
        ADReal Vjac = vjacobian(creep_strain_incr,Vcreep_strain_incr); // initialize volumetric Jacobian

        // Newton loop
        for (unsigned int iter = 0; iter < _max_its; ++iter)
        {
            // Call to nrStep to update the scalar deviatoric strain increment
            nrStep(res, jac, creep_strain_incr);

            // Recalculate residuals and Jacobians for the deviatoric part
            res = residual(creep_strain_incr);
            jac = jacobian(creep_strain_incr);

            // compute and update the scalar volumetric strain increment
            Vcreep_strain_incr -= Vres / Vjac;  
            Vres = vresidual(creep_strain_incr, Vcreep_strain_incr);
            Vjac = vjacobian(creep_strain_incr, Vcreep_strain_incr);
            
            // Convergence check
            bool D_converge = (norm(res) <= _abs_tol) || (norm(res) / norm(res_ini) <= _rel_tol);
            bool V_converge = (abs(Vres) <= _abs_tol) || (abs(Vres) / Vres_ini <= _rel_tol);
            bool converge = D_converge && V_converge;

            // Check if convergence was achieved
            if (converge) 
                {
                    // Assign results to output parameters
                    creep_strain_incr_out = creep_strain_incr;
                    Vcreep_strain_incr_out = Vcreep_strain_incr; // Ensure Vcreep_strain_incr is returned
                  return;  // Exit after successfully converging
                }
        }
        // If maximum iterations are exceeded without convergence, throw an exception
        throw MooseException("BVDeviatoricVolumetricUpdateBase: maximum number of iterations exceeded in the deviatoric-volumetric 'returnMap'!");
    }
    else // Implement only the deviatoric part for now
    {  
        // Call the returnMap function if only deviatoric updates are required
        creep_strain_incr_out = returnMap();
        Vcreep_strain_incr_out = 0.0; // Since volumetric update is not required in this case, set it to a default value
    }
}

ADReal
BVDeviatoricVolumetricUpdateBase::vresidual(const std::vector<ADReal> & creep_strain_incr, const ADReal & Vcreep_strain_incr)
{
   return vcreepRate(creep_strain_incr, Vcreep_strain_incr) * _dt - Vcreep_strain_incr;
}

ADReal
BVDeviatoricVolumetricUpdateBase::vjacobian(const std::vector<ADReal> & creep_strain_incr, const ADReal & Vcreep_strain_incr)
{
   return  vcreepRateDerivative(creep_strain_incr,Vcreep_strain_incr) * _dt - 1.0;
}

ADRankTwoTensor
BVDeviatoricVolumetricUpdateBase::dvreformPlasticStrainTensor(const std::vector<ADReal> & creep_strain_incr, const ADReal & Vcreep_strain_incr)
{
    if (_dev_vol)
    {
      ADRankTwoTensor res;
      ADRankTwoTensor Dres;
      ADRankTwoTensor Vres;
      res.zero();
      Dres.zero();
      Vres.zero();

      ADRankTwoTensor flow_dir1 =
          (_eqv_stress_tr != 0.0) ? _stress_tr.deviatoric() / _eqv_stress_tr : ADRankTwoTensor();

      ADRankTwoTensor flow_dir2 =  _identity_two;
      
      for (unsigned int i = 0; i < creep_strain_incr.size(); ++i)
        Dres += 1.5 * creep_strain_incr[i] * flow_dir1;

      Vres = ((1.0 / std::sqrt(3)) * Vcreep_strain_incr * _dt * flow_dir2);

      res = Dres + Vres;
        return res;
    }
    else
    {
        return reformPlasticStrainTensor(creep_strain_incr);
    }
}
