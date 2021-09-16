/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPERATIONS_LIB_DATA_UNITS_GRID_BOX_HPP
#define OPERATIONS_LIB_DATA_UNITS_GRID_BOX_HPP

#include "operations/data-units/concrete/holders/axis/concrete/Axis3D.hpp"
#include "operations/data-units/interface/DataUnit.hpp"
#include "operations/common//DataTypes.h"
#include <bs/io/data-units/concrete/Gather.hpp>
#include "FrameBuffer.hpp"

#include <bs/base/exceptions/Exceptions.hpp>

#include <iostream>
#include <map>
#include <vector>
#include <cstring>

using namespace operations::dataunits::axis;

namespace operations {
    namespace dataunits {
/**
 * @tableofcontents
 *      - Brief
 *      - Keys Description
 *      - Keys
 *
 * @brief
 * Grid box holds all the meta data (i.e. dt, nt, grid size, window
 * size, window start, reference point and cell dimensions) needed
 * by all components.
 *
 * It also holds all wave fields, parameters and parameters window
 * in member maps included in this class.
 *
 * Keys to all maps are of type u_int16_t thus 16 bits are needed for
 * the key to be constructed. These 16 bits are constructed as same as
 * Unix commands
 *
 * @example: (PARM | CURR | DIR_X) meaning that this element is a parameter
 * which have current time step and in the x direction.
 *
 * Keys are discussed more below.
 *
 * @note Using this class should not mess up with the bit mask
 *
 * @keysdescription
 *      - Bit 0:
 *          Identifies an element is a window or not.
 *      - Bit 1,2:
 *          Identifies an element is a wave field or a parameter.
 *              * WAVE      01
 *              * PARM      10
 *      - Bit 3,4,5:
 *          Identifies an element's time, whether it's current, next or previous.
 *              * CURR      001
 *              * PREV      010
 *              * NEXT      011
 *      - Bit 6,7:
 *          Identifies an element's direction. X,Y or Z.
 *              * DIR_Z     00
 *              * DIR_X     01
 *              * DIR_Y     10
 *      - Bit 8,9,10,11,12,13:
 *          Identifies an element's direction. X,Y or Z.
 *              * GB_VEL    000001
 *              * GB_DLT    000010
 *              * GB_EPS    000011
 *              * GB_THT    000100
 *              * GB_PHI    000101
 *              * GB_DEN    000111
 *      - Bit 14,15:
 *          Parity bits.
 *
 *
 * @keys
 *
 * WINDOW               1  00  000  00  000000  00
 *
 * WAVE FIELD           0  01  000  00  000000  00
 * PARAMETER            0  10  000  00  000000  00
 *
 * CURRENT              0  00  001  00  000000  00
 * PREVIOUS             0  00  010  00  000000  00
 * NEXT                 0  00  011  00  000000  00
 *
 * DIRECTION Z          0  00  000  00  000000  00
 * DIRECTION X          0  00  000  01  000000  00
 * DIRECTION Y          0  00  000  10  000000  00
 *
 * VELOCITY             0  00  000  00  000001  00
 * DELTA                0  00  000  00  000010  00
 * EPSILON              0  00  000  00  000011  00
 * THETA                0  00  000  00  000100  00
 * PHI                  0  00  000  00  000101  00
 * DENSITY              0  00  000  00  000111  00
 * PARTICLE VELOCITY    0  00  000  00  001000  00
 * PRESSURE             0  00  000  00  001001  00
 * GB_VEL_RMS           0  00  000  00  001010  00
 * GB_VEL_AVG           0  00  000  00  001011  00
 * GB_TIME_AVG          0  00  000  00  001100  00
*/

/**
 * @note
 * Never add definitions to your concrete implementations
 * with the same names.
 */
#define WIND            0b1000000000000000
#define WAVE            0b0010000000000000
#define PARM            0b0100000000000000
#define CURR            0b0000010000000000
#define PREV            0b0000100000000000
#define NEXT            0b0000110000000000
#define DIR_Z           0b0000000000000000
#define DIR_X           0b0000000100000000
#define DIR_Y           0b0000001000000000
#define GB_VEL          0b0000000000000100
#define GB_DLT          0b0000000000001000
#define GB_EPS          0b0000000000001100
#define GB_THT          0b0000000000010000
#define GB_PHI          0b0000000000010100
#define GB_DEN          0b0000000000011100
#define GB_PRTC         0b0000000000100000
#define GB_PRSS         0b0000000000100100
#define GB_VEL_RMS      0b0000000000101000
#define GB_VEL_AVG      0b0000000000101100
#define GB_TIME_AVG     0b0000000000110000

        /**
         * @brief
         * Grid box holds all the meta data (i.e. dt, nt, grid size, window
         * size, window start, reference point and cell dimensions) needed
         * by all components.
         *
         * It also holds all wave fields, parameters and parameters window
         * in member maps included in this class.
         */
        class GridBox : public DataUnit {
        public:
            typedef u_int16_t Key;

        public:
            /**
             * @brief Constructor
             */
            GridBox();

