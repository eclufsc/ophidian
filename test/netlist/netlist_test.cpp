#include "../catch.hpp"
#include <netlist_bgl.h>

using namespace openeda::netlist;

class insert_cell_fixture {
	cell_identifier m_id;
protected:
	netlist_bgl* m_netlist;
public:
	insert_cell_fixture() :
			m_netlist(0) {
		reset_fixture();
	}
	virtual ~insert_cell_fixture() {
		delete m_netlist;
	}
	cell_identifier id() {
		return m_id;
	}
	void reset_fixture() {
		if (m_netlist)
			delete m_netlist;
		m_netlist = new netlist_bgl;
		m_id = m_netlist->insert_cell("u1", "NAND2");
	}
};

TEST_CASE("boost netlist/empty","[netlist][bgl]") {

	netlist_bgl netlist;
	REQUIRE(netlist.number_of_cells() == 0);
	REQUIRE(netlist.number_of_pins() == 0);
	REQUIRE(netlist.number_of_nets() == 0);
	REQUIRE(netlist.pads(direction_enum::IN).empty());
	REQUIRE(netlist.pads(direction_enum::OUT).empty());
	REQUIRE(netlist.pads(direction_enum::INOUT).empty());

}

TEST_CASE("boost netlist/insert three remove 1","[netlist][bgl]") {

	netlist_bgl netlist;
	auto u1 = netlist.insert_cell("u1", "NAND");
	auto u2 = netlist.insert_cell("u2", "NAND");
	auto u3 = netlist.insert_cell("u3", "NAND");

	netlist.erase_cell(u2);

	REQUIRE(netlist.insert_cell("u1", "NAND") == u1);
	REQUIRE(netlist.insert_cell("u3", "NAND") == u3);
	REQUIRE(netlist.number_of_cells() == 2);

}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist/insert cell","[netlist][bgl]") {
	REQUIRE(m_netlist->cell_name(id()) == "u1");
	REQUIRE(m_netlist->cell_type(id()) == "NAND2");
	REQUIRE(m_netlist->cell_pins(id()).empty());
	reset_fixture();
}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist/can not insert a cell twice","[netlist][bgl]") {
	REQUIRE(m_netlist->insert_cell("u1", "NAND2") == id());
	REQUIRE(m_netlist->number_of_cells() == 1);
	reset_fixture();
}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist/erase cell","[netlist][bgl]") {
	REQUIRE_THROWS(m_netlist->erase_cell(-1));
	REQUIRE_NOTHROW(m_netlist->erase_cell(id()));
	REQUIRE(m_netlist->number_of_cells() == 0);
	REQUIRE_THROWS(m_netlist->erase_cell(id()));
	reset_fixture();
}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist/insert pin into cell","[netlist][bgl]") {
	auto pin_id = m_netlist->insert_pin("u1:a", id());
	REQUIRE(m_netlist->number_of_pins() == 1);
	REQUIRE(m_netlist->cell_pins(id()).size() == 1);
	REQUIRE(m_netlist->cell_pins(id()).front() == pin_id);
	REQUIRE(m_netlist->pin_name(pin_id) == "u1:a");
	REQUIRE(m_netlist->pin_owner(pin_id) == id());
	reset_fixture();
}

TEST_CASE_METHOD(insert_cell_fixture, "boost netlist/can not insert a pin twice","[netlist][bgl]") {
	auto pin_id = m_netlist->insert_pin("u1:a", id());
	REQUIRE(m_netlist->insert_pin("u1:a", id()) == pin_id);
	REQUIRE(m_netlist->number_of_pins() == 1);
	REQUIRE(m_netlist->cell_pins(id()).size() == 1);
	REQUIRE(m_netlist->cell_pins(id()).front() == pin_id);
	REQUIRE(m_netlist->pin_name(pin_id) == "u1:a");
	REQUIRE(m_netlist->pin_owner(pin_id) == id());
	reset_fixture();
}

class insert_net_fixture {
	cell_identifier m_id;
protected:
	netlist_bgl* m_netlist;
public:
	insert_net_fixture() :
			m_netlist(0) {
		reset_fixture();
	}
	virtual ~insert_net_fixture() {
		delete m_netlist;
	}
	cell_identifier id() {
		return m_id;
	}
	void reset_fixture() {
		if (m_netlist)
			delete m_netlist;
		m_netlist = new netlist_bgl;
		m_id = m_netlist->insert_net("n1");
	}
};

