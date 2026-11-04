#include "FormMillToolsSimulation.h"


extern "C" XI_EXPORT bool run(const char* context)
{   app = Application::get(); if (!app) return false;
    ui = app->userInterface(); if (!ui) return false;
    
    Ptr<CommandDefinitions>commandDefinitions = ui->commandDefinitions(); if (!commandDefinitions) return false;
    Ptr<Toolbars> toolbars = ui->toolbars(); if (!toolbars)  return false;
    Ptr<Toolbar> toolbarQAT = toolbars->itemById("QAT"); if (!toolbarQAT)   return false;
    Ptr<ToolbarControls> toolbarControlsQAT = toolbarQAT->controls();  if (!toolbarControlsQAT) return false;
    const std::string commandName = "FormMillToolSimulation";
    const std::string commandDescription = "Simulation with Operations containing Form Mill Tools";
    const std::string commandResources = "./resources/FormMillToolsSimulation";
    Ptr<ToolbarControl> btnCmdToolbarCtlQAT = toolbarControlsQAT->itemById("FormMillToolsSimulation");
    if (!btnCmdToolbarCtlQAT)
    {
        Ptr<CommandDefinition> btnCmdDefinitionQAT = commandDefinitions->itemById("FormMillToolsSimulation");
        if (!btnCmdDefinitionQAT)
        {
            btnCmdDefinitionQAT = commandDefinitions->addButtonDefinition("FormMillToolsSimulation", commandName, commandDescription, commandResources);
        }
        Ptr<CommandCreatedEvent> btnCmdCreatedEvent = btnCmdDefinitionQAT->commandCreated(); if (!btnCmdCreatedEvent)   return false;
        btnCmdCreatedEvent->add(&onCommandCreatedOnQAT);
        btnCmdToolbarCtlQAT = toolbarControlsQAT->addCommand(btnCmdDefinitionQAT); if (!btnCmdToolbarCtlQAT)  return false;
        btnCmdToolbarCtlQAT->isVisible(true);
        //ui->messageBox("A demo button command is successfully added to the Quick Access Toolbar");
        
        
    }
    ui->commandTerminated()->add(&onSimulatTerminatedHandler_);
    return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
    
    //if (!ui) return false;
    Ptr<Toolbars>  toolbars = ui->toolbars();if (!toolbars) return false;
    Ptr<Toolbar> toolbarQAT = toolbars->itemById("QAT"); if (!toolbarQAT) return false;
    Ptr<ToolbarControls> toolbarControlsQAT = toolbarQAT->controls();  if (!toolbarControlsQAT) return false;
    Ptr<ToolbarControl>  btnCmdToolbarCtlQAT = toolbarControlsQAT->itemById("FormMillToolsSimulation");

    //Ptr<ToolbarControl> listCmdToolbarCtlQAT = toolbarControlsQAT->itemById(listCmdIdOnQAT);
    Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();  if (!commandDefinitions) return false;
    Ptr<CommandDefinition> btnCmdDefinitionQAT = commandDefinitions->itemById("FormMillToolsSimulation");

    //Ptr<CommandDefinition> listCmdDefinitionQAT = commandDefinitions->itemById(listCmdIdOnQAT);
    Ptr<Workspaces> workspaces = ui->workspaces();if (!workspaces) return false;
    Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment"); if (!modelingWorkspace) return false;
    Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels(); if (!toolbarPanels)  return false;
    //Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelId); if (!toolbarPanel)  return false;
    //Ptr<ToolbarControls> toolbarControlsPanel = toolbarPanel->controls();if (!toolbarControlsPanel) return false;
    //Ptr<ToolbarControl> toolbarControlPanel = toolbarControlsPanel->itemById(commandIdOnPanel);
    //Ptr<CommandDefinition> commandDefinitionPanel = commandDefinitions->itemById(commandIdOnPanel);
    if (btnCmdToolbarCtlQAT) btnCmdToolbarCtlQAT->deleteMe();
    //if(listCmdToolbarCtlQAT) listCmdToolbarCtlQAT->deleteMe();
    if (btnCmdDefinitionQAT) btnCmdDefinitionQAT->deleteMe();
    
    ui->commandTerminated()->remove(&onSimulatTerminatedHandler_); 
    if (ui) ui = nullptr;
    
    return true;
}


