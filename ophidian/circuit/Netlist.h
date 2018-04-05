/*
 * Copyright 2017 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.
 */

#ifndef OPHIDIAN_CIRCUIT_NETLIST_H
#define OPHIDIAN_CIRCUIT_NETLIST_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/entity_system/Composition.h>
#include <unordered_map>

namespace ophidian
{
    namespace circuit
    {
        class Cell :
            public entity_system::EntityBase
        {
        public:
            using entity_system::EntityBase::EntityBase;
        };

        class Pin :
            public entity_system::EntityBase
        {
        public:
            using entity_system::EntityBase::EntityBase;
        };

        class Net :
            public entity_system::EntityBase
        {
        public:
            using entity_system::EntityBase::EntityBase;
        };

        class Input :
            public entity_system::EntityBase
        {
        public:
            using entity_system::EntityBase::EntityBase;
        };

        class Output :
            public entity_system::EntityBase
        {
        public:
            using entity_system::EntityBase::EntityBase;
        };

/*! A flatten Netlist */
        class Netlist final
        {
        public:
            using CellNotifier = entity_system::EntitySystem <Cell>::NotifierType;
            using PinNotifier = entity_system::EntitySystem <Pin>::NotifierType;
            using NetNotifier = entity_system::EntitySystem <Net>::NotifierType;

            //! Construct Netlist

            /*!
               \brief Constructs an empty Netlist, with no Cells, Pins or Nets.
             */
            Netlist();

            //! Move Constructor
            Netlist(Netlist && nl) = default;

            //! Netlist Destructor

