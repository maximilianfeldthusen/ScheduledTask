
## Documentation

### ScheduledTask

The provided code is a C program that creates a scheduled task in Windows using the Task Scheduler API. Below is a detailed explanation of the code's structure and functionality:

### Header Files
```c
#include <windows.h>
#include <stdio.h>
#include <taskschd.h>
#include <combaseapi.h>
```
- **windows.h**: Provides declarations for the Windows API functions and types.
- **stdio.h**: For input and output functions (e.g., `printf`).
- **taskschd.h**: Contains the definitions for the Task Scheduler API.
- **combaseapi.h**: Provides interfaces for COM (Component Object Model) operations.

### Linking Libraries
```c
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
```
- These lines link the necessary libraries for using the Task Scheduler API and COM support.

### Function to Create a Scheduled Task
```c
void CreateScheduledTask()
```
This function contains the core logic to create the scheduled task.

1. **Initialize COM**:
    ```c
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ```
   - Initializes the COM library for use by the calling thread. It sets the concurrency model to multi-threaded.

2. **Create the Task Service**:
    ```c
    hr = CoCreateInstance(&CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskService, (void**)&pService);
    ```
   - This line creates an instance of the Task Scheduler service.

3. **Connect to the Task Scheduler Service**:
    ```c
    hr = pService->lpVtbl->Connect(pService, NULL, NULL, NULL, NULL);
    ```
   - Establishes a connection to the Task Scheduler service.

4. **Create a New Task Definition**:
    ```c
    hr = pService->lpVtbl->NewTask(pService, 0, &pTask);
    ```
   - Creates a new task definition object.

5. **Set Task Registration Info**:
    ```c
    hr = pTask->lpVtbl->get_RegistrationInfo(pTask, &pRegInfo);
    ```
   - Retrieves the registration information for the task, which is used to set properties like the task description.

6. **Set Task Principal**:
    ```c
    hr = pTask->lpVtbl->get_Principal(pTask, &pPrincipal);
    ```
   - Retrieves the principal object related to the task, which is used to define the security context under which the task runs (e.g., as the local system account).

7. **Create a Trigger**:
    ```c
    hr = pTriggerCollection->lpVtbl->Create(pTriggerCollection, TriggerType_Logon, &pTrigger);
    ```
   - Adds a trigger that specifies when the task should run (in this case, at user logon).

8. **Set Task Action**:
    ```c
    hr = pActionCollection->lpVtbl->Create(pActionCollection, ActionType_Exec, &pAction);
    ```
   - Defines an action that the scheduled task will perform, such as executing a specified program.

9. **Register the Task**:
    ```c
    hr = pRootFolder->lpVtbl->RegisterTaskDefinition(pRootFolder, L"ExampleTask", pTask, TASK_CREATE_OR_UPDATE, NULL, NULL, LogonType_ServiceAccount, NULL, NULL);
    ```
   - Registers the task under the root folder of the Task Scheduler.

10. **Cleanup**:
    The function releases all COM objects and uninitializes COM to free resources before returning.

### Main Function
```c
int main()
{
    CreateScheduledTask();
    return 0;
}
```
- Calls the `CreateScheduledTask` function to execute the task creation process when the program runs.

### Summary
The code sets up a scheduled task in Windows that runs a specified executable whenever a user logs on. It utilizes the Windows Task Scheduler API and COM interfaces to achieve this. Error handling is performed at various steps to ensure that the program can gracefully report issues without crashing. The task is defined to run as the Local System account, which provides sufficient privileges for many tasks.

