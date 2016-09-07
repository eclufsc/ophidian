#ifndef NETLIST_H
#define NETLIST_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/entity_system/Composition.h>

namespace ophidian
{
    namespace circuit
    {

        class Cell : public entity_system::EntityBase
        {
            public:
                using entity_system::EntityBase::EntityBase;
        };

        class Pin : public entity_system::EntityBase
        {
            public:
                using entity_system::EntityBase::EntityBase;
        };

        class Net : public entity_system::EntityBase
        {
            public:
                using entity_system::EntityBase::EntityBase;
        };

        class Input : public entity_system::EntityBase
        {
            public:
                using entity_system::EntityBase::EntityBase;
        };

        class Output : public entity_system::EntityBase
        {
            public:
                using entity_system::EntityBase::EntityBase;
        };

        template <class Entity_>
        class Manager
        {
            public:
                virtual ~Manager() { }
                virtual Entity_ add(Entity_) = 0;
                virtual void erase(const Entity_ & en) = 0;
                virtual uint32_t size(Entity_) const = 0;
        };


        /*! A flatten Netlist */
        class Netlist final : public Manager<Cell>,
            public Manager<Pin>,
            public Manager<Net>
        {
            public:
                using CellNotifier = entity_system::EntitySystem<Cell>::NotifierType;
                using PinNotifier = entity_system::EntitySystem<Pin>::NotifierType;
                using NetNotifier = entity_system::EntitySystem<Net>::NotifierType;

                //! Construct Netlist
                /*!
                  Constructs an empty Netlist, with no Cells, Pins or Nets.
                */
                Netlist();

                Netlist(Netlist&& nl);
                //! Netlist Destructor
                /*!
                  This destroy all Netlist's EntitySystems.
                */
                ~Netlist() override;

                //! Add Cell
                /*!
                  \brief Adds a Cell instance.
                  \return A handler for the created Cell.
                */
                Cell add(Cell) override;  
                //! Erase Cell
                /*!
                  \param cell A handler for the Cell to erase.
                  \brief Erases a Cell instance.
                */
                void erase(const Cell& cell) override;
                //! Size of Cell's System
                /*!
                  \brief Returns the number of Cells.
                  \return The number of Cells.
                */
                uint32_t size(Cell) const override;
                //! Iterator to beginning
                /*!
                  \brief Returns an iterator pointing to the first element in the Cell's EntitySystem.
                  \return Iterator to the first element in the Cell's EntitySystem.
                */
                entity_system::EntitySystem<Cell>::const_iterator begin(Cell) const;
                //! Iterator to end
                /*!
                  \brief Returns an iterator referring to the past-the-end element in the Cell's EntitySystem.
                  \return Iterator referring to the past-the-end element in the Cell's EntitySystem.
                */
                entity_system::EntitySystem<Cell>::const_iterator end(Cell) const;
                //! Make Cell Property
                /*!
                  \brief Creates a Property for the Cell's Entity System.
                  \tparam Value value type of the Property.
                  \return An Cell => \p Value Map.
                */
                template <typename Value>
                entity_system::Property<Cell, Value> makeProperty(Cell)
                {
                    return entity_system::Property<Cell, Value>(cells_);
                }
                //! Get the Cell Notifier
                /*!
                  \brief Returns a pointer to the AlterationNotifier of the Cell's EntitySystem.
                  \return A pointer to the AlterationNotifier of the Cell's EntitySystem.
                */
                entity_system::EntitySystem<Cell>::NotifierType* notifier(Cell) const;
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
                //! Pins of a Cell
                /*!
                  \brief Returns a Container Wrapper for the Pins of a Cell.
                  \param cell A handler for the Cell we want to get the Pins.
                  \return Container Wrapper for the Pins of a Cell.
                */
                entity_system::Association<Cell, Pin>::Parts pins(const Cell& cell) const;
                //! Add Pin into Cell
                /*!
                  \brief Adds a Pin to a given Cell.
                  \param cell A handler for the Cell we want to add a Pin.
                  \param pin A handler for the Pin we want to add in \p cell.
                */
                void add(const Cell& cell, const Pin& pin);
                //! Add Pin
                /*!
                  \brief Adds a Pin instance.
                  \return A handler for the created Pin.
                */
                Pin add(Pin) override;
                //! Erase Pin
                /*!
                  \param pin A handler for the Pin to erase.
                  \brief Erases a Pin instance.
                */
                void erase(const Pin& pin) override;
                //! Size of Pin's System
                /*!
                  \brief Returns the number of Pins.
                  \return The number of Pins.
                */
                uint32_t size(Pin) const override;
                //! Iterator to beginning
                /*!
                  \brief Returns an iterator pointing to the first element in the Pin's EntitySystem.
                  \return Iterator to the first element in the Pin's EntitySystem.
                */
                entity_system::EntitySystem<Pin>::const_iterator begin(Pin) const;
                //! Iterator to end
                /*!
                  \brief Returns an iterator referring to the past-the-end element in the Pin's EntitySystem.
                  \return Iterator referring to the past-the-end element in the Pin's EntitySystem.
                */
                entity_system::EntitySystem<Pin>::const_iterator end(Pin) const;
                //! Make Pin Property
                /*!
                  \brief Creates a Property for the Pin's Entity System.
                  \tparam Value value type of the Property.
                  \return An Pin => \p Value Map.
                */
                template <typename Value>
                entity_system::Property<Pin, Value> makeProperty(Pin)
                {
                    return entity_system::Property<Pin, Value>(pins_);
                }
                //! Get the Pin Notifier
                /*!
                  \brief Returns a pointer to the AlterationNotifier of the Pin's EntitySystem.
                  \return A pointer to the AlterationNotifier of the Pin's EntitySystem.
                */
                entity_system::EntitySystem<Pin>::NotifierType* notifier(Pin) const;
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
                //! Net of a Pin
                /*!
                  \brief Returns the Net of a given Pin.
                  \param pin A handler for the Pin we want to get the Net.
                  \return A handler for the Net of \p pin.
                  \remark If \p pin is disconnected, returns Net().
                */
                Net net(const Pin& pin) const;
                //! Disconnect Pin
                /*!
                  \brief Disconnects a pin from its net.
                  \param pin A handler for the Pin we want to disconnect.
                */
                void disconnect(const Pin& pin);
                //! Cell of a Pin
                /*!
                  \brief Returns the Cell of a Pin.
                  \param pin A handler for the Pin we want to get the Cell.
                  \remark If \p pin doesn't have a Cell, returns Cell().
                */
                Cell cell(const Pin& pin) const;

                //! Add Net
                /*!
                  \brief Adds a Net instance.
                  \return A handler for the created Net.
                */
                Net add(Net) override;
                //! Erase Net
                /*!
                  \param net A handler for the Net to erase.
                  \brief Erases a Net instance.
                */
                void erase(const Net& net) override;
                //! Size of Net's System
                /*!
                  \brief Returns the number of Nets.
                  \return The number of Nets.
                */
                uint32_t size(Net) const override;
                //! Iterator to beginning
                /*!
                  \brief Returns an iterator pointing to the first element in the Net's EntitySystem.
                  \return Iterator to the first element in the Net's EntitySystem.
                */
                entity_system::EntitySystem<Net>::const_iterator begin(Net) const;
                //! Iterator to end
                /*!
                  \brief Returns an iterator referring to the past-the-end element in the Net's EntitySystem.
                  \return Iterator referring to the past-the-end element in the Net's EntitySystem.
                */
                entity_system::EntitySystem<Net>::const_iterator end(Net) const;
                //! Make Net Property
                /*!
                  \brief Creates a Property for the Net's Entity System.
                  \tparam Value value type of the Property.
                  \return An Net => \p Value Map.
                */
                template <typename Value>
                entity_system::Property<Net, Value> makeProperty(Net)
                {
                    return entity_system::Property<Net, Value>(nets_);
                }
                //! Get the Net Notifier
                /*!
                  \brief Returns a pointer to the AlterationNotifier of the Net's EntitySystem.
                  \return A pointer to the AlterationNotifier of the Net's EntitySystem.
                */
                entity_system::EntitySystem<Net>::NotifierType* notifier(Net) const;
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
                //! Pins of a Net
                /*!
                  \brief Returns a Container Wrapper for the Pins of a Net.
                  \param net A handler for the Net we want to get the Pins.
                  \return Container Wrapper for the Pins of a Net.
                */
                entity_system::Association<Net, Pin>::Parts pins(const Net& net) const;
                //! Connect Pin on Net
                /*!
                  \brief Connects a Pin
                  \param net A handler for the Net we want to connect \p pin.
                  \param pin A handler for the Pin we want to connect.
                */
                void connect(const Net& net, const Pin& pin);

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
                Input add(Input, const Pin& pin);
                //! Pin of an Input
                /*!
                  \brief Returns the Pin of a given Input.
                  \param input the Input we want the Pin.
                  \return A handler for the Pin of \p input.
                */
                Pin pin(const Input& input) const;
                //! Input of a Pin
                /*!
                  \brief Returns the Input of a given Pin.
                  \param pin the Pin we want the Input.
                  \return A handler for the Input of \p pin.
                  \remark If \p pin isn't associated with any Input, returns Input().
                */
                Input input(const Pin& pin) const;
                //! Iterator to beginning
                /*!
                  \brief Returns an iterator pointing to the first element in the Input's EntitySystem.
                  \return Iterator to the first element in the Input's EntitySystem.
                */
                entity_system::EntitySystem<Input>::const_iterator begin(Input) const;
                //! Iterator to end
                /*!
                  \brief Returns an iterator referring to the past-the-end element in the Input's EntitySystem.
                  \return Iterator referring to the past-the-end element in the Input's EntitySystem.
                */
                entity_system::EntitySystem<Input>::const_iterator end(Input) const;
                //! Make Input Property
                /*!
                  \brief Creates a Property for the Input's Entity System.
                  \tparam Value value type of the Property.
                  \return An Input => \p Value Map.
                */
                template <typename Value>
                entity_system::Property<Input, Value> makeProperty(Input)
                {
                    return entity_system::Property<Input, Value>(inputs_);
                }
                //! Get the Input Notifier
                /*!
                  \brief Returns a pointer to the AlterationNotifier of the Input's Entity System.
                  \return A pointer to the AlterationNotifier of the Input's Entity System.
                */
                entity_system::EntitySystem<Input>::NotifierType* notifier(Input) const;

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
                Output add(Output, const Pin& pin);
                //! Pin of an Output
                /*!
                  \brief Returns the Pin of a given Output.
                  \param output the Output we want the Pin.
                  \return A handler for the Pin of \p output.
                */
                Pin pin(const Output& output) const;
                //! Output of a Pin
                /*!
                  \brief Returns the Output of a given Pin
                  \param pin the Pin we want the Output.
                  \return A handler for the Output of \p pin.
                  \remark If pin isn't associated with any Output, returns Output().
                */
                Output output(const Pin& pin) const;
                //! Iterator to beginning
                /*!
                  \brief Returns an iterator pointing to the first element in the Output's Entity System.
                  \return Iterator to the first element in the Output's Entity System.
                */
                entity_system::EntitySystem<Output>::const_iterator begin(Output) const;
                //! Iterator to end
                /*!
                  \brief Returns an iterator referring to the past-the-end element in the Output's Entity System.
                  \return Iterator referring to the past-the-end element in the Output's Entity System.
                */
                entity_system::EntitySystem<Output>::const_iterator end(Output) const;
                //! Make Output Property
                /*!
                  \brief Creates a Property for the Output's Entity System.
                  \tparam Value value type of the Property.
                  \return An Output => \p Value Map.
                */
                template <typename Value>
                entity_system::Property<Output, Value> makeProperty(Output)
                {
                    return entity_system::Property<Output, Value>(outputs_);
                }
                //! Get the Output Notifier
                /*!
                  \brief Returns a pointer to the AlterationNotifier of the Output's Entity System.
                  \return A pointer to the AlterationNotifier of the Output's Entity System.
                */
                entity_system::EntitySystem<Output>::NotifierType* notifier(Output) const;

                //! Shrink Netlist
                /*!
                  \brief Shrink each EntitySystem in order to improve the memory usage.
                */
                void shrink();
            private:
                Netlist(const Netlist& nl) = delete;
                Netlist& operator =(const Netlist& nl) = delete;
                entity_system::EntitySystem<Cell> cells_;
                entity_system::EntitySystem<Pin> pins_;
                entity_system::EntitySystem<Net> nets_;
                entity_system::EntitySystem<Input> inputs_;
                entity_system::EntitySystem<Output> outputs_;
                entity_system::Aggregation<Net, Pin> netPins_;
                entity_system::Composition<Cell, Pin> cellPins_;
                entity_system::Composition<Pin, Input> pinInput_;
                entity_system::Composition<Pin, Output> pinOutput_;
        };

    }
}

#endif // NETLIST_H
