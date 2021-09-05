/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_IO_LOOKUPS_BINARY_HEADERS_LOOKUP_HPP
#define BS_IO_LOOKUPS_BINARY_HEADERS_LOOKUP_HPP

namespace bs {
    namespace io {
        namespace lookups {

#define IO_SIZE_BINARY_HEADER       400      /* Size of the binary header in SEG-Y and SU files */
#define IO_POS_S_BINARY_HEADER      3200     /* Start position of binary header in SEG-Y and SU files */
#define IO_POS_E_BINARY_HEADER      3600     /* End position of binary header in SEG-Y and SU files */

            /**
             * @brief SEG-Y and SU binary header lookup. Variable types corresponds to
             * the number of allocated byte(s) for this variable internally in the SEG-Y or SU file
             * according to the general format.
             *
             * @note All variables are stored as big endian, if the machine used is little endian
             * big endian to little endian conversion should take place.
             */
            struct BinaryHeaderLookup {
                int32_t JOBID;
                int32_t LINO;
                int32_t RENO;
                int16_t NTRPR;
                int16_t NART;
                uint16_t HDT;
                int16_t DTO;
                uint16_t HNS;
                uint16_t NSO;
                int16_t FORMAT;
                int16_t FOLD;
                int16_t TSORT;
                int16_t VSCODE;
                int16_t HSFS;
                int16_t HSFE;
                int16_t HSLEN;
                int16_t HSTYP;
                int16_t SCHN;
                int16_t HSTAS;
                int16_t HSTAE;
                int16_t HTATYP;
                int16_t HCORR;
                int16_t BGRCV;
                int16_t RCVM;
                int16_t MFEET;
                int16_t POLYT;
                int16_t VPOL;
                uint8_t UNASSIGNED1[240];
                int16_t SEGY_REV_NUM;
                int16_t FIXED_LEN;
                int16_t EXT_HEAD;
                uint8_t UNASSIGNED2[94];
            };
        } //namespace lookups
    } //namespace io
} //namespace bs

#endif //BS_IO_LOOKUPS_BINARY_HEADERS_LOOKUP_HPP