TEST_CASE_METHOD(insert_net_fixture, "boost netlist/insert net","[netlist][bgl]") {
	REQUIRE(m_netlist->net_name(id()) == "n1");
	REQUIRE(m_netlist->number_of_nets() == 1);
	REQUIRE(m_netlist->net_pins(id()).empty());
	reset_fixture();
}

TEST_CASE_METHOD(insert_net_fixture, "boost netlist/can not insert a net twice","[netlist][bgl]") {
	REQUIRE(m_netlist->insert_net("n1") == id());
	REQUIRE(m_netlist->number_of_nets() == 1);
	reset_fixture();
}

TEST_CASE_METHOD(insert_net_fixture, "boost netlist/erase net","[netlist][bgl]") {
	REQUIRE_NOTHROW(m_netlist->erase_net(id()));
	REQUIRE(m_netlist->number_of_nets() == 0);
	REQUIRE_THROWS(m_netlist->erase_net(id()));
	REQUIRE_THROWS(m_netlist->net_name(id()));
	REQUIRE_THROWS(m_netlist->net_pins(id()).empty());
	reset_fixture();
}

TEST_CASE_METHOD(insert_net_fixture, "boost netlist/insert net after erase","[netlist][bgl]") {
	m_netlist->erase_net(id());
	auto id = m_netlist->insert_net("n2");
	REQUIRE_NOTHROW(m_netlist->net_name(id) == "n2");
	REQUIRE(m_netlist->net_name(id) == "n2");
	REQUIRE(m_netlist->number_of_nets() == 1);
	REQUIRE_NOTHROW(m_netlist->net_pins(id).empty());
	reset_fixture();
}

class insert_two_cells_with_two_pins_each_fixture {
	cell_identifier m_u1;
	cell_identifier m_u2;

	pin_identifier m_u1a;
	pin_identifier m_u1o;

	pin_identifier m_u2a;
	pin_identifier m_u2o;

	net_identifier m_n1;
protected:
	netlist_bgl* m_netlist;
public:
	insert_two_cells_with_two_pins_each_fixture() :
			m_netlist(0) {
		reset_fixture();
	}
	virtual ~insert_two_cells_with_two_pins_each_fixture() {
		delete m_netlist;
	}
	cell_identifier u1() {
		return m_u1;
	}
	cell_identifier u2() {
		return m_u2;
	}
	pin_identifier u1a() {
		return m_u1a;
	}
	pin_identifier u1o() {
		return m_u1o;
	}
	pin_identifier u2a() {
		return m_u2a;
	}
	pin_identifier u2o() {
		return m_u2o;
	}
	net_identifier n1() {
		return m_n1;
	}
	void reset_fixture() {
		if (m_netlist)
			delete m_netlist;
		m_netlist = new netlist_bgl;
		m_u1 = m_netlist->insert_cell("u1", "INV");
		m_u2 = m_netlist->insert_cell("u2", "INV");
		m_u1a = m_netlist->insert_pin("u1:a", m_u1);
		m_u1o = m_netlist->insert_pin("u1:o", m_u1);
		m_u2a = m_netlist->insert_pin("u2:a", m_u2);
		m_u2o = m_netlist->insert_pin("u2:o", m_u2);
		m_n1 = m_netlist->insert_net("n1");
	}
};

TEST_CASE_METHOD(insert_two_cells_with_two_pins_each_fixture, "boost netlist/diconnected pin", "[netlist][bgl]") {
	REQUIRE(
			m_netlist->pin_net(u1a())
					== std::numeric_limits<net_identifier>::max());
	REQUIRE(
			m_netlist->pin_net(u1o())
					== std::numeric_limits<net_identifier>::max());
	REQUIRE(
			m_netlist->pin_net(u2a())
					== std::numeric_limits<net_identifier>::max());
	REQUIRE(
			m_netlist->pin_net(u2o())
					== std::numeric_limits<net_identifier>::max());
	reset_fixture();
}

