//
// Created by amr-nasr on 13/11/2019.
//

#ifndef OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_BINARY_TRACE_WRITER_HPP
#define OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_BINARY_TRACE_WRITER_HPP

#include <operations/components/independents/primitive/TraceWriter.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>

namespace operations {
    namespace components {

        class BinaryTraceWriter : public TraceWriter, public dependency::HasNoDependents {
        public:
            explicit BinaryTraceWriter(operations::configuration::ConfigurationMap *apConfigurationMap);

            ~BinaryTraceWriter() override;

            void InitializeWriter(ModellingConfiguration *apModellingConfiguration,
                                  std::string output_filename) override;

            void RecordTrace() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AcquireConfiguration() override;

        private:
            common::ComputationParameters *mpParameters = nullptr;

            dataunits::GridBox *mpGridBox = nullptr;

            std::ofstream *mpOutStream = nullptr;

            Point3D mReceiverStart;

            Point3D mReceiverEnd;

            Point3D mReceiverIncrement;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_BINARY_TRACE_WRITER_HPP
