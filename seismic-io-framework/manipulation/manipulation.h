#ifndef MANIPULATION_h
#define MANIPULATION_h

#include <../datatypes.h>

class Manipulation {

public:
  // sotring trace form type to another , exapmle from csr to cmp or vice versa
  // , his a high level function that can call nested function that will do the
  // sorting
  void TracesSort(vector<genral_traces> traces_in,
                  vector<genral_traces> traces_sorted, string sort_from,
                  string sort_to);

  void TracesToMatrix(vector<genral_traces> traces, float *data_matrix);
}

#endif MANIPULATION_h