//
// Created by amr-nasr on 13/11/2019.
//

#include "operations/components/independents/concrete/trace-writers/BinaryTraceWriter.hpp"


using namespace std;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;

void BinaryTraceWriter::RecordTrace() {
    int x_inc = this->mReceiverIncrement.x == 0 ? 1 : this->mReceiverIncrement.x;
    int y_inc = this->mReceiverIncrement.y == 0 ? 1 : this->mReceiverIncrement.y;
    int z_inc = this->mReceiverIncrement.z == 0 ? 1 : this->mReceiverIncrement.z;
    int wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int wnz_wnx = this->mpGridBox->GetActualWindowSize(Z_AXIS) * wnx;

    float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    for (int iz = this->mReceiverStart.z; iz < this->mReceiverEnd.z; iz += z_inc) {
        for (int iy = this->mReceiverStart.y; iy < this->mReceiverEnd.y; iy += y_inc) {
            for (int ix = this->mReceiverStart.x; ix < this->mReceiverEnd.x; ix += x_inc) {
                int offset = iy * wnz_wnx + iz * wnx + ix;
                this->mpOutStream->write((char *) &pressure[offset],
                                         sizeof(pressure[offset]));
            }
        }
    }
}
