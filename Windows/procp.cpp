#include <windows.h>
#include <iostream>

using namespace std;

const int BUFFER = 4096;

void copyFile(char source[MAX_PATH], char target[MAX_PATH])
{
  WIN32_FIND_DATAA lpData;
  HANDLE fFind = FindFirstFileA(source, &lpData);
  HANDLE hSource = CreateFileA(source, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  HANDLE hTarget = CreateFileA(target, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  long fileSize = lpData.nFileSizeLow - lpData.nFileSizeHigh;

  DWORD wordbit;

  int *buffer = new int[BUFFER];

  ReadFile(hSource, buffer, fileSize, &wordbit, NULL);
  WriteFile(hTarget, buffer, fileSize, &wordbit, NULL);

  CloseHandle(fFind);
  CloseHandle(hSource);
  CloseHandle(hTarget);
}

void copyDirectory(char sourceDir[MAX_PATH], char targetDir[MAX_PATH])
{
  WIN32_FIND_DATAA lpFindDirData;

  char source[BUFFER];
  char target[BUFFER];

  strcpy(source, sourceDir);
  strcpy(target, targetDir);
  strcat(source, "\\*.*");
  strcat(target, "\\");
  cout << source << endl;
  cout << target << endl;

  HANDLE sourceHandle = FindFirstFileA(source, &lpFindDirData);
  if (sourceHandle != INVALID_HANDLE_VALUE)
  {
    cout << "[INFO] Source directory <" << source << "> found!" << endl;

    while (FindNextFileA(sourceHandle, &lpFindDirData) != 0)
    {
      // 文件夹 -> 继续寻找下一级文件
      if (lpFindDirData.dwFileAttributes == 16)
      {
        if ((strcmp(lpFindDirData.cFileName, ".") != 0) && (strcmp(lpFindDirData.cFileName, "..") != 0))
        {
          memset(source, 0, sizeof(source));
          strcpy(source, sourceDir);
          strcat(source, "\\");
          strcat(source, lpFindDirData.cFileName);
          strcat(target, lpFindDirData.cFileName);

          CreateDirectoryA(target, NULL);
          copyDirectory(source, target);

          strcpy(source, sourceDir);
          strcat(source, "\\");
          strcpy(target, targetDir);
          strcat(target, "\\");
        }
      }
      // 文件 -> 复制！
      else
      {
        memset(source, 0, sizeof(source));
        strcpy(source, sourceDir);
        strcat(source, "\\");
        strcat(source, lpFindDirData.cFileName);
        strcat(target, lpFindDirData.cFileName);

        copyFile(source, target);

        strcpy(source, sourceDir);
        strcat(source, "\\");
        strcpy(target, targetDir);
        strcat(target, "\\");
      }
    }
  }
  else
  {
    cerr << "[ERR] Directory <" << sourceDir << "> not found." << endl;
  }
}

int main(int argc, char const *argv[])
{
  // 参数判断
  if (argc != 3)
  {
    cout << "[INFO] Usage: procp.exe <Original Directory> <Target Directory>" << endl;
    return -1;
  }

  char szSourceDir[MAX_PATH];
  char szTargetDir[MAX_PATH];

  WIN32_FIND_DATAA lpFindData;

  // 第一个参数：源文件夹
  sprintf(szSourceDir, "%s", argv[1]);
  // 第二个参数：目标文件夹
  sprintf(szTargetDir, "%s", argv[2]);

  // 判断源文件夹是否存在
  HANDLE sourceDirHandle = FindFirstFileA(szSourceDir, &lpFindData);
  if (sourceDirHandle == INVALID_HANDLE_VALUE)
  {
    cout << "[ERR] Source directory doesn't exist." << endl;
    return -1;
  }

  // 是否创建目标文件夹
  HANDLE targetDirHandle = FindFirstFileA(szTargetDir, &lpFindData);
  if (targetDirHandle == INVALID_HANDLE_VALUE)
  {
    CreateDirectoryA(szTargetDir, NULL);
  }

  // 复制文件和文件夹
  copyDirectory(szSourceDir, szTargetDir);

  return 0;
}
