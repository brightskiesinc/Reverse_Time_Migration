//
// Created by zeyad-osama on 02/09/2020.
//

#ifndef PIPELINE_WRITERS_NORMAL_WRITER_HPP
#define PIPELINE_WRITERS_NORMAL_WRITER_HPP

#include <stbx/writers/interface/Writer.hpp>

namespace stbx {
    namespace writers {

        class NormalWriter : public Writer {
        public:
            NormalWriter() = default;

            ~NormalWriter() override = default;

        private:
            void SpecifyRawMigration() override;

            void Initialize() override {};

            void PostProcess() override;
        };
    }//namespace writers
}//namespace stbx

#endif //PIPELINE_WRITERS_NORMAL_WRITER_HPP
