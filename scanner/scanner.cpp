
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

DWORD FindPattern(BYTE* data, DWORD size, const char* pattern, const char* mask)
{
    DWORD patternLen = strlen(mask);
    for (DWORD i = 0; i < size - patternLen; i++)
    {
        BOOL match = TRUE;
        for (DWORD j = 0; j < patternLen; j++)
        {
            if (mask[j] != '?' && data[i+j] != (BYTE)pattern[j])
            {
                match = FALSE;
                break;
            }
        }
        if (match)
            return i;
    }
    return 0;
}

BYTE* LoadFile(const char* path, DWORD& outSize)
{
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("无法打开文件: %s\n", path);
        return NULL;
    }
    outSize = GetFileSize(hFile, NULL);
    BYTE* buffer = (BYTE*)malloc(outSize);
    DWORD read = 0;
    ReadFile(hFile, buffer, outSize, &read, NULL);
    CloseHandle(hFile);
    return buffer;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("用法: scanner.exe GameServer.exe\n");
        return 0;
    }

    DWORD size = 0;
    BYTE* data = LoadFile(argv[1], size);
    if (!data) return 0;

    printf("已加载 GameServer.exe, 大小: %u bytes\n", size);

    const char sig[]  = "\x55\x8B\xEC\x83\xEC";
    const char mask[] = "xxxxx";

    DWORD offset = FindPattern(data, size, sig, mask);
    if (offset == 0)
    {
        printf("未找到 entity_PVP_GetRelation 的特征码！\n");
    }
    else
    {
        printf("=====================================\n");
        printf(" 找到 entity_PVP_GetRelation 函数!\n");
        printf("=====================================\n");
        printf("文件偏移: 0x%08X\n", offset);
        printf("PVP Relation Offset = 0x%08X\n", offset);
    }

    free(data);
    return 0;
}
