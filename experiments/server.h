#include <zmq.hpp>
#include <string>
#include <ophidian/design/Design.h>
#include <ophidian/routing/ICCAD2020Wrapper.h>
#include <memory>

namespace ophidian
{
    class Server
    {
        public:
            void run();
        private:
            std::string identifyRequest(const zmq::message_t & msg);
            void readCircuit(const std::string & path);
            void getGCellCapacity(const std::vector<std::string> & tokens);
            void getRoutingSegments(const std::vector<std::string> & tokens);
            void getPinDensity(const std::vector<std::string> & tokens);
            bool doILPRouting();
            void doILPRouteNet(const std::vector<std::string> & tokens);
            void moveCell(const std::vector<std::string> & tokens);

            zmq::context_t m_context{1};
            zmq::socket_t m_socket;
            ophidian::design::Design m_design;
            std::string m_circuit_name;
            std::unique_ptr<ophidian::routing::ICCAD2020Wrapper> m_wrapper_ptr;//lazy initialization
    };
}
