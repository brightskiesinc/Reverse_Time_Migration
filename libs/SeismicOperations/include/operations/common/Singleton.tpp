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

#ifndef OPERATIONS_LIB_BASE_SINGLETON_HPP
#define OPERATIONS_LIB_BASE_SINGLETON_HPP

#include <iostream>


namespace operations {
    namespace common {
/**
 * @brief Template class for any singleton class to inherit.
 * @tparam[in] T The type for the singleton pattern to be applied to.
 */
        template<typename T>
        class Singleton {
        public:
            /**
             * @brief Static function to obtain the instance of the singleton to use.
             * @return An instance of the singleton.
             */
            static T *GetInstance() {
                if (INSTANCE == nullptr) {
                    INSTANCE = new T;
                }
                return INSTANCE;
            }

            /**
             * @brief Destroy the active instance of the singleton.
             */
            static T *Kill() {
                if (INSTANCE != nullptr) {
                    delete INSTANCE;
                    INSTANCE = nullptr;
                }
                return INSTANCE;
            }

        protected:
            /**
             * @brief Default constructor.
             */
            Singleton() = default;

            /**
             * @brief Default Destructor.
             */
            ~Singleton() = default;

        private:
            /// The instance to utilize in the class.
            static T *INSTANCE;
        };

        template<typename T> T *Singleton<T>::INSTANCE = nullptr;

    }//namespace common
}//namespace operations

#endif //OPERATIONS_LIB_BASE_SINGLETON_HPP
