#include "Elmore.h"

namespace ophidian
{
namespace timing
{

Elmore::Elmore(const rctree_type & tree) :
    m_tree(tree),
    m_elmore_delay(tree.g()),
    m_downstream_capacitance(tree.g()),
    m_pred(tree.g()),
    m_source(tree.source())
{
    std::deque<capacitor_type> toProcess;
    std::set<capacitor_type> reached;
    m_pred[m_source] = std::make_pair(lemon::INVALID, lemon::INVALID);
    reached.insert(m_source);
    toProcess.push_back(m_source);
    while (!toProcess.empty())
    {
        auto element = toProcess.front();
        toProcess.pop_front();
        m_order.push_back(element);
        for (auto it = m_tree.resistors(element); it != m_tree.invalid(); ++it)
        {
            auto other = m_tree.oppositeCapacitor(element, it);
            if (reached.insert(other).second)
            {
                m_pred[other] = std::make_pair(element, it);
                toProcess.push_back(other);
            }
        }
    }

    assert(m_order.size() == m_tree.size(capacitor_type()));

    update();
}

Elmore::~Elmore()
{

}

void Elmore::update()
{
    for (auto c : m_order)
        m_downstream_capacitance[c] = m_tree.capacitance(c);

    for (auto it = m_order.rbegin(); it != m_order.rend(); ++it)
        if (m_pred[*it].first != lemon::INVALID)
            m_downstream_capacitance[m_pred[*it].first] += m_downstream_capacitance[*it];

    m_elmore_delay[m_source] = time_unit_type(0.0);

    for (auto c : m_order)
        if (m_pred[c].first != lemon::INVALID)
            m_elmore_delay[c] = m_elmore_delay[m_pred[c].first] + m_tree.resistance(m_pred[c].second) * m_downstream_capacitance[c];
}

Elmore::time_unit_type Elmore::at(const capacitor_type & cap) const
{
    return m_elmore_delay[cap];
}

const Elmore::graph_type::NodeMap<std::pair<Elmore::capacitor_type, Elmore::resistor_type>> & Elmore::pred() const
{
    return m_pred;
}

const std::vector<Elmore::capacitor_type> & Elmore::order() const
{
    return m_order;
}

ElmoreSecondMoment::ElmoreSecondMoment(const Elmore & e) :
    m_elmore(e),
    m_tree(e.m_tree),
    m_second_moment(m_tree.g())
{
    update();
}

ElmoreSecondMoment::~ElmoreSecondMoment()
{

}

ElmoreSecondMoment::square_time_unit_type ElmoreSecondMoment::at(const capacitor_type & capacitor) const
{
    return m_second_moment[capacitor];
}

void ElmoreSecondMoment::update()
{
    graph_type::NodeMap<capacitance_per_time_unit> downstream_capacitance(m_tree.g());

    const auto & order = m_elmore.order();

    for (auto c : order)
        downstream_capacitance[c] = m_tree.capacitance(c) * m_elmore.at(c);

    for (auto it = order.rbegin(); it != order.rend(); ++it)
        if (m_elmore.pred()[*it].first != lemon::INVALID)
            downstream_capacitance[m_elmore.pred()[*it].first] += downstream_capacitance[*it];


    m_second_moment[order.front()] = square_time_unit_type(0.0);
    for (auto c : order)
        if (m_elmore.pred()[c].first != lemon::INVALID)
            m_second_moment[c] = m_second_moment[m_elmore.pred()[c].first]
                               + m_tree.resistance(m_elmore.pred()[c].second) * downstream_capacitance[c];
}

}   // namespace timing
}   // namespace ophidian
