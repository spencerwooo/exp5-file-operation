#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <string.h>

using namespace std;

const int BUFFER = 4096;
const int MAX_PATH = 260;

void copyFile(char sourceDir[MAX_PATH], char targetDir[MAX_PATH])
{
  int fsourceData = open(sourceDir, O_RDONLY);
  if (fsourceData == -1)
  {
    cerr << "[ERR] Open file failed." << endl;
  }

  struct stat statbuf;
  struct utimbuf timebuf;
  char buffer[BUFFER];
  int wordbit;

  stat(sourceDir, &statbuf);

  int ftargetData = creat(targetDir, statbuf.st_mode);
  if (ftargetData == -1)
  {
    cerr << "[ERR] Create file failed." << endl;
  }

  while ((wordbit = read(fsourceData, buffer, BUFFER)) > 0)
  {
    if (write(ftargetData, buffer, wordbit) != wordbit)
    {
      cerr << "[ERR] Write file failed." << endl;
    }

    timebuf.actime = statbuf.st_atime;
    timebuf.modtime = statbuf.st_mtime;
    close(fsourceData);
    close(ftargetData);
  }
}

void copyDirectory(char sourceDir[MAX_PATH], char targetDir[MAX_PATH])
{
  char source[MAX_PATH];
  char target[MAX_PATH];

  struct stat statbuf;
  struct utimbuf timebuf;
  struct dirent *entry;
  DIR *dir;

  strcpy(source, sourceDir);
  strcpy(target, targetDir);

  dir = opendir(source);

  while ((entry = readdir(dir)) != NULL)
  {
    // 当前目录 "." 上级目录 ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    {
      continue;
    }
    // 文件夹！
    if (entry->d_type == 4)
    {
      strcat(source, "/");
      strcat(source, entry->d_name);
      strcat(target, "/");
      strcat(target, entry->d_name);

      stat(source, &statbuf);

      mkdir(target, statbuf.st_mode);
      timebuf.actime = statbuf.st_atime;
      timebuf.modtime = statbuf.st_mtime;

      utime(target, &timebuf);

      copyDirectory(source, target);

      strcpy(source, sourceDir);
      strcpy(target, targetDir);
    }
    // 不是目录，是文件
    else
    {
      strcat(source, "/");
      strcat(source, entry->d_name);
      strcat(target, "/");
      strcat(target, entry->d_name);

      copyFile(source, target);
      strcpy(source, sourceDir);
      strcpy(target, targetDir);
    }
  }
}

int main(int argc, char const *argv[])
{
  if (argc != 3)
  {
    cout << "[INFO] Usage: ./procp <Original Directory> <Target Directory>" << endl;
    return -1;
  }

  DIR *dir;
  struct stat statbuf;
  struct utimbuf timebuf;

  char source[MAX_PATH];
  char target[MAX_PATH];

  sprintf(source, "%s", argv[1]);
  sprintf(target, "%s", argv[2]);

  // 检测源文件是否存在
  if ((dir = opendir(source)) == NULL)
  {
    cout << "[ERR] Source directory not found!" << endl;
  }

  // 创建目标文件（如果没有目标文件）
  if ((dir = opendir(target)) == NULL)
  {
    // 将源文件的属性存入 statbuf
    stat(source, &statbuf);
    // 建立目标文件目录
    mkdir(target, statbuf.st_mode);
    // 记录文件存取和修改的时间
    timebuf.actime = statbuf.st_atime;
    timebuf.modtime = statbuf.st_mtime;
    // 记录目标文件的存取、修改时间
    utime(target, &timebuf);
  }

  // 开始复制！
  copyDirectory(source, target);

  return 0;
}
