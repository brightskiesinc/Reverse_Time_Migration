#ifndef RTM_FRAMEWORK_DATATYPES_H
#define RTM_FRAMEWORK_DATATYPES_H

// Unsigned integer type to be used for strictly positive numbers.
typedef unsigned int uint;

// Struct describing the number of points in our grid.
typedef struct {
  uint nx;
  uint nz;
  uint ny;
} GridSize;

// The step size in each direction.
typedef struct {
  float dx;
  float dz;
  float dy;
} CellDimensions;

// Point coordinates in 3D.
typedef struct {
  uint x;
  uint z;
  uint y;
} Point3D;

// Integer point in 3D.
typedef struct {
  int x;
  int z;
  int y;
} IPoint3D;

// An object representing our full window size, will not match grid size if a
// window model is applied.
typedef struct {
  /*!Will be set by traces to the appropriate start of the velocity(including
   * boundary). as the trace manager knows the shot location so it will change
   * the start of the window according to the shot location in each shot
   */
  Point3D window_start;

  // Will be set by ModelHandler to match the velocity/density properties.
  uint window_nx;
  uint window_nz;
  uint window_ny;
} WindowSize;

// Parameters needed for the modelling operation.
typedef struct {
  // Starting point for the receivers.
  Point3D receivers_start;
  // The increment step of the receivers.
  Point3D receivers_increment;
  // The end point of the receivers exclusive.
  Point3D receivers_end;
  // The source point for the modelling.
  Point3D source_point;
  // The total time of the simulation in seconds.
  float total_time;
} ModellingConfiguration;

// Structure containing the available traces information.
typedef struct {
  float *traces;
  uint num_receivers_in_x;
  uint num_receivers_in_y;
  uint trace_size_per_timestep;
  uint sample_nt;
  float sample_dt;
} Traces;

// The grid full description : to be implemented as a concrete implementation by
// the different variations.
// in case of variable grid it is class will be inherited from this base class
class GridBox {
public:
  GridSize grid_size;             // size of the grid;
  WindowSize window_size;         // full window size;
  CellDimensions cell_dimensions; // Step sizes of the grid;
  float dt;                       // time step size;
  uint nt;                        // number of time steps;
  float *velocity;                // Velocity;
  float *pressure_current;        // Current pressure;
  Point3D reference_point; // Reference point of the model in real coordinates
                           // distance.
  // define the deconstructor as virtual
  // which is a memebr function that is declared within a base class and
  // re-defined(Overriden) by a derived class default = {}
  virtual ~GridBox() = default;
};

// Enums for the supported order lengths.
// here we put values for each enum variable
// if we make instance of HALF_LENGTH and assign it to O_2 for example it will
// return 1;
enum HALF_LENGTH { O_2 = 1, O_4 = 2, O_8 = 4, O_12 = 6, O_16 = 8 };

// Parameters of the simulation independent from the block.
class ComputationParameters {
public:
  // frequency of the source(ricker wavelet)
  float source_frequency;
  HALF_LENGTH half_length;
  // the boundary length
  int boundary_length;
  // pointer of floats (array of floats) which contains the
  // second_derivative_finite difference coefficient
  float *second_derivative_fd_coeff;
  // pointer of floats (array of floats) which contains the
  // first_derivative_finite difference coefficient
  float *first_derivative_fd_coeff;
  // pointer of floats (array of floats) which contains the
  // first_derivative_staggered_finite difference coefficient
  float *first_derivative_staggered_fd_coeff;
  // is the stability condition safety /relaxation factor
  float dt_relax;

