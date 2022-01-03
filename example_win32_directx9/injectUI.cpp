#include "injectUI.h"
#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <string>
#include <requests.h>
using namespace requests;




DWORD GetGamePID() {
  
    HWND hwGame = FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9");
    if (!hwGame) return 0;
    DWORD ret = 0;
    GetWindowThreadProcessId(hwGame, &ret);

        return ret;
}

void InjectDll(const char* path, DWORD pid) {

  

    HANDLE hGame = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
    char dllpath[MAX_PATH];
    ZeroMemory(dllpath, sizeof(dllpath));
    strcpy_s(dllpath, path);
    LPVOID allocatedMem = VirtualAllocEx(hGame, NULL, sizeof(dllpath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hGame, allocatedMem, dllpath, sizeof(dllpath), NULL);
    HANDLE hThread = CreateRemoteThread(hGame, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocatedMem, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hGame, allocatedMem, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

std::string Lpcwstr2String(LPCWSTR lps) {

        int len = WideCharToMultiByte(CP_ACP, 0, lps, -1, NULL, 0, NULL, NULL);
    if (len <= 0) {
        return "";
    }
    else {
        char* dest = new char[len];
        WideCharToMultiByte(CP_ACP, 0, lps, -1, dest, len, NULL, NULL);
        dest[len - 1] = 0;
        std::string str(dest);
        delete[] dest;
      
            return str;
    }
}
//Response resps = Get("http://localhost:801/loader.exe");


std::string SelectDll() {


        OPENFILENAME ofn;
    char szFile[300];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = (LPWSTR)szFile;
    ofn.lpstrFile[0] = '\0';
    LPTSTR        lpstrCustomFilter;
    DWORD         nMaxCustFilter;
    ofn.nFilterIndex = 1;
    LPTSTR        lpstrFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"DLL File\0*.dll";
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    std::string path_image = "";
    if (GetOpenFileName(&ofn)) {
        path_image = Lpcwstr2String(ofn.lpstrFile);
        return path_image;
    }

    else {
        return "";
    }
   
}




void injectUIs(bool* open)
{
    if (open)
    {





    ImGui::Begin("injectUI", open);
   

    if (ImGui::Button("inject"))
    {
        DWORD gamePID = GetGamePID();
        std::string dllpath = SelectDll();
    //    resps.GetBinary();
        InjectDll(dllpath.c_str(), gamePID);
    }

    ImGui::End();
    }

  
}
