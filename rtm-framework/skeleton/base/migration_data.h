#ifndef RTM_FRAMEWORK_MIGRATION_H
#define RTM_FRAMEWORK_MIGRATION_H

// Unsigned integer type to be used for strictly positive numbers.
typedef unsigned int uint;

class MigrationData {

public:
  uint nx;
  uint nz;
  uint ny;
  uint nt;

  float dx;
  float dy;
  float dz;
  float dt;

  float *stacked_correlation;
  // define the deconstructor as virtual
  // which is a memebr function that is declared within a base class and
  // re-defined(Overriden) by a derived class default = {}

  MigrationData(uint nx, uint nz, uint ny, uint nt, float dx, float dz,
                float dy, float dt, float *stacked_correlation) {

    this->nx = nx;
    this->ny = ny;
    this->nz = nz;
    this->nt = nt;

    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    this->dt = dt;

    this->stacked_correlation = stacked_correlation;
  }

  virtual ~MigrationData() = default;
};

#endif // RTM_FRAMEWORK_MIGRATION_H