            /*!
               \brief This destroy all Netlist's EntitySystems.
             */
            ~Netlist();

//! Add Cell

/*!
   \param A cell name;
   \brief Adds a Cell instance, if the cell already exist then just return the existing cell.
   \return A handler for the created/existing Cell.
 */
            Cell add(Cell, std::string cellName);

//! Erase Cell

/*!
   \param cell A handler for the Cell to erase.
   \brief Erases a Cell instance.
 */
            void erase(const Cell & cell);

//! Size of Cell's System

/*!
   \brief Returns the number of Cells.
   \return The number of Cells.
 */
            uint32_t size(Cell) const;

//! Iterator to beginning

/*!
   \brief Returns an iterator pointing to the first element in the Cell's EntitySystem.
   \return Iterator to the first element in the Cell's EntitySystem.
 */
            entity_system::EntitySystem <Cell>::const_iterator begin(Cell) const;

//! Iterator to end

/*!
   \brief Returns an iterator referring to the past-the-end element in the Cell's EntitySystem.
   \return Iterator referring to the past-the-end element in the Cell's EntitySystem.
 */
            entity_system::EntitySystem <Cell>::const_iterator end(Cell) const;

//! Make Cell Property

/*!
   \brief Creates a Property for the Cell's Entity System.
   \tparam Value value type of the Property.
   \return An Cell => \p Value Map.
 */
            template <typename Value>
            entity_system::Property <Cell, Value> makeProperty(Cell) const
            {
                return entity_system::Property <Cell, Value>(mCells);
            }

//! Get the Cell Notifier

/*!
   \brief Returns a pointer to the AlterationNotifier of the Cell's EntitySystem.
   \return A pointer to the AlterationNotifier of the Cell's EntitySystem.
 */
            entity_system::EntitySystem <Cell>::NotifierType * notifier(Cell) const;

//! Allocate space for storing Cell entities

/*!
   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions cells), then it is worth reserving space for this amount before starting to build the netlist.
   \param size Minimum capacity for the cell container.
 */
            void reserve(Cell, uint32_t size);

//! Capacity of the Cell's System

/*!
   \return The capacity of the Cell EntitySystem.
 */
            uint32_t capacity(Cell) const;

//! Find a cell

/*!
   \brief Using the mapping, return a cell handler by cell's name.
   \param The cell name.
   \return Return a cell handler by cell's name.
 */
            Cell find(Cell, std::string cellName);

//! Returns the name of the cell

/*!
   \brief Returns the name of the cell.
   \param A handler for the cell.
   \return Return the cell's name.
 */
            std::string name(const Cell & cell) const;

//! Pins of a Cell

/*!
   \brief Returns a Container Wrapper for the Pins of a Cell.
   \param cell A handler for the Cell we want to get the Pins.
   \return Container Wrapper for the Pins of a Cell.
 */
            entity_system::Association <Cell, Pin>::Parts pins(const Cell & cell) const;

//! Add Pin into Cell

/*!
   \brief Adds a Pin to a given Cell.
   \param cell A handler for the Cell we want to add a Pin.
   \param pin A handler for the Pin we want to add in \p cell.
 */
            void add(const Cell & cell, const Pin & pin);

//! Add Pin

/*!
   \param A pin name.
   \brief Adds a Pin instance, if the pin already exist then just return the existing pin.
   \return A handler for the created/existing Pin.
 */
            Pin add(Pin, std::string pinName);

//! Erase Pin

/*!
   \param pin A handler for the Pin to erase.
   \brief Erases a Pin instance.
 */
            void erase(const Pin & pin);

//! Size of Pin's System

/*!
   \brief Returns the number of Pins.
   \return The number of Pins.
 */
            uint32_t size(Pin) const;

//! Iterator to beginning

/*!
   \brief Returns an iterator pointing to the first element in the Pin's EntitySystem.
   \return Iterator to the first element in the Pin's EntitySystem.
 */
            entity_system::EntitySystem <Pin>::const_iterator begin(Pin) const;

//! Iterator to end

/*!
   \brief Returns an iterator referring to the past-the-end element in the Pin's EntitySystem.
   \return Iterator referring to the past-the-end element in the Pin's EntitySystem.
 */
            entity_system::EntitySystem <Pin>::const_iterator end(Pin) const;

//! Make Pin Property

/*!
   \brief Creates a Property for the Pin's Entity System.
   \tparam Value value type of the Property.
   \return An Pin => \p Value Map.
 */
            template <typename Value>
            entity_system::Property <Pin, Value> makeProperty(Pin) const
            {
                return entity_system::Property <Pin, Value>(mPins);
            }

//! Get the Pin Notifier

/*!
   \brief Returns a pointer to the AlterationNotifier of the Pin's EntitySystem.
   \return A pointer to the AlterationNotifier of the Pin's EntitySystem.
 */
            entity_system::EntitySystem <Pin>::NotifierType * notifier(Pin) const;

//! Allocate space for storing Pin entities

/*!
   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions pins), then it is worth reserving space for this amount before starting to build the netlist.
   \param size Minimum capacity for the Pin container.
 */
            void reserve(Pin, uint32_t size);

//! Capacity of the Pin's System

/*!
   \return The capacity of the Pin EntitySystem.
 */
            uint32_t capacity(Pin) const;

//! Find a pin

/*!
   \brief Using the mapping, return a pin handler by pin's name.
   \param The pin name.
   \return Return a pin handler by pin's name.
 */
            Pin find(Pin, std::string pinName);

//! Const find a pin
/*!
   \brief Using the mapping, return a pin handler by pin's name, const.
   \param The pin name.
   \return Return a pin handler by pin's name.
 */
            Pin find(Pin, std::string pinName) const;

//! Returns the name of the pin

/*!
   \brief Returns the name of the pin.
   \param A handler for the pin.
   \return Return the pin's name.
 */
            std::string name(const Pin & pin) const;

//! Net of a Pin

/*!
   \brief Returns the Net of a given Pin.
   \param pin A handler for the Pin we want to get the Net.
   \return A handler for the Net of \p pin.
   \remark If \p pin is disconnected, returns Net().
 */
            Net net(const Pin & pin) const;

//! Disconnect Pin

/*!
   \brief Disconnects a pin from its net.
   \param pin A handler for the Pin we want to disconnect.
 */
            void disconnect(const Pin & pin);

//! Cell of a Pin

/*!
   \brief Returns the Cell of a Pin.
   \param pin A handler for the Pin we want to get the Cell.
   \remark If \p pin doesn't have a Cell, returns Cell().
 */
            Cell cell(const Pin & pin) const;

//! Add Net

/*!
   \param A net name.
   \brief Adds a Net instance, if the net already exist then just return the existing net.
   \return A handler for the created/existing Net.
 */
            Net add(Net, std::string netName);

//! Erase Net

/*!
   \param net A handler for the Net to erase.
   \brief Erases a Net instance.
 */
            void erase(const Net & net);

//! Size of Net's System

/*!
   \brief Returns the number of Nets.
   \return The number of Nets.
 */
            uint32_t size(Net) const;

//! Iterator to beginning

/*!
   \brief Returns an iterator pointing to the first element in the Net's EntitySystem.
   \return Iterator to the first element in the Net's EntitySystem.
 */
            entity_system::EntitySystem <Net>::const_iterator begin(Net) const;

//! Iterator to end

/*!
   \brief Returns an iterator referring to the past-the-end element in the Net's EntitySystem.
   \return Iterator referring to the past-the-end element in the Net's EntitySystem.
 */
            entity_system::EntitySystem <Net>::const_iterator end(Net) const;

//! Make Net Property

/*!
   \brief Creates a Property for the Net's Entity System.
   \tparam Value value type of the Property.
   \return An Net => \p Value Map.
 */
            template <typename Value>
            entity_system::Property <Net, Value> makeProperty(Net) const
            {
                return entity_system::Property <Net, Value>(mNets);
            }

//! Get the Net Notifier

/*!
   \brief Returns a pointer to the AlterationNotifier of the Net's EntitySystem.
   \return A pointer to the AlterationNotifier of the Net's EntitySystem.
 */
            entity_system::EntitySystem <Net>::NotifierType * notifier(Net) const;

//! Allocate space for storing Net entities

/*!
   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions nets), then it is worth reserving space for this amount before starting to build the netlist.
   \param size Minimum capacity for the Net container.
 */
            void reserve(Net, uint32_t size);

//! Capacity of the Net's System

/*!
   \return The capacity of the Net EntitySystem.
 */
            uint32_t capacity(Net) const;

//! Find a net

/*!
   \brief Using the mapping, return a net handler by net's name.
   \param The net name.
   \return Return a net handler by net's name.
 */
            Net find(Net, std::string netName);

//! Returns the name of the net

/*!
   \brief Returns the name of the net.
   \param A handler for the net.
   \return Return the net's name.
 */
            std::string name(const Net & net) const;

//! Pins of a Net

/*!
   \brief Returns a Container Wrapper for the Pins of a Net.
   \param net A handler for the Net we want to get the Pins.
   \return Container Wrapper for the Pins of a Net.
 */
            entity_system::Association <Net, Pin>::Parts pins(const Net & net) const;

//! Connect Pin on Net

/*!
   \brief Connects a Pin
   \param net A handler for the Net we want to connect \p pin.
   \param pin A handler for the Pin we want to connect.
 */
            void connect(const Net & net, const Pin & pin);

