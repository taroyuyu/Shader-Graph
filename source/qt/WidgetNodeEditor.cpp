#include "WidgetNodeEditor.h"

#include <QVBoxLayout>

#include "model/Node.h"
#include "pin/PinDecl.h"
#include "model/NodeDecl.h"
#include "nodeeditor/NodeManager.h"

#include "pin/PinDecl.h"
#include "model/NodeDecl.h"
#include "nodeeditor/NodeManager.h"

WidgetNodeEditor::WidgetNodeEditor(QWidget * parent) :
    QWidget(parent)
{
    ShaderGraph::NodeManager nodeManager;
    nodeManager.loadNodeStyle("../data/nodestyle.txt");

    nodeManager.registry()->registerModel<ShaderGraph::MasterMaterialOutput>("Output");

    nodeManager.registry()->registerModel<ShaderGraph::ColorNode>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::TextureNode>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::ScalarNode>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::Vec2Node>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::Vec3Node>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::Vec4Node>("Input");
    nodeManager.registry()->registerModel<ShaderGraph::UVNode>("Input");

    nodeManager.registry()->registerModel<ShaderGraph::MakeVec2Node>("VectorUtility");
    nodeManager.registry()->registerModel<ShaderGraph::BreakVec2Node>("VectorUtility");

    nodeManager.registry()->registerModel<ShaderGraph::MakeVec3Node>("VectorUtility");
    nodeManager.registry()->registerModel<ShaderGraph::BreakVec3Node>("VectorUtility");

    nodeManager.registry()->registerModel<ShaderGraph::MakeVec4Node>("VectorUtility");
    nodeManager.registry()->registerModel<ShaderGraph::BreakVec4Node>("VectorUtility");

    nodeManager.registry()->registerModel<ShaderGraph::AndOperatorNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::OrOperatorNode>("LogicalOperator");

    nodeManager.registry()->registerModel<ShaderGraph::EqualNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::NotEqualNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::GreaterNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::LowerNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::GreaterEqualNode>("LogicalOperator");
    nodeManager.registry()->registerModel<ShaderGraph::LowerEqualNode>("LogicalOperator");

    nodeManager.registry()->registerModel<ShaderGraph::AddNode>("Operator");
    nodeManager.registry()->registerModel<ShaderGraph::SubtractNode>("Operator");
    nodeManager.registry()->registerModel<ShaderGraph::MultiplyNode>("Operator");
    nodeManager.registry()->registerModel<ShaderGraph::DivideNode>("Operator");

    nodeManager.registry()->registerModel<ShaderGraph::StepNode>("Functions");

    ShaderGraph::registerToTemplateConverters(nodeManager.registry());

    m_layout    = new QVBoxLayout(this);
    m_scene     = new ShaderGraph::FlowScene(nodeManager.registry(),this);
    m_flowView  = new ShaderGraph::FlowView(m_scene);

    m_layout->addWidget(m_flowView);

    // Setup MasterMaterialOutput
    QString modelName = QStringLiteral("MasterMaterialOutput");

    // create the node
    QtNodes::Node& node = m_scene->createNode(m_scene->registry().create(modelName));

    // Set the node position to the center-right of the flow view
    auto viewportDimension = m_flowView->viewport()->rect();
    node.nodeGraphicsObject().setPos(QPointF((viewportDimension.width() * 2.0f) / 3.0f, 0));
    m_masterMaterialOutput = dynamic_cast<ShaderGraph::MasterMaterialOutput*>(node.nodeDataModel());
    LOG_INFO("Creating : {0} node", modelName.toStdString());
}
