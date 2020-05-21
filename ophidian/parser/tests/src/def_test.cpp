#include <catch.hpp>

#include <ophidian/parser/Def.h>
#include <ophidian/parser/ParserException.h>
#include <algorithm>

using ophidian::parser::Def;
using dbu_t = Def::database_unit_type;
using scalar_t = Def::scalar_type;
using orient_t = Def::component_type::orientation_type;

TEST_CASE("Def: Try to load inexistent file", "[parser][Def]")
{
    CHECK_THROWS_AS(
        Def{"a_file_with_this_name_should_not_exist"},
        ophidian::parser::exceptions::InexistentFile
    );
}

TEST_CASE("Def: Loading simple.def", "[parser][Def][simple]")
{
    auto simple = Def{"../../../../test/input_files/simple/simple.def"};

    SECTION("Def: Check units distance", "[parser][Def][simple][ratio]"){
        CHECK(simple.dbu_to_micrometer_ratio() == 2000.0);
    }

    SECTION("Def: Diearea lower and upper points", "[parser][Def][simple][die_area]"){
        CHECK(simple.die_area().min_corner().x() == dbu_t{0});
        CHECK(simple.die_area().min_corner().y() == dbu_t{0});
        CHECK(simple.die_area().max_corner().x() == dbu_t{27360});
        CHECK(simple.die_area().max_corner().y() == dbu_t{13680});
    }

    SECTION("Def: Checking Component vector", "[parser][Def][simple][components]"){
        auto& components = simple.components();
        CHECK(components.size() == 6);

        auto& first_component = components.front();
        CHECK(first_component.name() == "u1");
        CHECK(first_component.macro() == "NAND2_X1");
        CHECK(first_component.orientation() == orient_t::N);
        CHECK(first_component.position().x() == dbu_t{3420});
        CHECK(first_component.position().y() == dbu_t{6840});
        CHECK_FALSE(first_component.fixed());

        auto& midle_component = components.at(2);
        CHECK(midle_component.name() == "f1");
        CHECK(midle_component.macro() == "DFF_X80");
        CHECK(midle_component.orientation() == orient_t::N);
        CHECK(midle_component.position().x() == dbu_t{760});
        CHECK(midle_component.position().y() == dbu_t{0});
        CHECK(midle_component.fixed());

        auto& last_component = components.back();
        CHECK(last_component.name() == "lcb1");
        CHECK(last_component.macro() == "INV_Z80");
        CHECK(last_component.orientation() == orient_t::N);
        CHECK(last_component.position().x() == dbu_t{0});
        CHECK(last_component.position().y() == dbu_t{10260});
        CHECK_FALSE(last_component.fixed());
    }

    SECTION("Def: Checking Row vector", "[parser][Def][simple][rows]"){
        auto& rows = simple.rows();
        CHECK(rows.size() == 4);

        auto& first_row = rows.front();
        CHECK(first_row.name() == "core_SITE_ROW_0");
        CHECK(first_row.site() == "core");
        CHECK(first_row.origin().x() == dbu_t{0.0});
        CHECK(first_row.origin().y() == dbu_t{0.0});
        CHECK(first_row.step().x() == dbu_t{380.0});
        CHECK(first_row.step().y() == dbu_t{0.0});
        CHECK(first_row.num().x() == scalar_t{72.0});
        CHECK(first_row.num().y() == scalar_t{1.0});

        auto& last_row = rows.back();
        CHECK(last_row.name() == "core_SITE_ROW_3");
        CHECK(last_row.site() == "core");
        CHECK(last_row.origin().x() == dbu_t{0.0});
        CHECK(last_row.origin().y() == dbu_t{10260.0});
        CHECK(last_row.step().x() == dbu_t{380.0});
        CHECK(last_row.step().y() == dbu_t{0.0});
        CHECK(last_row.num().x() == scalar_t{72.0});
        CHECK(last_row.num().y() == scalar_t{1.0});
    }

    SECTION("Def: Checking Nets", "[parser][Def][simple][nets]"){
        auto& nets = simple.nets();

        CHECK(nets.empty());
    }
}

