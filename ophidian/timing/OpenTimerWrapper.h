/*
 * Copyright 2017 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.

 */

#ifndef OPHIDIAN_TIMING_OPENTIMERWRAPPER_H
#define OPHIDIAN_TIMING_OPENTIMERWRAPPER_H

#include "OpenTimer.h"
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <unordered_map>

namespace ophidian
{
namespace timing
{
class OpenTimerWrapper
{
public:
    OpenTimerWrapper(const circuit::Netlist &netlist, const circuit::LibraryMapping &libMapping, const standard_cell::StandardCells &stdCells, const std::string &spefPath, const std::string &earlyLibPath, const std::string &lateLibPath, const std::string &timingPath):
                     mCell2OTGate(netlist.makeProperty<opentimer::Gate*>(circuit::Cell())),
                     mPin2OTPin(netlist.makeProperty<opentimer::Pin*>(circuit::Pin())),
                     mNet2OTNet(netlist.makeProperty<opentimer::Net*>(circuit::Net())){

        auto earlyCellLib = mTimer.celllib_ptr(0);
        earlyCellLib->read(earlyLibPath);
        auto lateCellLib = mTimer.celllib_ptr(1);
        lateCellLib->read(lateLibPath);

        for(auto inputIt = netlist.begin(circuit::Input()); inputIt != netlist.end(circuit::Input()); inputIt++)
            mPin2OTPin[netlist.pin(*inputIt)] = mTimer.insert_primary_input(netlist.name(netlist.pin(*inputIt)))->pin_ptr();

        for(auto outputIt = netlist.begin(circuit::Output()); outputIt != netlist.end(circuit::Output()); outputIt++)
            mPin2OTPin[netlist.pin(*outputIt)] = mTimer.insert_primary_output(netlist.name(netlist.pin(*outputIt)))->pin_ptr();

        for(auto netIt = netlist.begin(circuit::Net()); netIt != netlist.end(circuit::Net()); netIt++)
            mNet2OTNet[*netIt] = mTimer.insert_net(netlist.name(*netIt));

        for(auto cellIt = netlist.begin(circuit::Cell()); cellIt != netlist.end(circuit::Cell()); cellIt++)
            mCell2OTGate[*cellIt] = mTimer.insert_gate(netlist.name(*cellIt), stdCells.name(libMapping.cellStdCell(*cellIt)));

        for(auto pinIt = netlist.begin(circuit::Pin()); pinIt != netlist.end(circuit::Pin()); pinIt++)
            mPin2OTPin[*pinIt] = mTimer.pin_ptr(netlist.name(*pinIt));

        for(auto netIt = netlist.begin(circuit::Net()); netIt != netlist.end(circuit::Net()); netIt++)
            for(auto pinIt = netlist.pins(*netIt).begin(); pinIt != netlist.pins(*netIt).end(); ++pinIt)
                mTimer.connect_pin(netlist.name(*pinIt), netlist.name(*netIt));

        mTimer._init_io_timing(timingPath);

        mTimer.read_spef(spefPath);

        mTimer.update_timing(false);
    }

    //! Set the max num threads
    /*!
       \brief Return the maximum number of threads that OpenTimer can invoke.
       \param The maximum number of threads.
     */
    void maxNumThreads(const std::size_t &numberOfThreads){
        mTimer.set_num_threads(numberOfThreads);
    }

    //! Query max number of threads
    /*!
       \brief Return the maximum number of threads that OpenTimer can invoke.
       \return The maximum number of threads.
     */
    const std::size_t maxNumThreads() const {
        return mTimer.max_num_threads();
    }

    //! Query the number of threads.
    /*!
       \brief Query the number of threads.
       \return The number of threads.
     */
    const std::size_t numThreads() const {
        return mTimer.num_threads();
    }

    //! Slew for a given pin
    /*!
       \brief Report the slew in picoseconds at a given pin name, where the pin will be in the current design,
              i.e., no internal spef nodes. Options -rise and -fall are mutually exclusive, and respectively
              specify the desired transition. Options -early and -late are mutually exclusive, and
              respectively specify the desired mode. By default the -early -rise options are selected.
       \param The pin from netlist.
       \param Time mode (0 for early, 1 for late).
       \param Transition type (0 for rise, 1 for fall).
       \return The slew in picoseconds.
     */
    float slew(const circuit::Pin &pin, int earlyLate, int fallRise){
        return mTimer.slew(mPin2OTPin[pin], earlyLate, fallRise);
    }

    //! Arrival Time for a given pin
    /*!
       \brief Report the arrival time in picoseconds at a given pin name, where the pin will be in the
              current design, i.e., no internal spef nodes. Options -rise and -fall are mutually
              exclusive, and respectively specify the desired transition. Options -early and -late are
              mutually exclusive, and respectively specify the desired mode. By default the -early -rise
              options are selected.
       \param The pin from netlist.
       \param Time mode (0 for early, 1 for late).
       \param Transition type (0 for rise, 1 for fall).
       \return The arrival time in picoseconds.
     */
    float at(const circuit::Pin &pin, int earlyLate, int riseFall){
        return mTimer.at(mPin2OTPin[pin], earlyLate, riseFall);
    }

    //! Required Arrival Time for a given pin
    /*!
       \brief Report the required arrival time in picoseconds at a given pin name, where the pin will be
              in the current design, i.e., no internal spef nodes. Options -rise and -fall are mutually
              exclusive, and respectively specify the desired transition. Options -early and -late are
              mutually exclusive, and respectively specify the desired mode. By default the -early -rise
              options are selected.
       \param The pin from netlist.
       \param Time mode (0 for early, 1 for late).
       \param Transition type (0 for rise, 1 for fall).
       \return The required arrival time in picoseconds.
     */
    float rat(const circuit::Pin &pin, int earlyLate, int riseFall){
        return mTimer.rat(mPin2OTPin[pin], earlyLate, riseFall);
    }

    //! Slack for a given pin
    /*!
       \brief Report the slack in picoseconds at a given pin name, where the pin will be in the current
              design, i.e., no internal spef nodes. Options -rise and -fall are mutually exclusive, and
              respectively specify the desired transition. Options -early and -late are mutually
              exclusive, and respectively specify the desired mode. By default the -early -rise options
              are selected.
       \param The pin from netlist.
       \param Time mode (0 for early, 1 for late).
       \param Transition type (0 for rise, 1 for fall).
       \return The slack in picoseconds.
     */
    float slack(const circuit::Pin &pin, int earlyLate, int riseFall){
        return mTimer.slack(mPin2OTPin[pin], earlyLate, riseFall);
    }

    //! Total Negative Slack
    /*!
       \brief Report the total negative slack among the design. The procedure sum up the values across all
              possible transitions and timing splits.
       \return The total negative slack in picoseconds.
     */
    float tns(){
        return mTimer.tns();
    }

    //! Worst Negative Slack
    /*!
       \brief Report the worst negative slack among the design. The procedure check the wns across all
              possible transitions and timing splits.
     */
    float wns(){
        return mTimer.wns();
    }

private:
    opentimer::Timer mTimer;
    ophidian::entity_system::Property<circuit::Cell, opentimer::Gate*> mCell2OTGate;
    ophidian::entity_system::Property<circuit::Pin, opentimer::Pin*> mPin2OTPin;
    ophidian::entity_system::Property<circuit::Net, opentimer::Net*> mNet2OTNet;
};
} // namespace timing
} // namespace ophidian

#endif // OPHIDIAN_TIMING_OPENTIMERWRAPPER_H
