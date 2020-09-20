FSInfo fs_info;

long fs_free ()
{
  return fs_info.totalBytes - fs_info.usedBytes;  
}

void fs_setup ()
{
  LittleFS.info(fs_info);
};
