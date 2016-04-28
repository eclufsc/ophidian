/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/


#include "../catch.hpp"

#include "../timing/spef.h"
#include "../interconnection/rc_tree.h"

#include <boost/date_time/posix_time/posix_time.hpp>


using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;
enum colors {
    OPEN, TOUCHED, CLOSED
} ;

#include <omp.h>

#include <lemon/connectivity.h>



void dfs(const interconnection::rc_tree & rc_tree, lemon::ListGraph::NodeMap< interconnection::rc_tree::resistor_id > & pred, lemon::ListGraph::NodeMap< colors > & color,  interconnection::rc_tree::capacitor_id source) {

    color[source] = CLOSED;


    std::vector<interconnection::rc_tree::resistor_id> edges;
    edges.reserve(10);
    for(auto it = rc_tree.capacitor_resistors(source); it != interconnection::rc_tree::invalid(); ++it)
        edges.push_back(it);

    int i;
#pragma omp parallel for
    for(i = 0; i < edges.size(); i++)
    {
        auto target = rc_tree.other_capacitor(edges[i], source);
        if(color[target] == OPEN)
        {
            color[target] = TOUCHED;
            pred[target] = edges[i];
            #pragma omp shared(color, pred)
            dfs(rc_tree, pred, color, target);
        }
    }

}


TEST_CASE("parallel elmore sandbox","[timing][multithread][elmore]")
{

//    timing::spef spef;
//    spef.read("/home/csguth/Downloads/starters_kit/superblue18.spef");
//    spef.m_trees.resize(1);

//    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();
//    for( const timing::spef_tree & front : spef.trees() )
//    {
//        interconnection::packed_rc_tree packed = front.tree.pack(front.tree.capacitor_by_name(front.source));
//        std::vector<quantity<si::time> > delays(packed.node_count());
//        std::vector<quantity<capacitance> > downstream(packed.node_count());
//        for(std::size_t i{0}; i < packed.node_count(); ++i)
//            downstream[i] = packed.capacitance(i);
//        for(int i{packed.node_count()-1}; i > 0; --i)
//            downstream[packed.pred(i)] += downstream[i];
//        for(std::size_t i{1}; i < packed.node_count(); ++i)
//            delays[i] = delays[packed.pred(i)] + packed.resistance(i)*downstream[i];
//    }
//    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
//    boost::posix_time::time_duration msdiff = mst2 - mst1;
//    std::cout << "sequential time: " << msdiff.total_milliseconds() << " ms" << std::endl;



//    omp_set_num_threads(8);
//    mst1 = boost::posix_time::microsec_clock::local_time();


//    for( const timing::spef_tree & front : spef.trees() )
//    {
//        lemon::ListGraph::NodeMap<int> order(front.tree.graph());
//        lemon::topologicalSort(front.tree.graph(), order);
//        std::vector<lemon::ListGraph::Node> order2(front.tree.capacitor_count());


//        std::vector<std::size_t> branches;
//        branches.reserve(order2.size());

//        for(lemon::ListGraph::NodeIt it(front.tree.graph()); it != lemon::INVALID; ++it)
//            order2[order[it]] = it;

//        for(auto node : order2)
//        {
//        }



//    }

//    mst2 = boost::posix_time::microsec_clock::local_time();
//    msdiff = mst2 - mst1;
//    std::cout << "parallel time: " << msdiff.total_milliseconds() << " ms" << std::endl;
}
