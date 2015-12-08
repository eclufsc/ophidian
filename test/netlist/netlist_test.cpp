#include "../catch.hpp"
#include <netlist_bgl.h>

using namespace openeda::netlist;

class insert_cell_fixture {

	cell_identifier m_id;
protected:
	netlist_interface* m_netlist;
public:
	insert_cell_fixture() :
			m_netlist(0) {
		reset_fixture();
	}
	virtual ~insert_cell_fixture(){
		delete m_netlist;
	}
	cell_identifier id() {
		return m_id;
	}
	void reset_fixture() {
		if(m_netlist)
			delete m_netlist;
		m_netlist = new netlist_bgl;
		m_id = m_netlist->insert_cell("u1", "NAND2");
	}
};

TEST_CASE("empty boost netlist","[netlist][bgl]") {

	netlist_bgl netlist;
	REQUIRE(netlist.number_of_cells() == 0);
	REQUIRE(netlist.number_of_pins() == 0);
	REQUIRE(netlist.number_of_nets() == 0);

}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist insert cell","[netlist][bgl]") {
	REQUIRE( m_netlist->cell_name(id()) == "u1" );
	REQUIRE( m_netlist->cell_type(id()) == "NAND2" );
	reset_fixture();

}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist erase cell","[netlist][bgl]") {
	REQUIRE( !m_netlist->erase_cell(-1) );
	REQUIRE( m_netlist->erase_cell(id()) );
	REQUIRE( m_netlist->number_of_cells() == 0 );
	REQUIRE( !m_netlist->erase_cell(id()) );
	reset_fixture();
}
