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

	Cell add(Cell);

	void erase(const Cell & cell);

	void reserve(Cell, uint32_t size);

	uint32_t size(Cell) const;

	uint32_t capacity(Cell) const;

	ophidian::util::Range<StandardCells::CellsIterator> range(Cell) const;

	//! Name setter
	/*!
	   \brief Set the name of a given cell
	   \param cell Cell entity to set the name.
	   \param name Name of the cell.
	 */
	void name(const Cell & cell, const std::string & name);

	std::string name(const Cell & cell) const;

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

	Pin add(Pin);

	void erase(const Pin & pin);

	void reserve(Pin, uint32_t size);

	uint32_t size(Pin) const;

	uint32_t capacity(Pin) const;

	void name(const Pin & pin, const std::string & name);

	std::string name(const Pin & pin) const;

	void direction(const Pin & pin, const PinDirection & direction);

	PinDirection direction(const Pin & pin);

	void owner(const Pin & pin, const Cell & cell);

	Cell owner(const Pin & pin);

	ophidian::util::Range<StandardCells::PinsIterator> range(Pin) const;

	//! Make Cell Property
	/*!
	   \brief Creates a Property for the Standard Cell's Entity System.
	   \tparam Value value type of the Property.
	   \return An Cell => \p Value Map.
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
	entity_system::Property<Pin, Cell> pinOwners_;
	entity_system::Property<Pin, Cell> pinDirections_;

	//composition and aggregation relations
	entity_system::Composition<Cell, Pin> cellPins_;
};

} //namespace ophidian

} //namespace standard_cell

#endif // OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