            /**
             * @brief Define the destructor as virtual which is a member function
             * that is declared within a base class and re-defined (Overridden)
             * by a derived class default = {}
             */
            ~GridBox() override;

            /**
             * @brief DT setter.
             * @throw ILLOGICAL_EXCEPTION()
             */
            void SetDT(float _dt);

            /**
             * @brief DT getter.
             */
            inline float GetDT() const {
                return this->mDT;
            }

            /**
             * @brief NT setter.
             * @throw ILLOGICAL_EXCEPTION()
             */
            void SetNT(float _nt);

            /**
             * @brief NT getter.
             */
            inline uint GetNT() const {
                return this->mNT;
            }

            /**
             * @brief WindowProperties struct getter.
             * @return[out] WindowProperties   Value
             */
            inline WindowProperties *GetWindowProperties() {
                return this->mpWindowProperties;
            }

            /**
             * @brief Window start per axe setter.
             * @param[in] axis      Axe direction
             * @param[in] val       Value to be set
             * @throw ILLOGICAL_EXCEPTION()
             */
            void SetWindowStart(uint axis, uint val);

            /**
             * @brief Window start per axe getter.
             * @param[in] axis      Axe direction
             * @return[out] value   Value
             */
            uint GetWindowStart(uint axis);

            /**
             * @brief Window start struct getter.
             */
            Point3D *GetWindowStart() {
                return &this->mpWindowProperties->window_start;
            }

            /**
             * @brief Initial Size Setter
             * @param[in] ptr_parameter_axis3D             Allocated parameter pointer
             */
            inline void SetInitialAxis(Axis3D<unsigned int> *apInitialAxis) {
                this->mpInitialAxis = apInitialAxis;
            }

            /**
             * @brief Initial Axis getter.
             * @return[out] Axis3D   Value
             */
            inline Axis3D<unsigned int> *GetInitialAxis() {
                return this->mpInitialAxis;
            }

            /**
             * @brief After Sampling Axis Setter
             * @param[in] ptr_parameter_axis3D             Allocated parameter pointer
             */
            inline void SetAfterSamplingAxis(Axis3D<unsigned int> *apAfterSamplingAxis) {
                this->mpAfterSamplingAxis = apAfterSamplingAxis;
            }

            /**
             * @brief After Sampling Axis getter.
             * @return[out] Axis3D   Value
             */
            inline Axis3D<unsigned int> *GetAfterSamplingAxis() {
                return this->mpAfterSamplingAxis;
            }

            /**
             * @brief Window Size Setter
             * @param[in] ptr_parameter_axis3D             Allocated parameter pointer
             */
            inline void SetWindowAxis(Axis3D<unsigned int> *apWindowAxis) {
                this->mpWindowAxis = apWindowAxis;
            }

            /**
             * @brief Window size getter.
             * @return[out] Axis3D   Value
             */
            inline Axis3D<unsigned int> *GetWindowAxis() {
                return this->mpWindowAxis;
            }

            /**
            * @brief Registers an allocated wave field pointer and it's
            * window pointer accompanied with it's value.
            * @param[in] key                        Wave field name
            * @param[in] ptr_wave_field             Allocated wave field pointer
            */
            void RegisterWaveField(u_int16_t key, FrameBuffer<float> *ptr_wave_field);

            /**
             * @brief Registers an allocated parameter pointer
             * accompanied with it's value.
             * @param[in] key                       Parameter name
             * @param[in] ptr_parameter             Allocated parameter pointer
             * @param[in] ptr_parameter_window      Allocated parameter window pointer
             */
            void RegisterParameter(u_int16_t key,
                                   FrameBuffer<float> *ptr_parameter,
                                   FrameBuffer<float> *ptr_parameter_window = nullptr);

            /**
             * @brief Master wave field setter.
             * @return[out] float *
             */
            void RegisterMasterWaveField();

            /**
             * @brief Wave Fields map getter.
             * @return[out] this->mWaveField
             */
            inline std::map<u_int16_t, FrameBuffer<float> *> GetWaveFields() {
                return this->mWaveFields;
            }

            /**
             * @brief Parameters map getter.
             * @return[out] this->mParameters
             */
            inline std::map<u_int16_t, FrameBuffer<float> *> GetParameters() {
                return this->mParameters;
            }

            /**
             * @brief Window Parameters map getter.
             * @return[out] this->mWindowParameters
             */
            inline std::map<u_int16_t, FrameBuffer<float> *> GetWindowParameters() {
                return this->mWindowParameters;
            }

            /**
             * @brief Master wave field getter.
             * @return[out] float *
             */
            float *GetMasterWaveField();

            /**
             * @brief WaveField/Parameter/WindowParameter getter.
             * @param[in] key           Key
             * @return[out] float *      (Parameter | Wave field | Window Parameter) pointer
             * @throw NO_KEY_FOUND_EXCEPTION()
             */
            FrameBuffer<float> *Get(u_int16_t key);

            /**
             * @brief WaveField/Parameter/WindowParameter setter.
             * @param[in] key       Key
             * @param[in] val       FrameBuffer
             * @throw NO_KEY_FOUND_EXCEPTION()
             */
            void Set(u_int16_t key, FrameBuffer<float> *val);