  // the constructor of the class, it takes as input the half_length
  explicit ComputationParameters(HALF_LENGTH hl) {
    // assign the values of the class
    source_frequency = 200;
    boundary_length = 20;
    half_length = hl;
    dt_relax = 0.4;
    // array of floats of size hl+1 only contains the zero and positive (x>0 )
    // coefficients and not all coefficients
    second_derivative_fd_coeff = new float[hl + 1];
    // array of floats of size hl+1 only contains the zero and positive (x>0 )
    // coefficients and not all coefficients
    first_derivative_fd_coeff = new float[hl + 1];
    // array of floats of size hl+1 only contains the zero and positive (x>0 )
    // coefficients and not all coefficients
    first_derivative_staggered_fd_coeff = new float[hl + 1];

    // accuracy = 2 and half_length=1
    if (hl == O_2) {
      second_derivative_fd_coeff[0] = -2;
      second_derivative_fd_coeff[1] = 1;
      first_derivative_fd_coeff[0] = 0;
      first_derivative_fd_coeff[1] = 0.5;
      first_derivative_staggered_fd_coeff[0] = 0.0f;
      first_derivative_staggered_fd_coeff[1] = 1.0f;

      // accuracy = 4 and half_length=2
    } else if (hl == O_4) {
      second_derivative_fd_coeff[0] = -2.5;
      second_derivative_fd_coeff[1] = 1.33333333333;
      second_derivative_fd_coeff[2] = -0.08333333333;
      first_derivative_fd_coeff[0] = 0.0f;
      first_derivative_fd_coeff[1] = 2.0f / 3.0f;
      first_derivative_fd_coeff[2] = -1.0f / 12.0f;
      first_derivative_staggered_fd_coeff[0] = 0.0f;
      first_derivative_staggered_fd_coeff[1] = 1.125f;
      first_derivative_staggered_fd_coeff[2] = -0.041666666666666664f;

      // accuracy = 8 and half_length=4
    } else if (hl == O_8) {
      second_derivative_fd_coeff[0] = -2.847222222;
      second_derivative_fd_coeff[1] = +1.6;
      second_derivative_fd_coeff[2] = -0.2;
      second_derivative_fd_coeff[3] = +2.53968e-2;
      second_derivative_fd_coeff[4] = -1.785714e-3;
      first_derivative_fd_coeff[0] = 0;
      first_derivative_fd_coeff[1] = +0.8;
      first_derivative_fd_coeff[2] = -0.2;
      first_derivative_fd_coeff[3] = +0.03809523809;
      first_derivative_fd_coeff[4] = -0.00357142857;
      first_derivative_staggered_fd_coeff[0] = 0.0f;
      first_derivative_staggered_fd_coeff[1] = 1.1962890625f;
      first_derivative_staggered_fd_coeff[2] = -0.07975260416666667f;
      first_derivative_staggered_fd_coeff[3] = 0.0095703125f;
      first_derivative_staggered_fd_coeff[4] = -0.0006975446428571429f;

      // accuracy=12 and half_length=6
    } else if (hl == O_12) {
      second_derivative_fd_coeff[0] = -2.98277777778;
      second_derivative_fd_coeff[1] = 1.71428571429;
      second_derivative_fd_coeff[2] = -0.26785714285;
      second_derivative_fd_coeff[3] = 0.05291005291;
      second_derivative_fd_coeff[4] = -0.00892857142;
      second_derivative_fd_coeff[5] = 0.00103896103;
      second_derivative_fd_coeff[6] = -0.00006012506;
      first_derivative_fd_coeff[0] = 0.0;
      first_derivative_fd_coeff[1] = 0.857142857143;
      first_derivative_fd_coeff[2] = -0.267857142857;
      first_derivative_fd_coeff[3] = 0.0793650793651;
      first_derivative_fd_coeff[4] = -0.0178571428571;
      first_derivative_fd_coeff[5] = 0.0025974025974;
      first_derivative_fd_coeff[6] = -0.000180375180375;
      first_derivative_staggered_fd_coeff[0] = 0.0f;
      first_derivative_staggered_fd_coeff[1] = 1.2213363647460938f;
      first_derivative_staggered_fd_coeff[2] = -0.09693145751953125f;
      first_derivative_staggered_fd_coeff[3] = 0.017447662353515626f;
      first_derivative_staggered_fd_coeff[4] = -0.002967289515904018f;
      first_derivative_staggered_fd_coeff[5] = 0.0003590053982204861f;
      first_derivative_staggered_fd_coeff[6] = -2.184781161221591e-05f;

      // accuracy = 16 and half_length=8
    } else if (hl == O_16) {
      second_derivative_fd_coeff[0] = -3.05484410431;
      second_derivative_fd_coeff[1] = 1.77777777778;
      second_derivative_fd_coeff[2] = -0.311111111111;
      second_derivative_fd_coeff[3] = 0.0754208754209;
      second_derivative_fd_coeff[4] = -0.0176767676768;
      second_derivative_fd_coeff[5] = 0.00348096348096;
      second_derivative_fd_coeff[6] = -0.000518000518001;
      second_derivative_fd_coeff[7] = 5.07429078858e-05;
      second_derivative_fd_coeff[8] = -2.42812742813e-06;
      first_derivative_fd_coeff[0] = -6.93889390391e-17;
      first_derivative_fd_coeff[1] = 0.888888888889;
      first_derivative_fd_coeff[2] = -0.311111111111;
      first_derivative_fd_coeff[3] = 0.113131313131;
      first_derivative_fd_coeff[4] = -0.0353535353535;
      first_derivative_fd_coeff[5] = 0.00870240870241;
      first_derivative_fd_coeff[6] = -0.001554001554;
      first_derivative_fd_coeff[7] = 0.0001776001776;
      first_derivative_fd_coeff[8] = -9.71250971251e-06;
      first_derivative_staggered_fd_coeff[0] = 0.0f;
      first_derivative_staggered_fd_coeff[1] = 1.2340910732746122f;
      first_derivative_staggered_fd_coeff[2] = -0.10664984583854668f;
      first_derivative_staggered_fd_coeff[3] = 0.023036366701126076f;
      first_derivative_staggered_fd_coeff[4] = -0.005342385598591385f;
      first_derivative_staggered_fd_coeff[5] = 0.0010772711700863268f;
      first_derivative_staggered_fd_coeff[6] = -0.00016641887751492495f;
      first_derivative_staggered_fd_coeff[7] = 1.7021711056048922e-05f;
      first_derivative_staggered_fd_coeff[8] = -8.523464202880773e-07f;
    }
  }
  // the deconstructor is virtual which can be overriden in derived classes from
  // this class
  virtual ~ComputationParameters() {
    // delete [] operator that is used to destroy array  objects which are
    // created by new expression.
    // destroy the array of floats of coefficients of the second derivative
    // finite difference
    delete[] second_derivative_fd_coeff;
    // destroy the array of floats of coefficients of the first derivative
    // finite difference
    delete[] first_derivative_fd_coeff;
    // destroy the array of floats of coefficients of the first derivative
    // staggered finite difference
    delete[] first_derivative_staggered_fd_coeff;
  };
};
#endif // RTM_FRAMEWORK_DATATYPES_H
