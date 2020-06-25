#include <concrete-components/boundary_managers/extensions/extension.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <rtm-framework/skeleton/components/boundary_manager.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#include <math.h>

class CPMLBoundaryManager : public BoundaryManager {
private:
  Extension *extension;

  float *coeff_a_x;
  float *coeff_b_x;
  float *coeff_a_z;
  float *coeff_b_z;
  float *coeff_a_y;
  float *coeff_b_y;

  float *aux_1_x_up;
  float *aux_1_x_down;
  float *aux_1_z_up;
  float *aux_1_z_down;
  float *aux_1_y_up;
  float *aux_1_y_down;

  float *aux_2_x_up;
  float *aux_2_x_down;
  float *aux_2_z_up;
  float *aux_2_z_down;
  float *aux_2_y_up;
  float *aux_2_y_down;

  float max_vel;

  float relax_cp;
  float shift_ratio;
  float reflect_coeff;

  ComputationParameters *parameters;

  AcousticSecondGrid *grid;

  template <int direction> void FillCpmlCoeff();

  template <int direction, bool opposite, int half_length>
  void CalculateFirstAuxilary();

  template <int direction, bool opposite, int half_length>
  void CalculateCpmlValue();

  template <int half_length> void ApplyAllCpml();

  void InitializeVariables();

  void ResetVariables();

public:
  CPMLBoundaryManager(bool use_top_layer = true, float reflect_coeff = 0.1,
                      float shift_ratio = 0.1, float relax_cp = 1);

  ~CPMLBoundaryManager() override;

  void ApplyBoundary(uint kernel_id = 0) override;

  void ExtendModel() override;

  void ReExtendModel() override;

  void AdjustModelForBackward() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};