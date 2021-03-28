//
// Created by amr-nasr on 18/01/2020.
//

#ifndef OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H
#define OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H

#include <operations/helpers/callbacks/interface/Callback.hpp>

#include <fstream>
#include <string>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class NormWriter : public Callback {
            public:
                NormWriter(uint show_each, bool write_forward, bool write_backward,
                           bool write_reverse, const std::string &write_path);

                ~NormWriter();

                void BeforeInitialization(common::ComputationParameters *apParameters) override;

                void AfterInitialization(dataunits::GridBox *apGridBox) override;

                void BeforeShotPreprocessing(dataunits::TracesHolder *apTraces) override;

                void AfterShotPreprocessing(dataunits::TracesHolder *apTraces) override;

                void BeforeForwardPropagation(dataunits::GridBox *apGridBox) override;

                void AfterForwardStep(dataunits::GridBox *apGridBox, uint aTimeStep) override;

                void BeforeBackwardPropagation(dataunits::GridBox *apGridBox) override;

                void AfterBackwardStep(dataunits::GridBox *apGridBox, uint aTimeStep) override;

                void AfterFetchStep(dataunits::GridBox *apGridBox, uint aTimeStep) override;

                void BeforeShotStacking(dataunits::GridBox *apGridBox,
                                        dataunits::FrameBuffer<float> *apShotCorrelation) override;

                void AfterShotStacking(dataunits::GridBox *apGridBox,
                                       dataunits::FrameBuffer<float> *apStackedShotCorrelation) override;

                void AfterMigration(dataunits::GridBox *apGridBox,
                                    dataunits::FrameBuffer<float> *apStackedShotCorrelation) override;


            public:
                std::string GetExtension();

            public:
                static float Solve(const float *apMatrix, uint nx, uint nz, uint ny);

            private:
                uint show_each;
                bool write_forward;
                bool write_backward;
                bool write_reverse;
                std::string write_path;

                std::ofstream *forward_norm_stream;
                std::ofstream *reverse_norm_stream;
                std::ofstream *backward_norm_stream;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_HELPERS_CALLBACKS_NORM_WRITER_H
