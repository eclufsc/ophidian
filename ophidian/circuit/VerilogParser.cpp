#include "VerilogParser.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <3rdparty/verilog-parser/src/verilog_parser.h>
#include <3rdparty/verilog-parser/src/verilog_ast_util.h>
#ifdef __cplusplus
}
#endif

namespace ophidian
{
    namespace circuit
    {

        Cell VerilogParser::createCell(Netlist* nl, const std::string& name)
        {
            if (cells_.count(name) == 0)
            {
                cells_[name] = nl->add(Cell());
            }

            return cells_.at(name);
        }

        Pin VerilogParser::createPin(Netlist* nl, const std::string& name)
        {
            if (pins_.count(name) == 0)
            {
                pins_[name] = nl->add(Pin());
            }

            return pins_.at(name);
        }

        Net VerilogParser::createNet(Netlist* nl, const std::string& name)
        {
            if (nets_.count(name) == 0)
            {
                nets_[name] = nl->add(Net());
            }

            return nets_.at(name);
        }

        VerilogParser::VerilogParser()
        {
        }

        Netlist* VerilogParser::createNetlist(std::istream& input)
        {
            verilog_parser_init();
            auto nl = std::make_unique<Netlist>();
            std::vector<char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            int result = verilog_parse_string(buffer.data(), buffer.size());

            nl->reserve(Cell(), 4000000);
            nl->reserve(Net(), 4000000);
            nl->reserve(Pin(), 8000000);

            if (result)
            {
                return nullptr;
            }

            assert(yy_verilog_source_tree -> modules -> items == 1);

            auto source = yy_verilog_source_tree;
            ast_module_declaration* module = static_cast<ast_module_declaration*>(ast_list_get(source -> modules, 0));


            ast_list_element* portWalker0 = module->module_ports->head;
            while (portWalker0)
            {
                ast_port_declaration* port = static_cast<ast_port_declaration*>(portWalker0->data);
                ast_list_element * portNameWalker = port->port_names->head;

                while (portNameWalker)
                {

                    ast_identifier identifier = static_cast<ast_identifier>(portNameWalker->data);
                    std::string portName = identifier->identifier;

                    const Pin p = createPin(nl.get(), portName);
                    const Net n = createNet(nl.get(), portName);

                    nl->connect(n, p);

                    switch (port->direction)
                    {
                        case PORT_INPUT:     //!< Input port.
                            nl->add(Input(), p);
                            break;

                        case PORT_OUTPUT:    //!< Output port.
                            nl->add(Output(), p);
                            break;

                        case PORT_INOUT:     //!< Bi-directional port.
                            break;

                        case PORT_NONE:  //!< Used for when we don't know at declaration time.
                            break;
                    }

                    portNameWalker = portNameWalker->next;
                }
                portWalker0 = portWalker0->next;
            }


            ast_list_element * netWalker = module->net_declarations->head;
            while (netWalker)
            {
                ast_net_declaration* net = static_cast<ast_net_declaration*>(netWalker->data);
                const std::string netName(net->identifier->identifier);
                createNet(nl.get(), netName);
                netWalker = netWalker->next;
            }


            ast_list_element * walker = module->module_instantiations->head;
            while (walker)
            {
                ast_module_instantiation* inst = static_cast<ast_module_instantiation*>(walker->data);

                ast_list_element *instWalker = inst->module_instances->head;
                while (instWalker)
                {
                    ast_module_instance* moduleInstance = static_cast<ast_module_instance*>(instWalker->data);
                    const std::string cellName(moduleInstance->instance_identifier->identifier);
                    const Cell c = createCell(nl.get(), cellName);

                    ast_list_element * portWalker =  moduleInstance->port_connections->head;
                    while (portWalker)
                    {
                        ast_port_connection* connection = static_cast<ast_port_connection*>(portWalker->data);
                        ast_expression* exp = connection->expression;
                        const std::string portName = connection->port_name->identifier;
                        const std::string netName = exp->primary->value.identifier->identifier;
                        const std::string pinName = cellName + ":" + portName;
                        const Pin p = createPin(nl.get(), pinName);
                        const Net n = createNet(nl.get(), netName);
                        nl->add(c, p);
                        nl->connect(n, p);
                        portWalker = portWalker->next;
                    }
                    instWalker = instWalker->next;
                }

                walker = walker->next;
            }
            // TODO: verify how to free the syntax tree.
            yy_preproc = nullptr;
            yy_verilog_source_tree = nullptr;

            nl->shrink();

            return nl.release();
        }

    }
}
