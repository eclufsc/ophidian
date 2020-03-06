#include <catch.hpp>

#include <ophidian/parser/ICCAD2020.cpp>

using namespace ophidian::parser;

TEST_CASE("ICCAD2020: read case1.txt file", "[parser][iccad2020]")
{
    ICCAD2020 sample = ICCAD2020{"input_files/iccad2020/case1.txt"};
}