TEST_CASE_METHOD(insert_two_cells_with_two_pins_each_fixture, "boost netlist/connect", "[netlist][bgl]") {
	REQUIRE_NOTHROW(m_netlist->connect(n1(), u1o()));
	REQUIRE(m_netlist->pin_net(u1o()) == n1());
	auto pins = m_netlist->net_pins(n1());
	REQUIRE(std::find(pins.begin(), pins.end(), u1o()) != pins.end());
	REQUIRE(pins.size() == 1);
	reset_fixture();
}

TEST_CASE_METHOD(insert_two_cells_with_two_pins_each_fixture, "boost netlist/connect a connected pin", "[netlist][bgl]") {
	REQUIRE_NOTHROW(m_netlist->connect(n1(), u1o()));
	REQUIRE_THROWS(m_netlist->connect(n1(), u1o()));
	reset_fixture();
}

TEST_CASE_METHOD(insert_two_cells_with_two_pins_each_fixture, "boost netlist/disconnect", "[netlist][bgl]") {
	m_netlist->connect(n1(), u1o());
	REQUIRE_NOTHROW(m_netlist->disconnect(n1(), u1o()));
	REQUIRE(
			m_netlist->pin_net(u1o())
					== std::numeric_limits<net_identifier>::max());
	REQUIRE(m_netlist->net_pins(n1()).empty());
	reset_fixture();
}

TEST_CASE("boost netlist/insert PI pad", "[netlist][bgl]") {
	netlist_bgl netlist;
	netlist.insert_pad("inp", direction_enum::IN);
	REQUIRE(netlist.pads(direction_enum::IN).size() == 1);
	REQUIRE(netlist.pads(direction_enum::OUT).empty());
	REQUIRE(netlist.pads(direction_enum::INOUT).empty());
}

TEST_CASE("boost netlist/insert PO pad", "[netlist][bgl]") {
	netlist_bgl netlist;
	netlist.insert_pad("out", direction_enum::OUT);
	REQUIRE(netlist.pads(direction_enum::IN).empty());
	REQUIRE(netlist.pads(direction_enum::OUT).size() == 1);
	REQUIRE(netlist.pads(direction_enum::INOUT).empty());
}

TEST_CASE("boost netlist/insert PIO pad", "[netlist][bgl]") {
	netlist_bgl netlist;
	netlist.insert_pad("inout", direction_enum::INOUT);
	REQUIRE(netlist.pads(direction_enum::IN).empty());
	REQUIRE(netlist.pads(direction_enum::OUT).empty());
	REQUIRE(netlist.pads(direction_enum::INOUT).size() == 1);
}

//#include  <iostream>
//class verilog {
//public:
//	verilog(netlist_interface* n, std::string module) {
//		std::cout << "module " << module << " (" << std::endl;
//		auto PIs = n->pads(direction_enum::IN);
//		auto POs = n->pads(direction_enum::OUT);
//		std::size_t count = 0;
//		for (auto i : PIs) {
//			std::cout << n->pin_name(i);
//			count++;
//			if (count < PIs.size() + POs.size() - 1)
//				std::cout << ",";
//			std::cout << std::endl;
//		}
//		for (auto i : POs) {
//			std::cout << n->pin_name(i);
//			count++;
//			if (count < PIs.size() + POs.size() - 1)
//				std::cout << ",";
//			std::cout << std::endl;
//		}
//		std::cout << ");" << std::endl << std::endl;
//
//		std::cout << "// Start PIs" << std::endl;
//		for (auto i : PIs)
//			std::cout << "input " << n->pin_name(i) << ";" << std::endl;
//		std::cout << std::endl;
//
//		std::cout << "// Start POs" << std::endl;
//		for (auto i : POs)
//			std::cout << "output " << n->pin_name(i) << ";" << std::endl;
//		std::cout << std::endl;
//
//		std::cout << "// Start wires" << std::endl;
//		for (std::size_t i = 0; i < n->number_of_nets(); ++i)
//			std::cout << "wire " << n->net_name(i) << ";" << std::endl;
//		std::cout << std::endl;
//
//		std::cout << "// Start cells" << std::endl;
//		for (std::size_t i = 3; i < n->number_of_cells() + 3; ++i)
//		{
//			std::cout << n->cell_type(i) << " " << n->cell_name(i) << " ( ";
//
//			auto pins = n->cell_pins(i);
//			count = 0;
//			for(auto p : pins)
//			{
//				std::cout << "." << n->pin_name(p).substr(n->pin_name(p).find_first_of(':')+1) << "(" << n->net_name(n->pin_net(p)) << ")";
//				if(count < pins.size() - 1)
//					std::cout << ", ";
//				count++;
//			}
//			std::cout << " );" << std::endl;
//		}
//		std::cout << std::endl;
//		std::cout << "endmodule" << std::endl;
//
//	}
//};

