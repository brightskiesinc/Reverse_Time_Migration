# Axis

<p>
The axis is a meta-data holder for each axis. 
</p>

It holds the **Axis Size**, **Boundary**, **Computational Padding** and **Half-Length Padding**.

`Axis Size` : The size of the axis.  
`Boundary` : The boundary added to the subsurface model due to memory limitations.  
`Computational Padding` : Each technology needs different padding in order to perform the computations more
efficiently.  
`Half-Length Padding` : AKA *Half-Length*, it is the padding added at both ends of an axis to help perform finite
difference on the first and last points of an axis.

## Interface

An abstract template class, it :

- Has a constructor and a copy constructor.
- Initializes the Axis with the initial size.
- Adds Boundary to the Front, Rear or Both ends.
- Adds Computational Padding to the Front, Rear or Both ends.
- Adds Half-Length Padding to the Front, Rear or Both ends.
- Calculates the `Logical Axis Size`.
- Calculates the `Actual Axis Size`.
- Calculates the `Computation Axis Size`.
- Has getters for boundaries, paddings and different axis size.
- Supports `unsigned short int`, `unsigned int` and `unsigned long int` datatypes.

### Calculations

* **Logical Axis Size** :  
  `Axis Size +  
  Front Boundary Length + Rear Boundary Length +  
  Front Half-Length Padding + Rear Half-Length Padding`
* **Actual Axis Size**  :  
  `Axis Size + Front Boundary Length + Rear Boundary Length +  
  Front Computational Padding + Rear Computational Paddingg +  
  Front Half-Length Padding + Rear Half-Length Padding`
* **Computation Axis Size** :  
  `Axis Size + Front Boundary Length + Rear Boundary Length + Front Computational Padding + Rear Computational Paddingg`

## RegularAxis

This is an implementation of the Axis Interface for *the Regular Sampling*.

To add boundary or padding, we should use:  
`OP_DIREC_FRONT` : to add at the front end of the axis.  
`OP_DIREC_REAR`  : to add at the rear end of the axis.  
`OP_DIREC_BOTH`  : to add at both ends of the axis.

### Usage

1. To create new instance:
   ```c++
        auto regular_axis = RegularAxis<unsigned int>(axis_size);
   ```
2. To add Boundaries, Computational and Half-Length Paddings:

   ```c++
       rc = regular_axis.AddBoundary(OP_DIREC_FRONT, front_boundary);
       rc = regular_axis.AddBoundary(OP_DIREC_REAR, rear_boundary);
       rc = regular_axis.AddHalfLengthPadding(OP_DIREC_BOTH, halfLength_padding);
       rc = regular_axis.AddComputationalPadding(OP_DIREC_FRONT, front_computational_padding);
       rc = regular_axis.AddComputationalPadding(OP_DIREC_REAR, rear_computational_padding);
   ```

3. To use Getters:

    ```c++
        front_boundary = regular_axis.GetFrontBoundaryLength();
        rear_boundary = regular_axis.GetRearBoundaryLength();
        front_halfLength_padding = regular_axis.GetFrontHalfLengthPadding();
        rear_halfLength_padding = regular_axis.GetRearHalfLengthPadding();
        front_computational_padding =  regular_axis.GetFrontComputationalPadding() ;
        rear_computational_padding = regular_axis.GetRearComputationalPadding();
   
        axis_size = regular_axis.GetAxisSize() ;
        logical_size = regular_axis.GetLogicalAxisSize();
        actual_size = regular_axis.GetActualAxisSize();
        computation_size = regular_axis.GetComputationAxisSize();
    ```

4. To use copy constructor:
   ```c++
         auto regular_axis_copy = regular_axis;
   ```

## Axis3D

This is a simple wrapper class to wrap the X, Y and Z axis in one class for easier use.  
It has:

- Constructor that gets the 3 axis size as parameters.
- Copy constructor.
- 3 Regular Axis; X, Y and Z.
- Vector of pointers to the 3 axis.
- Getter for each axis and the vector.

### Usage

1. To create new instance:
   ```c++
       auto axis_3d = Axis3D<unsigned int>(x_axis_size, z_axis_size, y_axis_size);
   ```
2. To do operations on each axis individually:

   ```c++
       rc = axis_3d.GetXAxis().AddBoundary(OP_DIREC_FRONT, front_boundary);
       rc = axis_3d.GetXAxis().AddBoundary(OP_DIREC_REAR, rear_boundary);
       rc = axis_3d.GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH, halfLength_padding);
       rc = axis_3d.GetXAxis().AddComputationalPadding(OP_DIREC_FRONT, front_computational_padding);
       rc = axis_3d.GetXAxis().AddComputationalPadding(OP_DIREC_REAR, rear_computational_padding);

    ```

3. To use Getters on each axis individually:

    ```c++
        x_axis_size = axis_3d.GetXAxis().GetAxisSize() ;
        x_axis_logical_size = axis_3d.GetXAxis().GetLogicalAxisSize() 
        x_axis_actual_size = axis_3d.GetXAxis().GetActualAxisSize() 
        x_axis_computation_size = axis_3d.GetXAxis().GetComputationAxisSize()
    
        y_axis_size = axis_3d.GetYAxis().GetAxisSize() 
        y_axis_logical_size = axis_3d.GetYAxis().GetLogicalAxisSize() 
        y_axis_actual_size = axis_3d.GetYAxis().GetActualAxisSize() 
        y_axis_computation_size = axis_3d.GetYAxis().GetComputationAxisSize() 
    
        z_axis_size = axis_3d.GetZAxis().GetAxisSize()
        z_axis_logical_size = axis_3d.GetZAxis().GetLogicalAxisSize() 
        z_axis_actual_size = axis_3d.GetZAxis().GetActualAxisSize() 
        z_axis_computation_size = axis_3d.GetZAxis().GetComputationAxisSize()
    ```

4. To add values by looping over the vector:
   ```c++
       std::vector<RegularAxis<unsigned int> *>::iterator iter, end;
       for (iter = vec.begin(), end = vec.end(); iter != end; ++iter) {
           rc = (*iter)->AddHalfLengthPadding(OP_DIREC_FRONT, front_halfLength_padding);
           rc = (*iter)->AddHalfLengthPadding(OP_DIREC_REAR, rear_halfLength_padding);
           rc = (*iter)->AddBoundary(OP_DIREC_FRONT, front_boundary);
           rc = (*iter)->AddBoundary(OP_DIREC_REAR, rear_boundary);
           rc = (*iter)->AddComputationalPadding(OP_DIREC_FRONT, front_computational_padding);
           rc = (*iter)->AddComputationalPadding(OP_DIREC_REAR, rear_computational_padding);
       }
   ```

5. To use copy constructor:
   ```c++
         auto copy_axis_3d = axis_3d;
   ```


