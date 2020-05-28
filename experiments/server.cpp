#include "server.h"
#include <iostream>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/design/DesignFactory.h>

namespace ophidian
{
    void Server::run()
    {
        m_socket = {m_context, ZMQ_REP};
        m_socket.bind("tcp://*:5555");

        while (true)
        {
            zmq::message_t request;

            //  Wait for next request from client
            std::cout << "Waiting for client request." << std::endl;
            m_socket.recv(&request);

            auto response = identifyRequest(request);
            //  Do some 'work'
            sleep(1);

            //  Send reply back to client
            auto size = response.size();
            zmq::message_t reply(size);
            memcpy(reply.data(), response.c_str(), size);
            m_socket.send(reply);
        }
    }

    std::string Server::identifyRequest(const zmq::message_t & msg)
    {
        std::string response;
        std::string message = (char*)msg.data();
        std::istringstream iss{message};
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};

        std::vector<std::string> commands{"read_circuit circuit_path",
            "gcell_capacity x y z", "net_segments net_name", "pin_density x y z",
            "cell_density x y z", "ilp_route", "astar_route net_name",
            "move_cell cell_name x_old y_old z_old x_new y_new z_new"};
        if(tokens.at(0) == "read_circuit")
        {
            m_circuit_name = tokens.at(1);
            readCircuit(tokens.at(2));
            response = "Check server log to see if circuit was read.";
        }else if(tokens.at(0) == "gcell_capacity")
        {
        }else if(tokens.at(0) == "net_segments")
        {
        }else if(tokens.at(0) == "pin_density")
        {
        }else if(tokens.at(0) == "cell_density")
        {
        }else if(tokens.at(0) == "ilp_route")
        {
            response = doILPRouting() ? "circuit fully routed" :
                                        "circuit could not be routed";
        }else if(tokens.at(0) == "astar_route")
        {
        }else if(tokens.at(0) == "route_net")
        {
        }else if(tokens.at(0) == "move_cell")
        {
        }else{
            std::cout<<"ERROR: unrecognized command."<<std::endl;
            std::cout<<"Please use: ";
            response = "ERROR: unrecognized command.\nPlease use: ";
            for(auto c : commands)
            {
                std::cout<<c<<" | ";
                response += c + " | ";
            }
            std::cout<<std::endl;
        }
        return response;
    }

    void ophidian::Server::readCircuit(const std::string & path)
    {
        auto iccad_2020 = ophidian::parser::ICCAD2020{path};
        ophidian::design::factory::make_design_iccad2020(m_design, iccad_2020);
        m_wrapper_ptr = std::make_unique<ophidian::routing::ICCAD2020Wrapper>(m_design, m_circuit_name);
    }

    bool ophidian::Server::doILPRouting()
    {
        ophidian::parser::ICCAD2020Writer iccad_output_writer(m_design, m_circuit_name);
        std::cout << "Running ILP routing for whole circuit..." << std::endl;
        auto result = m_wrapper_ptr->route_circuit();
        std::cout << "Result: " << result << std::endl;

        if(result)
        {
            std::cout << "Writing output ..." << std::endl;
            iccad_output_writer.write_ICCAD_2020_output("", {});
        }
        return result;
    }

    void moveCell(const std::vector<std::string> & tokens)
    {
        //create a function to update all gcell demands.
        //
        //first get cellname source and target gcell.
        //move cell
        //incrementally update extra demands from gcell end its neighbors
        //
        //apply a diff with Global_Routing_Feature and also with CPLEX and ZEROMQ Wrapper
    }
}
