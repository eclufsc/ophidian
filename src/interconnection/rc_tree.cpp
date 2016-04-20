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

#include "rc_tree.h"

#include <deque>

namespace ophidian {
namespace interconnection {

rc_tree::rc_tree() :
    m_names(m_graph), m_capacitances(m_graph), m_resistances(m_graph), m_lumped_capacitance(
                                                                           0.0 * si::farad) {

}

rc_tree::rc_tree(const rc_tree &other):
    m_names(m_graph),
    m_capacitances(m_graph),
    m_resistances(m_graph),
    m_lumped_capacitance(0.0 * si::farad)
{
    *this = other;
}

rc_tree &rc_tree::operator=(const rc_tree &other)
{
    lemon::GraphCopy<lemon::ListGraph, lemon::ListGraph> cg(other.graph(), m_graph);
    lemon::ListGraph::NodeMap<lemon::ListGraph::Node> nr(other.graph());
    cg.nodeRef(nr);

    lemon::ListGraph::EdgeMap<lemon::ListGraph::Edge> ecr(m_graph);
    cg.edgeCrossRef(ecr);

    cg.nodeMap(other.m_names, m_names);
    cg.nodeMap(other.m_capacitances, m_capacitances);
    cg.edgeMap(other.m_resistances, m_resistances);
    m_lumped_capacitance = other.m_lumped_capacitance;
    cg.run();

    for(lemon::ListGraph::NodeIt it(m_graph); it != lemon::INVALID; ++it)
        m_name2node[m_names[it]] = it;

    return *this;
}

rc_tree::~rc_tree() {
}

packed_rc_tree rc_tree::pack(capacitor_id source)
{
    packed_rc_tree result(capacitor_count());

    enum status {
        OPEN, TOUCHED, CLOSED
    };

    graph_t::NodeMap<resistor_id> pred(m_graph);
    graph_t::NodeMap<std::size_t> order(m_graph);
    std::vector<capacitor_id> reverse_order(capacitor_count());
    graph_t::NodeMap<status> stat(m_graph, OPEN);
    std::deque<capacitor_id> ready;
    ready.push_back(source);
    std::size_t current_order{0};
    while(!ready.empty())
    {
        auto cap = ready.front();
        ready.pop_front();
        stat[cap] = CLOSED;
        reverse_order[current_order] = cap;
        order[cap] = current_order++;
        for(graph_t::OutArcIt it(m_graph, cap); it != lemon::INVALID; ++it)
        {
            capacitor_id target = m_graph.target(it);
            switch(stat[target])
            {
            case OPEN:
                pred[target] = it;
                stat[target] = TOUCHED;
                ready.push_back(target);
                break;
            case TOUCHED:
            case CLOSED:
                break;
            }
        }
    }


    result.pred(0, std::numeric_limits<std::size_t>::max());
    for(current_order = 0; current_order < reverse_order.size(); ++current_order)
    {
        auto current_capacitor = reverse_order[current_order];
        result.capacitance(current_order, capacitance(current_capacitor));
        if(current_order > 0)
        {
            auto parent = m_graph.oppositeNode(current_capacitor, pred[current_capacitor]);
            result.resistance(current_order, resistance(pred[current_capacitor]));
            result.pred(current_order, order[parent]);
        }
    }

    return result;
}

lemon::ListGraph::Node rc_tree::capacitor_insert(std::string name) {
    auto result = m_name2node.find(name);
    if (result != m_name2node.end())
        return result->second;
    auto node = m_graph.addNode();
    m_names[node] = name;
    m_name2node[name] = node;
    m_capacitances[node] = 0.0*si::farad;
    return node;
}

lemon::ListGraph::Edge rc_tree::resistor_insert(lemon::ListGraph::Node u,
                                                lemon::ListGraph::Node v, quantity<si::resistance> res) {
    auto resistor = m_graph.addEdge(u, v);
    m_resistances[resistor] = res;
    return resistor;
}

void rc_tree::capacitance(lemon::ListGraph::Node u,
                          quantity<si::capacitance> cap) {
    m_lumped_capacitance -= m_capacitances[u];
    m_capacitances[u] = cap;
    m_lumped_capacitance += m_capacitances[u];
}

packed_rc_tree::packed_rc_tree(std::size_t node_count) :
    m_pred(node_count, -1),
    m_resistances(node_count),
    m_capacitances(node_count)
{

}

packed_rc_tree::~packed_rc_tree()
{

}

void packed_rc_tree::pred(std::size_t i, std::size_t pred)
{
    m_pred[i] = pred;
}

void packed_rc_tree::capacitance(std::size_t i, quantity<si::capacitance> cap)
{
    m_capacitances[i] = cap;
}

void packed_rc_tree::resistance(std::size_t i, quantity<si::resistance> res)
{
    m_resistances[i] = res;
}

} /* namespace timing */
} /* namespace ophidian */

