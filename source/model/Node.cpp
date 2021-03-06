#include "Node.h"

#include <nodes/Connection>

namespace ShaderGraph
{
    /// Give for a specified port, the number of data.
    /// @portType : the type of the port.
    /// @return : the number of the data.
    unsigned int Node::nPorts(QtNodes::PortType portType) const
    {
        switch (portType)
        {
            case QtNodes::PortType::In :    return static_cast<unsigned int>(m_inputs.size());
            case QtNodes::PortType::Out :   return static_cast<unsigned int>(m_outputs.size());
            default :                       break;
        }
        return 0;
    }

    /// Give for a specified port, its type.
    /// @portType : the type of the port.
    /// @index : the index of the port.
    /// @return : the type of the data.
    QtNodes::NodeDataType Node::dataType(QtNodes::PortType portType, QtNodes::PortIndex index) const
    {
        switch (portType)
        {
            case QtNodes::PortType::In :    return m_inputs[index]->type();
            case QtNodes::PortType::Out :   return m_outputs[index]->type();
            default :                       break;
        }
        return QtNodes::NodeDataType();
    }

    /// That event happens when an other node is plug-in.
    /// @data : the plugged-in data node.
    /// @index : the index of the port.
    void Node::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex index)
    {
        if (index >= 0 || index < (int) m_inputs.size())
        {
            auto input = std::dynamic_pointer_cast<IPin>(data);
            auto pin   = std::dynamic_pointer_cast<IPin>(m_inputs[index]);

            assert(pin != nullptr);

            if (input)
            {
                std::string connectedPinName = data->type().name.toStdString();
                std::string inputName = m_inputs[index]->type().name.toStdString();
                LOG_DEBUG("Connect : Input {0} to Output {1}", inputName, connectedPinName);

                pin->connect(data);
            }
        }
        else LOG_ERROR("ShaderGraph::Node::setInData : Invalid port index");
    }

    /// The output of this node, at the specified index.
    /// @index : the index of the port.
    /// @return : a shared point to the data to retrieve.
    std::shared_ptr<QtNodes::NodeData> Node::outData(QtNodes::PortIndex index)
    {
        if (index >= 0 || index < (int) m_outputs.size())
        {
            auto output = m_outputs[index];

            if (output == nullptr)
            {
                LOG_WARN("Node::outData : An output data is null");
            }
            return output;
        }

        LOG_ERROR("ShaderGraph::Node::outData : Invalid port index");
        return nullptr;
    }

    /// Specified the embedded widget in the Node.
    /// @return : the widget.
    QWidget * Node::embeddedWidget()
    {
        return nullptr;
    }

    /// Getter only to the reference to the name.
    QString Node::name() const
    {
        return m_name;
    }

    /// Getter to the reference to the caption.
    QString Node::caption() const
    {
        return m_caption;
    }

    void Node::showDetails(QTreeWidget * tree)
    {
        (void) tree;
        m_isDetailedNode = true;
    }

    void Node::hideDetails(QTreeWidget * tree)
    {
        (void) tree;
        m_isDetailedNode = false;
    }

    std::string Node::autoName(PIN& pin)
    {
        return "id" + std::to_string(m_id) + "_" + pin->type().name.toStdString();
    }

    GLSLData Node::outputsToGLSL()
    {
        GLSLData glslData;

        for (PIN output : m_outputs)
        {
            auto pin = std::dynamic_pointer_cast<IPin>(output);
            assert(pin);

            std::string line = pin->typeToGLSL()         + " "   +
                               autoName(output)          + " = " +
                               pin->defaultValueToGLSL() + "; // Output" ;

            glslData.generatedCode += line + "\n";
        }
        return glslData;
    }

    GLSLData Node::inputsToGLSL(std::list<unsigned int>& nodes)
    {
        GLSLData glslData;

        for (PIN input : m_inputs)
        {
            auto pin = std::dynamic_pointer_cast<IPin>(input);
            assert(pin);

            std::string value = "INVALID_VALUE";

            if (pin->isConnected())
            {
                std::shared_ptr<QtNodes::NodeData> connectedNodeData = pin->getConnectedPin();

                auto connectedPin = std::dynamic_pointer_cast<IPin>(connectedNodeData);
                assert(connectedPin);

                auto connectedNode = dynamic_cast<Node*>(connectedPin->getNode());
                assert(connectedNode);

                glslData.generatedCode += "\n";
                glslData = glslData + connectedNode->toGLSL(nodes);

                value = connectedNode->autoName(connectedNodeData);
            }
            else value = pin->defaultValueToGLSL();

            std::string line = pin->typeToGLSL() + " "   +
                               autoName(input)   + " = " +
                               value             + ";  // Input" ;

            glslData.generatedCode += line + "\n";
        }

        return glslData;
    }

    GLSLData Node::toGLSL()
    {
        GLSLData glslData;
        std::list<unsigned int> nodes;

        nodes.push_back(m_id);

        glslData = glslData + inputsToGLSL(nodes);
        glslData = glslData + outputsToGLSL();
        glslData = glslData + nodeToGLSL();

        return glslData;
    }

    GLSLData Node::toGLSL(std::list<unsigned int>& nodes)
    {
        GLSLData glslData;

        bool isFound = std::find(nodes.begin(), nodes.end(), m_id) != nodes.end();

        if (!isFound)
        {
            nodes.push_back(m_id);

            glslData = glslData + inputsToGLSL(nodes);
            glslData = glslData + outputsToGLSL();
            glslData = glslData + nodeToGLSL();
        }

        return glslData;
    }

    void Node::inputConnectionDeleted(const QtNodes::Connection& c)
    {
        Q_UNUSED(c);

        QtNodes::PortIndex portIndex = c.getPortIndex(QtNodes::PortType::In);
        auto pin = std::dynamic_pointer_cast<IPin>(m_inputs[portIndex]);

        assert(pin);

        std::string inputName = m_inputs[portIndex]->type().name.toStdString();
        LOG_DEBUG("Disconnect : Input {0} is now disconnected", inputName);

        pin->disconnect();
    }

    void Node::inputConnectionCreated(const QtNodes::Connection& c)  { Q_UNUSED(c); }
    void Node::outputConnectionCreated(const QtNodes::Connection& c) { Q_UNUSED(c); }
    void Node::outputConnectionDeleted(const QtNodes::Connection& c) { Q_UNUSED(c); }
}
