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
        
        
        
    }
    ui->commandTerminated()->add(&onCommandTerminatedHandler_);
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
    
    ui->commandTerminated()->remove(&onCommandTerminatedHandler_); 
    if (ui) ui = nullptr;
    
    return true;
}


int FormMillToolSimulation(bool SimulateWithMachine)
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
        Ptr<Selections> ActiveSelects = ui->activeSelections(); if (!ActiveSelects) return false;
        Ptr<Setup> ActiveSetup = nullptr;
        if (ui->activeSelections()->count() == 0) { ui->messageBox("SELECT SETUP"); return false; }
        if (ActiveSelects && ui->activeSelections()->count() > 0) 
        {Ptr<Base> selectedObject = ui->activeSelections()->item(0)->entity();
         
        if (string(selectedObject->objectType()) == "adsk::cam::Setup") ActiveSetup = (Ptr<Setup>) selectedObject;
        else { ui->messageBox("SELECT SETUP NOT OPERATION"); return false; }
        //ActiveSetup = selectedObject;
        }
        //else { ui->messageBox("SELECT SETUP"); return false;}
        Ptr<OperationBase> SetupOper = nullptr;
        Ptr<OperationBase> SetupSimulOper = nullptr;
        string SetupOperName;
        string SetupSImulOperName;
        double OpBotHeightOffset = 0;
        double OpToolTipOffset = 0;
        Ptr<FloatParameterValue> OpBotHeightOffsetParVal = nullptr;
        Ptr<FloatParameterValue> SimulatOpBotHeightOffsetParVal = nullptr;
        //Ptr<FloatParameterValue> OpToolTipOffsetParVal = nullptr;
        
        Ptr<ProgressDialog> PrgDlg = nullptr;
        
        int SetupOpersNum = (int)ActiveSetup->operations()->count();
        PrgDlg = ui->createProgressDialog();
        PrgDlg->show("GENERATING TEMPORARY OPERATIONS", "%v OPERATIONS (%p%)", 0, SetupOpersNum);
        
        int SetupOpersWithFormMillTool = 0;
        for (int i=0;i< SetupOpersNum;i++)
        {SetupOper = ActiveSetup->operations()->item(i);
         PrgDlg->progressValue(i);
        
        if (SetupOper->parameters()->itemByName("tool_tipOffset") !=nullptr) OpToolTipOffset = ((Ptr<FloatParameterValue>)SetupOper->parameters()->itemByName("tool_tipOffset")->value())->value(); //Unit cm
        if (SetupOper->parameters()->itemByName("bottomHeight_offset") != nullptr)
        {
            OpBotHeightOffsetParVal = (Ptr<FloatParameterValue>)SetupOper->parameters()->itemByName("bottomHeight_offset")->value();
            OpBotHeightOffset = OpBotHeightOffsetParVal->value();//Unit cm

        }
        if (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'" && OpToolTipOffset>0)
        {if (!SetupOper->duplicate()) return false;
         SetupOpersNum++;
         OperIdsWithFormMillTool.push_back(SetupOper->operationId());
         SetupOpersWithFormMillTool++;
         SetupSimulOper = ActiveSetup->operations()->item(i + 1);
         TemporarySimulatOperIds.push_back(SetupSimulOper->operationId());
         SetupSimulOper->name(SetupOper->name() + "-SIMUL");
         SimulatOpBotHeightOffsetParVal = (Ptr<FloatParameterValue>)SetupSimulOper->parameters()->itemByName("bottomHeight_offset")->value();
         SimulatOpBotHeightOffsetParVal->value(OpBotHeightOffset - OpToolTipOffset); //Unit cm
         //SetupSimulOper->parameters()->itemByName("bottomHeight_offset")->expression()
         GenToolPathChecker=DocCAM->generateToolpath(SetupSimulOper);
         SetupOper->isSuppressed(true);
         i++;
        
        }
        }
        PrgDlg->hide();
        if (SimulateWithMachine==false)
        {
        Ptr<CommandDefinition> simCmd = ui->commandDefinitions()->itemById("IronSimulation");
        if (simCmd)
        {
            simCmd->execute();
            
        }
        }
        if (SimulateWithMachine == true)
        {
            Ptr<CommandDefinition> simCmd = ui->commandDefinitions()->itemById("IronMachineSimulation");
            if (simCmd)
            {
                simCmd->execute();
                
            }
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
        Ptr<Selections> ActiveSelects = ui->activeSelections(); if (!ActiveSelects) return false;
        Ptr<Setup> ActiveSetup = nullptr;
        if (ActiveSelects && ui->activeSelections()->count() > 0)
        {
            Ptr<Base> selectedObject = ui->activeSelections()->item(0)->entity();
            if (string(selectedObject->objectType())=="adsk::cam::Setup") ActiveSetup = (Ptr<Setup>) selectedObject;
            else {ui->messageBox("SELECT SETUP NOT OPERATION"); return false;}
        }
        

        Ptr<OperationBase> SetupOper = nullptr;
        Ptr<OperationBase> SetupSimulOper = nullptr;
        string SetupOperName;
        string SetupSImulOperName;
        
        
        Ptr<FloatParameterValue> OpToolTipOffsetParVal = nullptr;
        int OperId = 0;
        int SimulOperId = 0;
        bool IsRestored = false;
        
        
        for (int j = 0; j < OperIdsWithFormMillTool.size(); j++)
        {   OperId = OperIdsWithFormMillTool.at(j);
            SetupOper = ActiveSetup->operations()->itemByOperationId(OperId);
           IsRestored = SetupOper->isSuppressed(false);
           GenToolPathChecker = DocCAM->generateToolpath(SetupOper);
            if (IsRestored != true)
            {ui->messageBox("ERROR Restoring Operation " + SetupOper->name());
             
            }
        }
        for (int k = 0; k < TemporarySimulatOperIds.size(); k++)
        {
            SimulOperId = TemporarySimulatOperIds.at(k);
            SetupSimulOper = ActiveSetup->operations()->itemByOperationId(SimulOperId);
            if (SetupSimulOper->deleteMe() != true) ui->messageBox("ERROR DELETING Operation " + SetupSimulOper->name());
        }
            
            
                OperIdsWithFormMillTool.clear();
                TemporarySimulatOperIds.clear();



        
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