            //! Number of Inputs

            /*!
               \brief Returns the number of Inputs.
               \return The number of Inputs.
             */
            uint32_t size(Input) const;

            //! Create an Input

            /*!
               \brief Creates an Input for a given Pin.
               \param pin The Pin we want to create an Input.
               \return A handler for the created Input.
             */
            Input add(Input, const Pin &pin);

            //! Pin of an Input

            /*!
               \brief Returns the Pin of a given Input.
               \param input the Input we want the Pin.
               \return A handler for the Pin of \p input.
             */
            Pin pin(const Input & input) const;

            //! Input of a Pin

            /*!
               \brief Returns the Input of a given Pin.
               \param pin the Pin we want the Input.
               \return A handler for the Input of \p pin.
               \remark If \p pin isn't associated with any Input, returns Input().
             */
            Input input(const Pin & pin) const;

            //! Iterator to beginning

            /*!
               \brief Returns an iterator pointing to the first element in the Input's EntitySystem.
               \return Iterator to the first element in the Input's EntitySystem.
             */
            entity_system::EntitySystem <Input>::const_iterator begin(Input) const;

            //! Iterator to end

            /*!
               \brief Returns an iterator referring to the past-the-end element in the Input's EntitySystem.
               \return Iterator referring to the past-the-end element in the Input's EntitySystem.
             */
            entity_system::EntitySystem <Input>::const_iterator end(Input) const;