int FormMillToolSimulation()
{
    try
    {   
        Ptr<Document> ActiveDoc = app->activeDocument(); if (!ActiveDoc) return false;
        Ptr<Workspace> ManufEnvirWorkSpace = ui->workspaces()->itemById("CAMEnvironment"); if (!ManufEnvirWorkSpace) return false;
        ManufEnvirWorkSpace->activate();
        Ptr<TextCommandPalette> TxtCmdPalette = ui->palettes()->itemById("TextCommands"); if (!TxtCmdPalette) return false;
        Ptr<Products> DocProducts = ActiveDoc->products(); if (!DocProducts) return false;
        Ptr<Product> DocCAMProduct = DocProducts->itemByProductType("CAMProductType"); if (!DocCAMProduct) { ui->messageBox("THERE ARE NO CAM Operations in the Active Document"); return false; }
        Ptr<CAM> DocCAM = Ptr<CAM>(DocCAMProduct); if (!DocCAM) return false;
        Ptr<Setups> DocCAMSetups = DocCAM->setups(); if (!DocCAMSetups) return false;
        Ptr<Setup> ActiveSetup = DocCAMSetups->item(0); if (!ActiveSetup) return false;
        Ptr<OperationBase> SetupOper = nullptr;
        Ptr<OperationBase> SetupSimulOper = nullptr;
        string SetupOperName;
        string SetupSImulOperName;
        double OpBotHeightOffset = 0;
        double OpToolTipOffset = 0;
        Ptr<FloatParameterValue> OpBotHeightOffsetParVal = nullptr;
        Ptr<FloatParameterValue> SimulatOpBotHeightOffsetParVal = nullptr;
        //Ptr<FloatParameterValue> OpToolTipOffsetParVal = nullptr;
        int SetupOpersNum = (int)ActiveSetup->operations()->count();
        
        int SetupOpersWithFormMillTool = 0;
        for (int i=0;i< SetupOpersNum;i++)
        {SetupOper = ActiveSetup->operations()->item(i);
        if (SetupOper->parameters()->itemByName("operation_description")->expression().find("Contour")==-1) continue;
        if (SetupOper->parameters()->itemByName("tool_tipOffset") !=nullptr) OpToolTipOffset = ((Ptr<FloatParameterValue>)SetupOper->parameters()->itemByName("tool_tipOffset")->value())->value(); //Unit cm
        if (SetupOper->parameters()->itemByName("bottomHeight_offset") != nullptr)
        {
            OpBotHeightOffsetParVal = (Ptr<FloatParameterValue>)SetupOper->parameters()->itemByName("bottomHeight_offset")->value();
            OpBotHeightOffset = OpBotHeightOffsetParVal->value();//Unit cm

        }
        if (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'" && OpToolTipOffset>0)
        {if (!SetupOper->duplicate()) return false;
         SetupOpersNum++;
         //OperWithFormMillToolIndexes.push_back(i);
         SetupOpersWithFormMillTool++;
         
         SetupSimulOper = ActiveSetup->operations()->item(i + 1);
         SetupSimulOper->name(SetupOper->name() + "-SIMUL");
         SimulatOpBotHeightOffsetParVal = (Ptr<FloatParameterValue>)SetupSimulOper->parameters()->itemByName("bottomHeight_offset")->value();
         SimulatOpBotHeightOffsetParVal->value(OpBotHeightOffset - OpToolTipOffset); //Unit cm
         //SetupSimulOper->parameters()->itemByName("bottomHeight_offset")->expression()
         
         DocCAM->generateToolpath(SetupSimulOper);
         ui->messageBox("WAIT UNTIL OPERATION GENERATES");
         SetupOper->isSuppressed(true);
         
         

         i++;
        
        }
        }
        
        
        if (!DocCAM->checkAllToolpaths()) DocCAM->generateAllToolpaths(true);
        
        
        
            
        
        
        
        
        }
        
    
    catch (std::exception* e)
    {
        if (ui)
        {
            ui->messageBox(std::string(e->what()));

        }
    }

    catch (...)
    {
        
       ui->messageBox("Generic Exception Thrown");

    }
    return true;
}
int CleanSimulatOperations()
{
    try
    {
        Ptr<Document> ActiveDoc = app->activeDocument(); if (!ActiveDoc) return false;
        Ptr<Workspace> ManufEnvirWorkSpace = ui->workspaces()->itemById("CAMEnvironment"); if (!ManufEnvirWorkSpace) return false;
        ManufEnvirWorkSpace->activate();
        Ptr<TextCommandPalette> TxtCmdPalette = ui->palettes()->itemById("TextCommands"); if (!TxtCmdPalette) return false;
        Ptr<Products> DocProducts = ActiveDoc->products(); if (!DocProducts) return false;
        Ptr<Product> DocCAMProduct = DocProducts->itemByProductType("CAMProductType"); if (!DocCAMProduct) { ui->messageBox("THERE ARE NO CAM Operations in the Active Document"); return false; }
        Ptr<CAM> DocCAM = Ptr<CAM>(DocCAMProduct); if (!DocCAM) return false;
        Ptr<Setups> DocCAMSetups = DocCAM->setups(); if (!DocCAMSetups) return false;
        Ptr<Setup> ActiveSetup = DocCAMSetups->item(0); if (!ActiveSetup) return false;

        Ptr<OperationBase> SetupOper = nullptr;
        Ptr<OperationBase> SetupSimulOper = nullptr;
        string SetupOperName;
        string SetupSImulOperName;
        //Ptr<FloatParameterValue> OpBotHeightOffsetParVal = nullptr;
        //double OpBotHeightOffset = 0;
        double OpToolTipOffset = 0;
        Ptr<FloatParameterValue> OpToolTipOffsetParVal = nullptr;
        //for (int j = 0; j < OperWithFormMillToolIndexes.size(); j++)
        for (int j = 0; j < ActiveSetup->operations()->count(); j++)

        {
            SetupOper = ActiveSetup->operations()->item(j);
            if (SetupOper->parameters()->itemByName("tool_type")->expression() != "'form mill'") continue;
            OpToolTipOffsetParVal = (Ptr<FloatParameterValue>)SetupOper->parameters()->itemByName("tool_tipOffset")->value();
            OpToolTipOffset = OpToolTipOffsetParVal->value();//Unit cm

            if (SetupOper->isSuppressed() && (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'") && OpToolTipOffset > 0)
            {
                SetupOper->isSuppressed(false);
                DocCAM->generateToolpath(SetupOper);
            }
            if ((SetupOper->name().find("-SIMUL") != -1) && (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'") && OpToolTipOffset > 0)
                SetupOper->deleteMe();




        }
    }
    catch (std::exception* e)
    {
        if (ui)
        {
            ui->messageBox(std::string(e->what()));

        }
    }

    catch (...)
    {

        ui->messageBox("Generic Exception Thrown");

    }
    return true;
}