//
// Created by zeyad-osama on 02/09/2020.
//

#include <stbx/writers/concrete/NormalWriter.hpp>

#define EPSILON 1e-20 // very small number to protect from division by zero

using namespace std;
using namespace stbx::writers;
using namespace operations::utils::filters;
using namespace operations::utils::io;

void NormalWriter::SpecifyRawMigration() {

    mRawMigration = mpMigrationData->GetResultAt(0)->GetData();
}

void NormalWriter::PostProcess() {
    auto migration_results = this->mpMigrationData->GetResults();

    if (migration_results.size() != 1) { // combined compensation

        int model_size = mpMigrationData->GetGridSize(X_AXIS) *
                         mpMigrationData->GetGridSize(Y_AXIS) *
                         mpMigrationData->GetGridSize(Z_AXIS);

        float *source_illumination = mpMigrationData->GetResultAt(1)->GetData();
        float *receiver_illumination = mpMigrationData->GetResultAt(2)->GetData();


        for (int idx = 0; idx < model_size; idx++) {
            mRawMigration[idx] = mRawMigration[idx] / (source_illumination[idx] * receiver_illumination[idx] + EPSILON);
        }
    }
}
