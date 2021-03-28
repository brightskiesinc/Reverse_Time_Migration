//
// Created by zeyad-osama on 09/03/2021.
//

#ifndef THOTH_DATA_UNITS_HELPERS_TRACE_HELPER_HPP
#define THOTH_DATA_UNITS_HELPERS_TRACE_HELPER_HPP

#include <thoth/data-units/concrete/Trace.hpp>
#include <thoth/lookups/tables/TraceHeaderLookup.hpp>
#include <thoth/lookups/tables/BinaryHeaderLookup.hpp>

namespace thoth {
    namespace dataunits {
        namespace helpers {

            /**
             * @brief Trace helper to take any trace data unit and helps manipulate
             * or get any regarded meta data from it.
             */
            class TraceHelper {
            public:
                static int
                Weight(Trace *&apTrace,
                       lookups::TraceHeaderLookup aTraceHeaderLookup,
                       lookups::BinaryHeaderLookup aBinaryHeaderLookup);

                static int
                WeightData(Trace *&apTrace,
                           lookups::TraceHeaderLookup aTraceHeaderLookup,
                           lookups::BinaryHeaderLookup aBinaryHeaderLookup);

                static int
                WeightCoordinates(Trace *&apTrace,
                                  lookups::TraceHeaderLookup aTraceHeaderLookup,
                                  lookups::BinaryHeaderLookup aBinaryHeaderLookup);
            };
        } //namespace helpers
    } //namespace dataunits
} //namespace thoth

#endif //THOTH_DATA_UNITS_HELPERS_TRACE_HELPER_HPP
