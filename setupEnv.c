
// provides the basic types definitions used across xv6 kernel like int, unsigned int, long and datatypes.
#include "kernel/types.h"
//define the metadata like size, type, device, inode number...etc
#include "kernel/stat.h"
//contain the system calls
#include "user/user.h"

#include "kernel/fcntl.h"

int main()
{
//start with the registry file that will hold the package names
int fd = open("registryFile.txt", O_CREATE|O_WRONLY);
reg_file_content="package1.pkg|1.0|A dummy package for testing\npackage2.pkg|1.0|Another dummy package for testing";
if(fd <0)
{
printf("Failed to create file!!!\n");
}
else{
write(fd,reg_file_content,strlen(reg_file_content));
close(fd);
printf("registry file has been created with name \"registryFile.txt\"\n");
}
//
return 0;
}
