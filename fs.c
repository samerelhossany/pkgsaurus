static struct inode*
namex(char *path, int nameiparent, char *name)
{
  struct inode *ip, *next;
  if (*path == '/')
    ip = iget(ROOTDEV, ROOTINO); // start from root
  else
    ip = idup(myproc()->cwd); // current dir

  while ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    if (ip->type != T_DIR) {
      iunlockput(ip);
      return 0;
    }
    if (nameiparent && *path == '\0') {
      // stop one before last
      iunlock(ip);
      return ip;
    }
    if ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      return 0;
    }
    iunlockput(ip);
    ip = next;
  }
  if (nameiparent) {
    iput(ip);
    return 0;
  }
  return ip;
}
knamex(char *path, int nameiparent, char *name)
{
  struct inode *ip, *next;
  if (*path == '/')
    ip = iget(ROOTDEV, ROOTINO); // start from root
  else
    ip = idup(myproc()->cwd); // current dir

  while ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    if (ip->type != T_DIR) {
      iunlockput(ip);
      return 0;
    }
    if (nameiparent && *path == '\0') {
      // stop one before last
      iunlock(ip);
      return ip;
    }
    if ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      return 0;
    }
    iunlockput(ip);
    ip = next;
  }
  if (nameiparent) {
    iput(ip);
    return 0;
  }
  return ip;
}
