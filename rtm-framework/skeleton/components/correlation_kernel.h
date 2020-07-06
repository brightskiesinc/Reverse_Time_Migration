//
// Created by amrnasr on 16/10/2019.
//

#ifndef RTM_FRAMEWORK_CORRELATION_KERNEL_H
#define RTM_FRAMEWORK_CORRELATION_KERNEL_H

#include "component.h"
#include <skeleton/base/datatypes.h>
#include <skeleton/base/migration_data.h>

/*
 * enum to hold illumination compensation categories
 * -- no compensation will provide normal cross correlation
 * -- source compensation will compensate the cross correlation stack for the source illumination effect
 * -- receiver compensation will compensate the cross correlation stack for the receiver illumination effect
 * -- combined compensation will compensate the cross correlation stack for both the source and the receiver illumination effect
 */
enum CompensationType{
	NO_COMPENSATION,
	SOURCE_COMPENSATION,
	RECEIVER_COMPENSATION,
	COMBINED_COMPENSATION
};

/*!
 * Correlation Kernel Interface. All concrete techniques for the imaging
 * conditions should be implemented using this interface.
 */
class CorrelationKernel : public Component {
public:
  /*!
   * De-constructors should be overridden to ensure correct memory management.
   */
  virtual ~CorrelationKernel(){};
  /*!
   * Resets the single shot correlation results.
   */
  virtual void ResetShotCorrelation() = 0;
  /*!
   * Stacks the single shot correlation current result into the stacked shot
   * correlation.
   */
  virtual void Stack() = 0;

  /*!
   * Correlates two frames using the dimensions provided in the meta data using
   * an imaging condition and stacks/writes them in the single shot correlation
   * result.
   * @param in_1
   * The pointer to the GridBox containing the first frame to be correlated.
   * This should be the simulation or forward propagation result. Our frame of
   * interest is the current one.
   */
  virtual void Correlate(GridBox *in_1) = 0;
  /*!
   * @return
   * The pointer to the array that should contain the results of the correlation
   * of the frames of a single shot.
   */
  virtual float *GetShotCorrelation() = 0;
  /*!
   * @return
   * The pointer to the array that should contain the results of the stacked
   * correlation of the frames of all shots.
   */
  virtual float *GetStackedShotCorrelation() = 0;
  /*!
   * @return
   * The pointer to the array that should contain the final results and details
   * of the stacked correlation of the frams of all shots.
   */
  virtual MigrationData *GetMigrationData() = 0;

  /*!
   * Sets the compensation category according to CompensationType Enum
   * @param in_1
   * the compensation type that the correlation kernel would output at the end of migration
   */
  virtual void SetCompensation(CompensationType c) = 0;

  /*!
   * @return
   * The pointer to the array that should contain the stacked correlation after
   * it has been compensated for source illumination effect
   */
  virtual float *GetSourceCompensationCorrelation() = 0;
  /*!
   * @return
   * The pointer to the array that should contain the stacked correlation after
   * it has been compensated for receiver illumination effect
   */
  virtual float *GetReceiverCompensationCorrelation() = 0;
  /*!
   * @return
   * The pointer to the array that should contain the stacked correlation after
   * it has been compensated for both source and receiver illumination effects
   */
  virtual float *GetCombinedCompensationCorrelation() = 0;
};

#endif // RTM_FRAMEWORK_CORRELATION_KERNEL_H
