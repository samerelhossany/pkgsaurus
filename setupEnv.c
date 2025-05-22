
// provides the basic types definitions used across xv6 kernel like int, unsigned int, long and datatypes.
#include "kernel/types.h"
//define the metadata like size, type, device, inode number...etc
#include "kernel/stat.h"
//contain the system calls
#include "user/user.h"


#include "kernel/fcntl.h"

int create_executable(char* exe_init,char* name, char* version, char* description )
{

    int fdreg=open("registryFile.txt",O_RDWR);
    if(fdreg <0)//check if the registry file is not found
    {
    printf("\033[31;1mERROR IN \033[3mcreate_package\033[1m: \033[4mregistery file is not found\033[0m");
    return -1;
    }







if(name[0] =='\0')
{
printf("\033[31;1mERROR in creating executable: executable must have a name!!\033[0m\n");
return -1;
}
if(version[0] =='\0')
{
printf("\033[31;1mERROR in creating executable: package must have a default version!!\033[0m\n");
return -1;
}
if(description[0] =='\0')
{
description ="No description provided\n";
printf("\033[1;32mNote: No description is provided\033[0m\n");
}

printf("\033[32;1mCreating the %s executable\033[0m\n",name);
char* file_args[]= {name,0};
if(fork() ==0){
exec(exe_init,file_args);//here the program will be created: Samer
printf("the installation of %s program failed\n",name);

}else
{
//exit(0);
wait(0);

}
printf("\033[35mupdating data int the registry file....\033[0m\n");
char regbuffer[1024];
char temp_buffer[1024];
int pos=0;
int temp_pos=0;

int n;
  while ((n = read(fdreg, temp_buffer + temp_pos, sizeof(temp_buffer) - temp_pos)) > 0) {
        temp_pos += n;
    }

memmove(regbuffer + pos, "\n", 1);
pos += 1;
memmove(regbuffer+pos,name,strlen(name));
pos+=strlen(name);//1+strlen(name)
memmove(regbuffer+pos,"|",1);
pos+=1;//1+strlen(name)+1
memmove(regbuffer+pos,version,strlen(version));
pos+=strlen(version);
memmove(regbuffer+pos,"|",1);
pos+=1;
memmove(regbuffer+pos,description,strlen(description));
pos+=strlen(description);
memmove(regbuffer + pos, "\n", 1);
pos += 1;
//write(fdreg, temp_buffer, temp_pos);
write(fdreg,regbuffer,pos);
close(fdreg);
return fdreg;

}

//create package in the server.
int create_package(char* name,char* version,char*description,char*content)
{
int fdreg=open("registryFile.txt",O_RDWR);
if(fdreg <0)//check if the registry file is not found
{
printf("\033[31;1mERROR IN \033[3mcreate_package\033[1m: \033[4mregistery file is not found\033[0m");
return -1;
}



if(content[0] == '\0')
{
content ="No content provided...!!!\n";
}
//no strcat and snprintf so do not try this approach, Samer
char path[128] = "server/";

memmove(path + strlen("server/"), name, strlen(name));
path[strlen("server/") + strlen(name)] = '\0';


int fd = open(path,O_CREATE|O_RDWR);
if(fd<0)
{
printf("\033[31;1mERROR in creating package with name %s\033[0m\n",name);
return -1;
}
if(name[0] =='\0')
{
printf("\033[31;1mERROR in creating package: package must have a name!!\033[0m\n");
return -1;
}
if(version[0] =='\0')
{
printf("\033[31;1mERROR in creating package: package must have a default version!!\033[0m\n");
return -1;
}
if(description[0] =='\0')
{
description ="No description provided";
printf("\033[1;32mNote: No description is provided\033[0m\n");
}
if(content[0] =='\0')
{
printf("\033[34;1mNote: No content is provided!!!\033[0m\n");

}else
{
printf("\033[33mcontent with length of \033[1m%d\033[0;33m has been added into the package \033[1m%s\033[0m\n",strlen(content),name);
write(fd,content,strlen(content));
}

printf("\033[35;1m package with name %s has been created successfully\033[0m\n",name);
printf("\033[35mupdating data int the registry file....\033[0m\n");
char regbuffer[1024];
char temp_buffer[1024];
int pos=0;
int temp_pos=0;

int n;
  while ((n = read(fdreg, temp_buffer + temp_pos, sizeof(temp_buffer) - temp_pos)) > 0) {
        temp_pos += n;
    }

memmove(regbuffer + pos, "\n", 1);
pos += 1;
memmove(regbuffer+pos,name,strlen(name));
pos+=strlen(name);//1+strlen(name)
memmove(regbuffer+pos,"|",1);
pos+=1;//1+strlen(name)+1
memmove(regbuffer+pos,version,strlen(version));
pos+=strlen(version);
memmove(regbuffer+pos,"|",1);
pos+=1;
memmove(regbuffer+pos,description,strlen(description));
pos+=strlen(description);
memmove(regbuffer + pos, "\n", 1);
pos += 1;
//write(fdreg, temp_buffer, temp_pos);
write(fdreg,regbuffer,pos);
close(fdreg);
return fdreg;
}



int main()
{
//start with the registry file that will hold the package names
int fd = open("registryFile.txt", O_CREATE|O_RDWR);
if(fd <0)
{
printf("Failed to create file!!!\n");
return -1;

}
else{
char* registry_file_content="pkg Name|Version|Description\n";
write(fd,registry_file_content,strlen(registry_file_content));
close(fd);
printf("\033[34;1mregistry file has been created with name \"registryFile.txt\"\033[0m\n");

}
//Lets_create_local_package directory
if(mkdir("server")<0)
{

    printf("\033[31;1m ERROR from the setup environment: Failed to create packages directory!!!\033[0m\n");

}
else
{
    printf("\033[32;1m packages directory has been created!!!\033[0m\n");
    //let's create some dump packages
    create_package("package1.pkg","1.0","A dummy package for testing","just a content");
    create_package("package2.pkg","1.0","Another dummy package for testing","just another content");
    create_package("package3.pkg","2.1","\0","\0");
    //executable files
    create_executable("create_calc","calculator","1.2","calculator program");
    create_executable("create_count10","count10","1.1","program counts from 1 to 10");
    create_executable("create_exec","testprogram","1.0","It was a test program that prints one statement");
}

//_______________________________________________________________________________________________
//Now to the part of the package manager itself
printf("\033[32;44m---------------------------------------------------------\033[0m\n");
printf("\033[33;1mcreating the package manager local inventory....\033[0m\n");


//local inventory list
int fdLocal = open("pkgsaurusList.txt", O_CREATE|O_RDWR);
if(fdLocal <0)
{
    printf("\033[31;1mFailed to create pkgsaurusList.txt!!!\033[0m\n");
    return -1;

}
else{
char* registry_file_content="pkg Name|Version|Description\n";
write(fdLocal,registry_file_content,strlen(registry_file_content));
close(fdLocal);
printf("\033[34;1mpkgsaurusList.txt file has been created successfully\033[0m\n");
}

//create the pkgsaurus local inventory:
printf("\033[35;1;3mCreating the pkgsaurus local inventory.....\033[0m\n");
if(mkdir("pkgsaurus_packages")<0)
{
printf("\033[31;1mERROR: unable to create \033[4;3mthe pkgsaurus local inventory\033[0m\n");
return -1;
}
else
{
printf("\033[32;1m \033[3mpkgsaurus_packages\033[36;1m inventory has been created successfully\033[0m\n");

}

printf("\033[36;3;4mSetup finished successfully, have fun with pckgsaurus!!!!\033[0m\n");

return 0;
}
