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

            if (result)
            {
                return nullptr;
            }

            assert(yy_verilog_source_tree -> modules -> items == 1);

            auto source = yy_verilog_source_tree;
            ast_module_declaration* module = static_cast<ast_module_declaration*>(ast_list_get(source -> modules, 0));


            for (uint32_t i = 0; i < module->module_ports->items; i++)
            {
                ast_port_declaration* port = static_cast<ast_port_declaration*>(ast_list_get(module->module_ports, i));

                for (uint32_t j = 0; j < port->port_names->items; ++j)
                {
                    ast_identifier identifier = static_cast<ast_identifier>(ast_list_get(port->port_names, j));
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
                }
            }

            for (uint32_t i = 0; i < module->net_declarations->items; ++i)
            {
                ast_net_declaration* net = static_cast<ast_net_declaration*>(ast_list_get(module->net_declarations, i));
                const std::string netName(net->identifier->identifier);
                createNet(nl.get(), netName);
            }

            for (uint32_t i = 0; i < module->module_instantiations->items; ++i)
            {
                ast_module_instantiation* inst = static_cast<ast_module_instantiation*>(ast_list_get(module->module_instantiations, i));

                for (uint32_t j = 0; j < inst->module_instances->items; ++j)
                {
                    ast_module_instance* module_instance = static_cast<ast_module_instance*>(ast_list_get(inst->module_instances, j));
                    const std::string cellName(module_instance->instance_identifier->identifier);
                    const Cell c = createCell(nl.get(), cellName);

                    for (uint32_t k = 0; k < module_instance->port_connections->items; ++k)
                    {
                        ast_port_connection* connection = static_cast<ast_port_connection*>(ast_list_get(module_instance->port_connections, k));
                        ast_expression* exp = connection->expression;
                        const std::string portName = connection->port_name->identifier;
                        const std::string netName = exp->primary->value.identifier->identifier;
                        const std::string pinName = cellName + ":" + portName;
                        const Pin p = createPin(nl.get(), pinName);
                        const Net n = createNet(nl.get(), netName);
                        nl->add(c, p);
                        nl->connect(n, p);
                    }
                }
            }
            // TODO: verify how to free the syntax tree.
            yy_preproc = nullptr;
            yy_verilog_source_tree = nullptr;
            return nl.release();
        }

    }
}
