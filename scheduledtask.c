
#include <windows.h>
#include <stdio.h>
#include <taskschd.h>
#include <combaseapi.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

void CreateScheduledTask()
{
    HRESULT hr;

    // Initialize COM
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf("CoInitializeEx failed: 0x%X
", hr); // Corrected newline escape
        return;
    }

    // Create the Task Service
    ITaskService *pService = NULL;
    hr = CoCreateInstance(&CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskService, (void**)&pService);
    if (FAILED(hr))
    {
        printf("CoCreateInstance failed: 0x%X
", hr); // Corrected newline escape
        CoUninitialize();
        return;
    }

    // Connect to the Task Scheduler service
    hr = pService->lpVtbl->Connect(pService, NULL, NULL, NULL, NULL);
    if (FAILED(hr))
    {
        printf("Connect failed: 0x%X
", hr); // Corrected newline escape
        pService->lpVtbl->Release(pService);
        CoUninitialize();
        return;
    }

    // Create a new task definition
    ITaskDefinition *pTask = NULL;
    hr = pService->lpVtbl->NewTask(pService, 0, &pTask);
    if (FAILED(hr))
    {
        printf("NewTask failed: 0x%X
", hr); // Corrected newline escape
        pService->lpVtbl->Release(pService);
        CoUninitialize();
        return;
    }

    // Get the registration info for the task
    IRegistrationInfo *pRegInfo = NULL;
    hr = pTask->lpVtbl->get_RegistrationInfo(pTask, &pRegInfo);
    if (SUCCEEDED(hr))
    {
        // Set the task's description
        pRegInfo->lpVtbl->put_Description(pRegInfo, L"Example Scheduled Task to Run a Program");
        pRegInfo->lpVtbl->Release(pRegInfo);
    }

    // Get the principal for the task
    IPrincipal *pPrincipal = NULL;
    hr = pTask->lpVtbl->get_Principal(pTask, &pPrincipal);
    if (SUCCEEDED(hr))
    {
        // Set the user to run the task
        pPrincipal->lpVtbl->put_UserId(pPrincipal, L"S-1-5-18"); // Local System account
        pPrincipal->lpVtbl->put_LogonType(pPrincipal, LogonType_ServiceAccount);
        pPrincipal->lpVtbl->Release(pPrincipal);
    }

    // Create a trigger to run at logon
    ITriggerCollection *pTriggerCollection = NULL;
    hr = pTask->lpVtbl->get_Triggers(pTask, &pTriggerCollection);
    if (SUCCEEDED(hr))
    {
        ITrigger *pTrigger = NULL;
        hr = pTriggerCollection->lpVtbl->Create(pTriggerCollection, TriggerType_Logon, &pTrigger);
        if (SUCCEEDED(hr))
        {
            // Release the trigger after use
            pTrigger->lpVtbl->Release(pTrigger);
        }
        pTriggerCollection->lpVtbl->Release(pTriggerCollection);
    }

    // Get the action to run a program
    IActionCollection *pActionCollection = NULL;
    hr = pTask->lpVtbl->get_Actions(pTask, &pActionCollection);
    if (SUCCEEDED(hr))
    {
        IAction *pAction = NULL;
        hr = pActionCollection->lpVtbl->Create(pActionCollection, ActionType_Exec, &pAction);
        if (SUCCEEDED(hr))
        {
            IExecAction *pExecAction = NULL;
            hr = pAction->lpVtbl->QueryInterface(pAction, &IID_IExecAction, (void**)&pExecAction);
            if (SUCCEEDED(hr))
            {
                // Set the path to the executable
                hr = pExecAction->lpVtbl->put_Path(pExecAction, L"C:\\Path\\To\\Your\\Executable.exe");
                if (FAILED(hr))
                {
                    printf("put_Path failed: 0x%X
", hr); // Handle error
                }
                // Release the ExecAction
                pExecAction->lpVtbl->Release(pExecAction);
            }
            // Release the action
            pAction->lpVtbl->Release(pAction);
        }
        pActionCollection->lpVtbl->Release(pActionCollection);
    }

    // Register the task
    IFolder *pRootFolder = NULL; // Use IFolder instead of IRegisteredTask
    hr = pService->lpVtbl->GetFolder(pService, L"\\", &pRootFolder);
    if (SUCCEEDED(hr))
    {
        hr = pRootFolder->lpVtbl->RegisterTaskDefinition(pRootFolder, L"ExampleTask", pTask, 
            TASK_CREATE_OR_UPDATE, NULL, NULL, LogonType_ServiceAccount, NULL, NULL); // Adjusted parameters
        if (FAILED(hr))
        {
            printf("RegisterTaskDefinition failed: 0x%X
", hr); // Corrected newline escape
        }
        pRootFolder->lpVtbl->Release(pRootFolder); // Release the folder
    }

    // Release the task and service
    pTask->lpVtbl->Release(pTask);
    pService->lpVtbl->Release(pService);
    CoUninitialize();
}

int main()
{
    CreateScheduledTask();
    return 0;
}
