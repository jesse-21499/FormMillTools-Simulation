#pragma once
//#include <Core/CoreAll.h>

#include <Core/Application/Products.h>
#include <Fusion/FusionAll.h>
#include <CAM/CamAll.h>

#include <Core/UserInterface/CommandEventHandler.h>
#include <Core/UserInterface/Command.h>
#include <Core/UserInterface/CommandInputs.h>
#include <Core/UserInterface/BoolValueCommandInput.h>
#include <Core/UserInterface/CommandEvent.h>
#include <Core/Application/Events.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/Selection.h>
#include <Core/UserInterface/Selections.h>
#include <Core/UserInterface/CommandCreatedEvent.h>
#include <Core/UserInterface/CommandCreatedEventHandler.h>

#include <Core/UserInterface/ApplicationCommandEvent.h>
#include <Core/UserInterface/ApplicationCommandEventHandler.h>
#include <Core/UserInterface/ApplicationCommandEventArgs.h>
#include <Core/UserInterface/CommandCreatedEventArgs.h>

#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandControl.h>
#include <Core/Application/Application.h>
#include <Core/UserInterface/Toolbar.h>
#include <Core/UserInterface/Toolbars.h>
#include <Core/UserInterface/ToolbarPanel.h>
#include <Core/UserInterface/ToolbarPanels.h>
#include <Core/UserInterface/ToolbarControl.h>
#include <Core/UserInterface/ToolbarControls.h>
#include <Core/UserInterface/Workspace.h>
#include <Core/UserInterface/Workspaces.h>
#include <Core/UserInterface/Palette.h>
#include <Core/UserInterface/Palettes.h>
#include <Core/UserInterface/TextCommandPalette.h>
#include <Core/UserInterface/ProgressDialog.h>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;
//using namespace adsk::drawing;
using namespace std;

Ptr<Application> app;
Ptr<UserInterface> ui;

Ptr<GenerateToolpathFuture> GenToolPathChecker = nullptr;
int FormMillToolSimulation(bool SimulateWithMachine);
int CleanSimulatOperations();

class CommandTerminatedHandler : public adsk::core::ApplicationCommandEventHandler
{public:
    void notify(const Ptr<ApplicationCommandEventArgs>& eventArgs) override
    {if (eventArgs->commandId() == "FormMillToolsSimulation")
        { if   (GenToolPathChecker!=nullptr)
    {
          bool RegenIsFinished = GenToolPathChecker->isGenerationCompleted();
            while (!RegenIsFinished)
            {_sleep(500);
             RegenIsFinished = GenToolPathChecker->isGenerationCompleted();
            }
    }
          
        }
        // This fires when simulation's panel closes
        if (eventArgs->commandId() == "IronSimulation" || eventArgs->commandId() == "IronMachineSimulation")
        {
            CleanSimulatOperations();
        }
    }
};

class CommandExecutedHandler : public adsk::core::CommandEventHandler
{public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override
    {Ptr<Event> firingEvent = eventArgs->firingEvent(); if (!firingEvent)  return;
     Ptr<Command> command = firingEvent->sender(); if (!command) return;
     Ptr<CommandDefinition> parentDefinition = command->parentCommandDefinition(); if (!parentDefinition) return;
     if (parentDefinition->id() == "FormMillToolsSimulation")
     {  Ptr<Command> command = eventArgs->firingEvent()->sender(); if (!command) return;
         Ptr<CommandInputs> inputs = command->commandInputs();
         Ptr<BoolValueCommandInput> SimulateWithMachine = inputs->itemById("SimulWithMachineId");
         if (SimulateWithMachine->value() == false)
         {FormMillToolSimulation(false);
         }
         if (SimulateWithMachine->value() == true)
         {FormMillToolSimulation(true);
         }
         
     }
         
      
    }

    
} onCommandExecuted;
class CommandCreatedHandler : public CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
    {//Ptr<Command> command = eventArgs->command(); if (!command) return;
        // Define the inputs.
        //Ptr<CommandInputs> inputs = command->commandInputs();
        //if (inputs)
        //{if (command->parentCommandDefinition()->id() == "FormMillToolsSimulation") FormMillToolSimulation();
        //}
    }

    
} onCommandCreatedOnPanel;
class CommandCreatedOnQATHandler : public adsk::core::CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
    {
        if (eventArgs)
        {
            Ptr<Command> command = eventArgs->command();if (!command) return;
            Ptr<CommandEvent> exec = command->execute();if (!exec) return;
            exec->add(&onCommandExecuted_);
            Ptr<CommandInputs> inputs = command->commandInputs();
            Ptr<BoolValueCommandInput> SimulateWithMachine = inputs->addBoolValueInput("SimulWithMachineId", "SIMULATE WITH MACHINE", true, "", false);

             
        }
    }

private:
    
    CommandExecutedHandler onCommandExecuted_;
    

} onCommandCreatedOnQAT;
static CommandTerminatedHandler onCommandTerminatedHandler_;
static std::vector<int> OperIdsWithFormMillTool;
static std::vector<int> TemporarySimulatOperIds;
