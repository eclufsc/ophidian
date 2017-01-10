#ifndef OPHIDIAN_PLACEMENT_PLACEMENT_H
#define OPHIDIAN_PLACEMENT_PLACEMENT_H

namespace ophidian {
namespace placement {

class Placement
{
public:
	//! Placement Constructor
	/*!
	   \brief Constructs a placement system with no properties
	 */
	Placement();

	//! Floorplan Destructor
	/*!
	   \brief Destroys the placement system, including its properties.
	 */
	~Placement();
};

} //namespace placement

} //namespace ophidian

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
