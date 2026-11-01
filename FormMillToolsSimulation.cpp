#pragma once

#include "FormMillToolsSimulation.h"
#include <chrono>
#include <thread>

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
        
        ////////////////////////////////////////////////////// CAM Environment //////////////////////////////////////////////////////
        
              
        
        Ptr<Document> ActiveDoc = app->activeDocument(); if (!ActiveDoc) return false;
        Ptr<Workspace> ManufEnvirWorkSpace = ui->workspaces()->itemById("CAMEnvironment"); if (!ManufEnvirWorkSpace) return false;
        ManufEnvirWorkSpace->activate();
        Ptr<TextCommandPalette> TxtCmdPalette = ui->palettes()->itemById("TextCommands"); if (!TxtCmdPalette) return false;
        Ptr<Products> DocProducts = ActiveDoc->products(); if (!DocProducts) return false;
        Ptr<Product> DocCAMProduct = DocProducts->itemByProductType("CAMProductType"); if (!DocCAMProduct) { ui->messageBox("THERE ARE NO CAM Operations in the Active Document"); return false; }
        Ptr<CAM> DocCAM = Ptr<CAM>(DocCAMProduct); if (!DocCAM) return false;
        Ptr<Setups> DocCAMSetups = DocCAM->setups(); if (!DocCAMSetups) return false;
        Ptr<Setup> ActiveSetup = DocCAMSetups->item(0); if (!ActiveSetup) return false;
        
        Ptr<Occurrence> CAMDesignOccurr = DocCAM->designRootOccurrence(); if (!CAMDesignOccurr) return false;
        
        Ptr<ManufacturingModels> ManufModels = DocCAM->manufacturingModels(); if (!ManufModels) return false;
        
        Ptr<ManufacturingModel> ManufModel = ManufModels->item(0); if (!ManufModel) return false;
        
        Ptr<Occurrence> ManufModelOccurr = ManufModel->occurrence()->childOccurrences()->item(0); if (!ManufModelOccurr) return false;
        
        string ocurrname = ManufModelOccurr->name();
        Ptr<BRepBody> StockBody = ManufModelOccurr->bRepBodies()->itemByName("BRUTE"); if (!StockBody) return false;
        Ptr<BRepBody> MachinedBody = ManufModelOccurr->bRepBodies()->itemByName("MACHINED"); if (!MachinedBody) return false;
        std::vector<Ptr<Base>> StockBodyList = {StockBody };
        std::vector<Ptr<Base>> MachinedBodyList = {MachinedBody };
        
        Ptr<CAMManager> CamMgr = CAMManager::get();
        Ptr<CAMLibraryManager> CamLibMgr = CamMgr->libraryManager();
        Ptr<MachineLibrary> MachineLib = CamLibMgr->machineLibrary();
        Ptr<URL> MachineLibraryURL = MachineLib->urlByLocation(LibraryLocations::CloudLibraryLocation);
        
        std::vector<Ptr<Machine>> Machines = MachineLib->childMachines(MachineLibraryURL);
        string MachineModel;
        
        
                
        Ptr<OperationBase> SetupOper = nullptr;
        Ptr<OperationBase> SetupSimulOper = nullptr;
        string SetupOperName;
        string SetupSImulOperName;
        for (int i=0;i< ActiveSetup->operations()->count();i++)
        {SetupOper = ActiveSetup->operations()->item(i);
         
         if ((SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'") && (stoi(SetupOper->parameters()->itemByName("tool_tipOffset")->expression()) > 0) && (stod(SetupOper->parameters()->itemByName("bottomHeight_offset")->expression())>0) )
        {if (!SetupOper->duplicate()) return false;
         SetupSimulOper = ActiveSetup->operations()->item(i + 1);
         SetupSimulOper->name(SetupOper->name() + "-SIMUL");
         SetupSimulOper->parameters()->itemByName("bottomHeight_offset")->expression("0");
         DocCAM->generateToolpath(SetupSimulOper);
         SetupOper->isSuppressed(true);
         DocCAM->generateToolpath(SetupOper);
         
        }
        }
        
        ui->activeSelections()->add(ActiveSetup);
            
        
        
        
        
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
        for (int j = 0; j < ActiveSetup->operations()->count(); j++)
        {
            SetupOper = ActiveSetup->operations()->item(j);
            if (SetupOper->isSuppressed() && (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'") && (stoi(SetupOper->parameters()->itemByName("tool_tipOffset")->expression()) > 0) && (stod(SetupOper->parameters()->itemByName("bottomHeight_offset")->expression()) > 0))
                SetupOper->isSuppressed(false);
            DocCAM->generateToolpath(SetupOper);
            if ((SetupOper->name().find("-SIMUL") != -1) && (SetupOper->parameters()->itemByName("tool_type")->expression() == "'form mill'") && (stoi(SetupOper->parameters()->itemByName("bottomHeight_offset")->expression()) == 0))
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