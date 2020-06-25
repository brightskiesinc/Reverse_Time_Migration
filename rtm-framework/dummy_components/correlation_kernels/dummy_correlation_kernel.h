//
// Created by amrnasr on 21/10/2019.
//

#ifndef RTM_FRAMEWORK_DUMMY_CORRELATION_KERNEL_H
#define RTM_FRAMEWORK_DUMMY_CORRELATION_KERNEL_H

#include <skeleton/components/correlation_kernel.h>

class DummyCorrelationKernel : public CorrelationKernel {
public:
  /*!
   * the  De-constructors is overridden here.
   */
  ~DummyCorrelationKernel() override;
  /*!
   * Stacks the single shot correlation array into a stacked shot correlation
   * one.
   */
  /*!this function is a virtual function and will be overridden here*/
  void Stack() override;

  /*!
   * Correlates two frames using the dimensions provided in the meta data using
   * an imaging condition and stacks/writes them in a single shot correlation
   * frame.
   * @param in_1
   * The pointer to the GridBox containing the first frame to be correlated.
   * This should be the simulation or forward propagation result. Our frame of
   * interest is the current one.
   */
  /*!this function is a virtual function and will be overridden here*/
  void Correlate(GridBox *in_1) override;

  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the grid.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetComputationParameters(ComputationParameters *parameters) override;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box
   * The designated grid box to run operations on.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetGridBox(GridBox *grid_box) override;

  void ResetShotCorrelation() override;

  float *GetShotCorrelation() override;

  float *GetStackedShotCorrelation() override;

  MigrationData *GetMigrationData() override;
};

#endif // RTM_FRAMEWORK_DUMMY_CORRELATION_KERNEL_H