            //! Make Input Property

            /*!
               \brief Creates a Property for the Input's Entity System.
               \tparam Value value type of the Property.
               \return An Input => \p Value Map.
             */
            template <typename Value>
            entity_system::Property <Input, Value> makeProperty(Input) const
            {
                return entity_system::Property <Input, Value>(mInputs);
            }

            //! Get the Input Notifier

            /*!
               \brief Returns a pointer to the AlterationNotifier of the Input's Entity System.
               \return A pointer to the AlterationNotifier of the Input's Entity System.
             */
            entity_system::EntitySystem <Input>::NotifierType * notifier(Input) const;

            //! Number of Outputs

            /*!
               \brief Returns the number of Outputs.
               \return The number of Outputs.
             */
            uint32_t size(Output) const;

            //! Create an Output

            /*!
               \brief Creates an Output for a given Pin.
               \param pin The Pin we want to create an Output.
               \return A handler for the created Output.
             */
            Output add(Output, const Pin &pin);

            //! Pin of an Output

            /*!
               \brief Returns the Pin of a given Output.
               \param output the Output we want the Pin.
               \return A handler for the Pin of \p output.
             */
            Pin pin(const Output & output) const;

            //! Output of a Pin

            /*!
               \brief Returns the Output of a given Pin
               \param pin the Pin we want the Output.
               \return A handler for the Output of \p pin.
               \remark If pin isn't associated with any Output, returns Output().
             */
            Output output(const Pin & pin) const;

            //! Iterator to beginning

            /*!
               \brief Returns an iterator pointing to the first element in the Output's Entity System.
               \return Iterator to the first element in the Output's Entity System.
             */
            entity_system::EntitySystem <Output>::const_iterator begin(Output) const;

            //! Iterator to end

            /*!
               \brief Returns an iterator referring to the past-the-end element in the Output's Entity System.
               \return Iterator referring to the past-the-end element in the Output's Entity System.
             */
            entity_system::EntitySystem <Output>::const_iterator end(Output) const;

            //! Make Output Property

            /*!
               \brief Creates a Property for the Output's Entity System.
               \tparam Value value type of the Property.
               \return An Output => \p Value Map.
             */
            template <typename Value>
            entity_system::Property <Output, Value> makeProperty(Output) const
            {
                return entity_system::Property <Output, Value>(mOutputs);
            }

            //! Get the Output Notifier

            /*!
               \brief Returns a pointer to the AlterationNotifier of the Output's Entity System.
               \return A pointer to the AlterationNotifier of the Output's Entity System.
             */
            entity_system::EntitySystem <Output>::NotifierType * notifier(Output) const;

            //! Shrink Netlist

            /*!
               \brief Shrink each EntitySystem in order to improve the memory usage.
             */
            void shrinkToFit();

        private:

            Netlist(const Netlist & nl) = delete;

            Netlist & operator =(const Netlist & nl) = delete;

            entity_system::EntitySystem <Cell>          mCells;
            entity_system::EntitySystem <Pin>           mPins;
            entity_system::EntitySystem <Net>           mNets;
            entity_system::EntitySystem <Input>         mInputs;
            entity_system::EntitySystem <Output>        mOutputs;
            entity_system::Property <Cell, std::string> mCellNames;
            entity_system::Property <Pin, std::string>  mPinNames;
            entity_system::Property <Net, std::string>  mNetNames;
            std::unordered_map <std::string, Cell>      mName2Cell;
            std::unordered_map <std::string, Pin>       mName2Pin;
            std::unordered_map <std::string, Net>       mName2Net;
            entity_system::Aggregation <Net, Pin>       mNetPins;
            entity_system::Composition <Cell, Pin>      mCellPins;
            entity_system::Composition <Pin, Input>     mPinInput;
            entity_system::Composition <Pin, Output>    mPinOutput;
        };
    }     // namespace circuit
}     // namespace ophidian

#endif // OPHIDIAN_CIRCUIT_NETLIST_H