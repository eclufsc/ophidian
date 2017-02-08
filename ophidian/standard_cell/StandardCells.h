#ifndef OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
#define OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H

#include <memory>
#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Composition.h>
#include <ophidian/util/Range.h>

namespace ophidian {
namespace standard_cell {

class Pin : public entity_system::EntityBase
{
public:
	using entity_system::EntityBase::EntityBase;
};

class Cell : public entity_system::EntityBase
{
public:
	using entity_system::EntityBase::EntityBase;
};

enum class PinDirection {
    NOT_ASSIGNED, INPUT, OUTPUT, INOUT
};

class StandardCells
{
public:
	using CellsIterator = entity_system::EntitySystem<Cell>::const_iterator;
	using PinsIterator = entity_system::EntitySystem<Pin>::const_iterator;

	//! StandardCell Constructor
	/*!
	   \brief Constructs an empty system with no Cells and Pins.
	 */
	StandardCells();

	//! StandardCell Copy Constructor
	/*!
	    \brief Copy the entity system and its properties.
	 */
	StandardCells(StandardCells && stdCell);

	//! StandardCell Destructor
	/*!
	   \brief Destroys the Cells and Pins EntitySystem, including its properties.
	 */
	~StandardCells();

//--------------------------- Cells -------------------------------//

    //! Add Cell
    /*!
      \brief Adds a cell instance. A cell has a name associated to it.
      \param name Name of the cell, used to identify it.
      \return A handler for the created Cell.
    */
    Cell add(Cell, std::__cxx11::string name);

    //! Erase Cell
    /*!
      \param cell A handler for the Cell to erase.
      \brief Erases a Cell instance.
    */
	void erase(const Cell & cell);

    //! Allocate space for storing Cell entities
    /*!
      \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions cells), then it is worth reserving space for this amount before starting to build the standard cells.
      \param size Minimum capacity for the cell container.
    */
	void reserve(Cell, uint32_t size);

    //! Size of Cell's System
    /*!
      \brief Returns the number of Cells.
      \return The number of Cells.
    */
	uint32_t size(Cell) const;

    //! Capacity of the Cell's System
    /*!
      \return The capacity of the Cell EntitySystem.
    */
	uint32_t capacity(Cell) const;

    //! Cells iterator
    /*!
       \return Range iterator for the cells.
     */
	ophidian::util::Range<StandardCells::CellsIterator> range(Cell) const;

    //! Cell name getter
    /*!
       \brief Get the name of a given cell.
       \param cell Cell entity to get the name.
       \return Name of the cell
     */
	std::string name(const Cell & cell) const;

    //! Pins of a Cell
    /*!
      \brief Returns a Container Wrapper for the Pins of a Cell.
      \param cell A handler for the Cell we want to get the Pins.
      \return Container Wrapper for the Pins of a Cell.
    */
    entity_system::Association<Cell, Pin>::Parts pins(const Cell& cell) const;

	//! Make Cell Property
	/*!
	   \brief Creates a Property for the Standard Cell's Entity System.
	   \tparam Value value type of the Property.
	   \return An Cell => \p Value Map.
	 */
	template <typename Value>
	entity_system::Property<Cell, Value> makeProperty(Cell)
	{
		return entity_system::Property<Cell, Value>(cells_);
	}

//--------------------------- Pins -------------------------------//

    //! Add Pin
    /*!
      \brief Adds a pin instance. A pin has a name and a direction associated to it.
      \param name Name of the pin, used to identify it.
      \return A handler for the created pin.
    */
    Pin add(Pin, std::__cxx11::string name, PinDirection direction);

    //! Erase Pin
    /*!
      \param pin A handler for the pin to erase.
      \brief Erases a Pin instance.
    */
	void erase(const Pin & pin);

    //! Allocate space for storing pin entities
    /*!
      \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions pins), then it is worth reserving space for this amount before starting to build the standard cells.
      \param size Minimum capacity for the Pin container.
    */
	void reserve(Pin, uint32_t size);

    //! Size of Pin's System
    /*!
      \brief Returns the number of pins.
      \return The number of pins.
    */
	uint32_t size(Pin) const;

    //! Capacity of the Pin's System
    /*!
      \return The capacity of the Pin EntitySystem.
    */
	uint32_t capacity(Pin) const;

    //! Pin name getter
    /*!
       \brief Get the name of a given pin.
       \param pin Pin entity to get the name.
       \return Name of the pin
     */
	std::string name(const Pin & pin) const;

    //! Pin direction getter
    /*!
       \brief Get the direction of a given pin. A direction can be NOT_ASSIGNED, INPUT, OUTPUT or INOUT.
       \param pin Pin entity to get the direction.
       \return Direction of the pin
     */
	PinDirection direction(const Pin & pin);

    //! Pin owner getter
    /*!
       \brief Get the owner of a given pin.
       \param pin Pin entity to get the owner.
       \return Owner of the pin
     */
	Cell owner(const Pin & pin);

    //! Pins iterator
    /*!
       \return Range iterator for the Pins.
     */
	ophidian::util::Range<StandardCells::PinsIterator> range(Pin) const;

    //! Make Pin Property
	/*!
	   \brief Creates a Property for the Standard Cell's Entity System.
	   \tparam Value value type of the Property.
       \return An Pin => \p Value Map.
	 */
	template <typename Value>
    entity_system::Property<Pin, Value> makeProperty(Pin)
	{
		return entity_system::Property<Pin, Value>(pins_);
	}

	//--------------------------- Association -------------------------------//

	//! Add Pin into Cell
	/*!
	   \brief Adds a Pin to a given Cell.
	   \param cell A handler for the Cell we want to add a Pin.
	   \param pin A handler for the Pin we want to add in \p cell.
	 */
	//Maybe rename to create_association or associate...
	void add(const Cell& cell, const Pin& pin);

private:
	//cells entity system and properties
	entity_system::EntitySystem<Cell> cells_;
	entity_system::Property<Cell, std::string> cellNames_;

	//pins entity system and properties
	entity_system::EntitySystem<Pin> pins_;
	entity_system::Property<Pin, std::string> pinNames_;
    entity_system::Property<Pin, PinDirection> pinDirections_;

	//composition and aggregation relations
	entity_system::Composition<Cell, Pin> cellPins_;
};

} //namespace ophidian

} //namespace standard_cell

#endif // OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