TEST_CASE("Def: Loading sample def ispd18", "[parser][Def][ispd18]")
{
    auto sample = Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};

    CHECK(sample.dbu_to_micrometer_ratio() == 2000.0);

    CHECK(sample.components().size() == 22);

    CHECK(sample.rows().size() == 5);

    CHECK(sample.nets().size() == 11);

    SECTION("Net: test net names and pins", "[parser][Def][Net][ispd18]")
    {
        auto& first_net = sample.nets().front();

        CHECK(first_net.name() == "net1237");

        CHECK(first_net.pins().size() == 2);

        auto& first_net_first_pin = first_net.pins().front();

        CHECK(first_net_first_pin.first == "inst5638");
        CHECK(first_net_first_pin.second == "A");

        auto& first_net_last_pin = first_net.pins().back();

        CHECK(first_net_last_pin.first == "inst4678");
        CHECK(first_net_last_pin.second == "Y");

        auto& last_net = sample.nets().back();

        CHECK(last_net.name() == "net1230");

        CHECK(last_net.pins().size() == 2);

        auto& last_net_first_pin = last_net.pins().front();

        CHECK(last_net_first_pin.first == "inst7234");
        CHECK(last_net_first_pin.second == "Y");

        auto& last_net_last_pin = last_net.pins().back();

        CHECK(last_net_last_pin.first == "inst5195");
        CHECK(last_net_last_pin.second == "C0");
    }
}

TEST_CASE("Def: Loading ispd_18_sample.input.def", "[parser][Def][ispd18][sample]")
{
    Def sample = Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};

    auto& tracks = sample.tracks();
    CHECK(tracks.size() == 18);

    auto& first_track = tracks.front();
    CHECK(first_track.orientation() == Def::track_type::orientation_type::VERTICAL);
    CHECK(first_track.start() == dbu_t{83800.0});
    CHECK(first_track.number_of_tracks() == scalar_t{52.0});
    CHECK(first_track.space() == dbu_t{400.0});
    CHECK(first_track.layer_name() == "Metal9");
}

TEST_CASE("Def: Test for GCELLGRID keyword", "[parser][Def][ispd19][sample]")
{
    Def sample = Def{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.def"};

    auto & gcells = sample.gcells();
    std::vector<ophidian::parser::GCell> expected_gcells = {{false, 390100, 2, 500}, {false, 100, 196, 2000}, {false, 0, 2, 100},
                                                            {true, 388200, 2, 1800}, {true, 200, 195, 2000}, {true, 0, 2, 200}};
    REQUIRE(std::is_permutation(gcells.begin(), gcells.end(), expected_gcells.begin()));
}

TEST_CASE("Def: Test for GCELL", "[parser][Def][gcell]")
{
    Def sample = Def{"input_files/ophidianSample/ophidian_sample.def"};

    SECTION("GCell Unroll values", "[parser][Def][gcell]")
    {
        // GCELLGRID X 1100 DO 2 STEP 50 ;
        // GCELLGRID X 200 DO 4 STEP 300 ;
        // GCELLGRID X 0 DO 2 STEP 200 ;
        // GCELLGRID Y 700 DO 2 STEP 150 ;
        // GCELLGRID Y 100 DO 4 STEP 200 ;
        // GCELLGRID Y 0 DO 2 STEP 100 ;
        using dbu = ophidian::parser::Def::database_unit_type;
        std::vector<dbu> expected_gcells_x_axis{dbu{0}, dbu{200}, dbu{500}, dbu{800}, dbu{1100}, dbu{1150}};
        std::vector<dbu> expected_gcells_y_axis{dbu{0}, dbu{100}, dbu{300}, dbu{500}, dbu{700}, dbu{850}};

        auto obtained_gcells_x_axis = sample.gcell_x_axis();
        auto obtained_gcells_y_axis = sample.gcell_y_axis();

        REQUIRE(obtained_gcells_x_axis.size() == 6);
        REQUIRE(obtained_gcells_y_axis.size() == 6);
        REQUIRE(std::equal(obtained_gcells_x_axis.begin(), obtained_gcells_x_axis.end(), expected_gcells_x_axis.begin()));
        REQUIRE(std::equal(obtained_gcells_y_axis.begin(), obtained_gcells_y_axis.end(), expected_gcells_y_axis.begin()));
    }
}