TEST_CASE("boost netlist/simple", "[netlist][bgl]") {
	netlist_bgl netlist;

	netlist.connect(netlist.insert_net("inp1"),
			netlist.insert_pad("inp1", direction_enum::IN));
	netlist.connect(netlist.insert_net("inp2"),
			netlist.insert_pad("inp2", direction_enum::IN));
	netlist.connect(netlist.insert_net("iccad_clk"),
			netlist.insert_pad("iccad_clk", direction_enum::IN));

	netlist.connect(netlist.insert_net("out"),
			netlist.insert_pad("out", direction_enum::OUT));

	netlist.insert_net("n1");
	netlist.insert_net("n2");
	netlist.insert_net("n3");
	netlist.insert_net("n4");
	netlist.insert_net("inp1");
	netlist.insert_net("inp2");
	netlist.insert_net("iccad_clk");
	netlist.insert_net("out");
	netlist.insert_net("lcb1_fo");

	netlist.connect(netlist.insert_net("inp1"),
			netlist.insert_pin("u1:a", netlist.insert_cell("u1", "NAND2_X1")));
	netlist.connect(netlist.insert_net("inp2"),
			netlist.insert_pin("u1:b", netlist.insert_cell("u1", "NAND2_X1")));
	netlist.connect(netlist.insert_net("n1"),
			netlist.insert_pin("u1:o", netlist.insert_cell("u1", "NAND2_X1")));

	netlist.connect(netlist.insert_net("n1"),
			netlist.insert_pin("u2:a", netlist.insert_cell("u2", "NOR2_X1")));
	netlist.connect(netlist.insert_net("n3"),
			netlist.insert_pin("u2:b", netlist.insert_cell("u2", "NOR2_X1")));
	netlist.connect(netlist.insert_net("n2"),
			netlist.insert_pin("u2:o", netlist.insert_cell("u2", "NOR2_X1")));

	netlist.connect(netlist.insert_net("n2"),
			netlist.insert_pin("f1:d", netlist.insert_cell("f1", "DFF_X80")));
	netlist.connect(netlist.insert_net("lcb1_fo"),
			netlist.insert_pin("f1:ck", netlist.insert_cell("f1", "DFF_X80")));
	netlist.connect(netlist.insert_net("n3"),
			netlist.insert_pin("f1:q", netlist.insert_cell("f1", "DFF_X80")));

	netlist.connect(netlist.insert_net("n3"),
			netlist.insert_pin("u3:a", netlist.insert_cell("u3", "INV_X1")));
	netlist.connect(netlist.insert_net("n4"),
			netlist.insert_pin("u3:o", netlist.insert_cell("u3", "INV_X1")));

	netlist.connect(netlist.insert_net("n4"),
			netlist.insert_pin("u4:a", netlist.insert_cell("u4", "INV_X1")));
	netlist.connect(netlist.insert_net("out"),
			netlist.insert_pin("u4:o", netlist.insert_cell("u4", "INV_X1")));

	netlist.connect(netlist.insert_net("iccad_clk"),
			netlist.insert_pin("lcb1:a",
					netlist.insert_cell("lcb1", "INV_Z80")));
	netlist.connect(netlist.insert_net("lcb1_fo"),
			netlist.insert_pin("lcb1:o",
					netlist.insert_cell("lcb1", "INV_Z80")));

	REQUIRE(netlist.number_of_cells() == 6);
	REQUIRE(netlist.number_of_nets() == 9);
	REQUIRE(netlist.number_of_pins() == 19);


//	verilog v(&netlist, "simple");
}