            /**
             * @brief WaveField/Parameter/WindowParameter setter.
             * @param[in] key       Key
             * @param[in] val       float pointer
             * @throw NO_KEY_FOUND_EXCEPTION()
             */
            void Set(u_int16_t key, float *val);

            /**
             * @brief WaveField/Parameter/WindowParameter value re-setter.
             * @param[in] key       Key
             * @note Uses normal setter with a nullptr set value.
             */
            void Reset(u_int16_t key);

            /**
             * @brief Swaps two pointers with respect to the provided keys.
             * @param[in] _src
             * @param[in] _dst
             * @throw NO_KEY_FOUND_EXCEPTION()
             */
            void Swap(u_int16_t _src, u_int16_t _dst);

            /**
             * @brief Clones current grid box into the sent grid box.
             *
             * @param[in] apGridBox
             * GridBox to clone in.
             */
            void Clone(GridBox *apGridBox);

            /**
             * @brief Clones current grid box's meta data into
             * the sent grid box.
             *
             * @param[in] apGridBox
             * GridBox to clone in.
             */
            void CloneMetaData(GridBox *apGridBox);

            /**
             * @brief Clones current grid box's wave fields into
             * the sent grid box.
             *
             * @param[in] apGridBox
             * GridBox to clone in.
             *
             * @note Same pointers for registry are used.
             */
            void CloneWaveFields(GridBox *apGridBox);

            /**
             * @brief Clones current grid box's parameters into
             * the sent grid box.
             *
             * @param[in] apGridBox
             * GridBox to clone in.
             *
             * @note Same pointers for registry are used.
             */
            void CloneParameters(GridBox *apGridBox);

            /**
             * @brief Report all current grid box inner values.
             * @param[in] aReportLevel
             */
            void Report(REPORT_LEVEL aReportLevel = SIMPLE);

            /**
             * @brief WaveField/Parameter/WindowParameter checker.
             * @param[in] key           Key
             * @return[out] boolean
             *
             */
            bool Has(u_int16_t key);

            /**
             * @brief
             * Set the gather for the parameter header values, to an appropriate to
             * utilize to get the appropriate headers.
             *
             * @param[in] apParameterHeaderGather
             * A provided header gather.
             */
            void SetParameterGatherHeader(bs::io::dataunits::Gather *apParameterHeaderGather) {
                this->mpParameterHeadersGather = apParameterHeaderGather;
            }

            /**
             * @brief
             * Return the gather containing the appropriate headers
             * for any parameter field.
             *
             * @return
             * Pointer to gather containing all the needed headers
             * for a parameter field.
             */
            bs::io::dataunits::Gather *GetParameterGatherHeader() {
                return this->mpParameterHeadersGather;
            }

        public:
            /**
             * @param[in] key
             * @param[in] mask
             * @return[out] bool is the mask included in the key.
             */
            static bool inline Includes(u_int16_t key, u_int16_t mask) {
                return (key & mask) == mask;
            }

            /**
             * @brief Masks source bits by provided mask bits.
             * @param[in,out] key
             * @param[in] _src
             * @param[in] _dest
             */
            static void Replace(u_int16_t *key, u_int16_t _src, u_int16_t _dest);

            /**
             * @brief Converts a given key (i.e. u_int16_t) to string.
             * @param[in] key
             * @return[out] String value
             */
            static std::string Stringify(u_int16_t key);

            std::string Beautify(std::string str);

        private:
            /**
             * @brief Bit setter
             * @param[in] key
             * @param[in] type
             */
            static void inline SetBits(u_int16_t *key, u_int16_t type) {
                *key |= type;
            }

            /**
             * @brief Convert first letter in string to uppercase
             * @paramp[in] str to be capitalized
             * @return[out] capitalized string
             */
            std::string Capitalize(std::string str);

            /**
             * @brief Replace all occurrences of a character in string
             * @paramp[in] str to be operated on
             * @paramp[in] str to be changed from
             * @paramp[in] str to be changed to
             * @return[out] replaced string
             */
            std::string ReplaceAll(std::string str,
                                   const std::string &from,
                                   const std::string &to);

        private:
            /// Wave field map
            std::map<u_int16_t, FrameBuffer<float> *> mParameters;
            /// Parameters map
            std::map<u_int16_t, FrameBuffer<float> *> mWindowParameters;
            /// Window Parameters map
            std::map<u_int16_t, FrameBuffer<float> *> mWaveFields;
            /// Initial Size
            Axis3D<unsigned int> *mpInitialAxis;
            /// Size of the After Sampling
            Axis3D<unsigned int> *mpAfterSamplingAxis;
            /// Size of the Window
            Axis3D<unsigned int> *mpWindowAxis;
            /// WindowProperties
            WindowProperties *mpWindowProperties;

            /// Time-step size.
            float mDT;
            /// Number of time steps.
            uint mNT;

            /// Parameter headers(Used to restate the headers of any parameter field).
            bs::io::dataunits::Gather *mpParameterHeadersGather;
        };
    } //namespace dataunits
} //namespace operations

#endif //OPERATIONS_LIB_DATA_UNITS_GRID_BOX_HPP
