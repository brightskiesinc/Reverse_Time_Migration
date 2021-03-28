//
// Created by amr-nasr on 05/11/19.
//

#ifndef OPERATIONS_LIB_CALLBACK_COLLECTION_HPP
#define OPERATIONS_LIB_CALLBACK_COLLECTION_HPP

#include <operations/helpers/callbacks/interface/Callback.hpp>

#include <vector>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class CallbackCollection {
            public:
                void RegisterCallback(Callback *apCallback);

                void BeforeInitialization(common::ComputationParameters *apParameters);

                void AfterInitialization(dataunits::GridBox *apGridBox);

                void BeforeShotPreprocessing(dataunits::TracesHolder *apTraces);

                void AfterShotPreprocessing(dataunits::TracesHolder *apTraces);

                void BeforeForwardPropagation(dataunits::GridBox *apGridBox);

                void AfterForwardStep(dataunits::GridBox *apGridBox, uint time_step);

                void BeforeBackwardPropagation(dataunits::GridBox *apGridBox);

                void AfterBackwardStep(dataunits::GridBox *apGridBox, uint time_step);

                void AfterFetchStep(dataunits::GridBox *apGridBox, uint time_step);

                void BeforeShotStacking(dataunits::GridBox *apGridBox,
                                        dataunits::FrameBuffer<float> *shot_correlation);

                void AfterShotStacking(dataunits::GridBox *apGridBox,
                                       dataunits::FrameBuffer<float> *stacked_shot_correlation);

                void AfterMigration(dataunits::GridBox *apGridBox,
                                    dataunits::FrameBuffer<float> *stacked_shot_correlation);

                std::vector<Callback *> &GetCallbacks();

            private:
                std::vector<Callback *> callbacks;
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_CALLBACK_COLLECTION_HPP
