[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 50
  ny = 1
  nz = 1
  xmin = -50
  xmax = 50
  ymin = 0
  ymax = 5
  zmin = 0
  zmax = 5
[]

[Variables]
  [pf]
    type = MooseVariableFVReal
  []
[]

[FVKernels]
  [fluid_time_derivative]
    type = BVFVSinglePhaseTimeDerivative
    variable = pf
  []
  [darcy]
    type = BVFVSinglePhaseDarcy
    variable = pf
  []
[]

[Functions]
  [pf_func]
    type = ParsedFunction
    expression = 'p1*t'
    symbol_names = 'p1'
    symbol_values = '23.14814815'
  []
[]

[AuxVariables]
  [vf_x]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [vf_x_aux]
    type = BVDarcyVelocityAux
    variable = vf_x
    fluid_pressure = pf
    component = x
    execute_on = 'TIMESTEP_END'
  []
[]

[FVBCs]
  [p_left]
    type = FVFunctionDirichletBC
    variable = pf
    boundary = left
    function = pf_func
  []
  [p_right]
    type = FVFunctionDirichletBC
    variable = pf
    boundary = right
    function = pf_func
  []
[]

[Materials]
  [fluid_flow_mat]
    type = BVSinglePhaseFlowMaterial
  []
  [permeability]
    type = BVConstantPermeability
    permeability = 1.0e-14
  []
  [porosity]
    type = BVConstantPorosity
    porosity = 1.0
  []
  [fluid_properties]
    type = BVFluidProperties
    fluid_pressure = pf
    temperature = 273
    fp = simple_fluid
  []
[]

[FluidProperties]
  [simple_fluid]
    type = SimpleFluidProperties
    density0 = 1.0
    viscosity = 1.728e-03
    bulk_modulus = 4.0e+09
  []
[]

[Preconditioning]
  [asm]
    type = SMP
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -ksp_rtol -ksp_max_it
                           -pc_type
                           -sub_pc_type
                           -snes_type -snes_atol -snes_rtol -snes_max_it -snes_linesearch_type
                           -ksp_gmres_restart'
    petsc_options_value = 'fgmres 1e-10 100
                           asm
                           ilu
                           newtonls 1e-12 1e-08 100 basic
                           201'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  automatic_scaling = true
  residual_and_jacobian_together = true
  start_time = 0.0
  end_time = 43200
  num_steps = 10
[]

[Outputs]
  interval = 5
  exodus = true
[]
