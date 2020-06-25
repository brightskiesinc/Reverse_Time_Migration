#include <concrete-components/boundary_managers/extensions/extension.h>
#include <concrete-components/data_units/staggered_grid.h>
#include <rtm-framework/skeleton/components/boundary_manager.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#include <math.h>
#include <vector>

class StaggeredCPMLBoundaryManager : public BoundaryManager {
private:
  std::vector<Extension *> extensions;
  float max_vel;
  float relax_cp;
  float shift_ratio;
  float reflect_coeff;
  float *small_a_x;
  float *small_a_y;
  float *small_a_z;
  float *small_b_x;
  float *small_b_y;
  float *small_b_z;

  float *auxiliary_vel_x_left;
  float *auxiliary_vel_x_right;

  float *auxiliary_vel_y_up;
  float *auxiliary_vel_y_down;

  float *auxiliary_vel_z_up;
  float *auxiliary_vel_z_down;

  float *auxiliary_ptr_x_left;
  float *auxiliary_ptr_x_right;

  float *auxiliary_ptr_y_up;
  float *auxiliary_ptr_y_down;

  float *auxiliary_ptr_z_up;
  float *auxiliary_ptr_z_down;

  ComputationParameters *parameters;
  StaggeredGrid *grid;

public:
  StaggeredCPMLBoundaryManager(bool use_top_layer = true,
                               float reflect_coeff = 0.1,
                               float shift_ratio = 0.1, float relax_cp = 1);

  ~StaggeredCPMLBoundaryManager() override;
  void fillCpmlCoeff(float *coeff_a, float *coeff_b, int boundary_length,
                     float dh, float dt, float max_vel, float shift_ratio,
                     float reflect_coeff, float relax_cp);
  void zero_auxiliary_variables();
  void ApplyBoundary(uint kernel_id = 0) override;

  void ExtendModel() override;

  void ReExtendModel() override;

  void AdjustModelForBackward() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};
