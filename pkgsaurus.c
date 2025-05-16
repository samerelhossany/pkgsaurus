// provides the basic types definitions used across xv6 kernel like int, unsigned int, long and datatypes.
#include "kernel/types.h"
//define the metadata like size, type, device, inode number...etc
#include "kernel/stat.h"
//contain the system calls
#include "user/user.h"

#include "kernel/fcntl.h"


int append_to_file(char *filename, char *text)
{
  int fd, n;
  char buf[512];

  // Open the file for reading and writing
  fd = open(filename, O_RDWR);
  if(fd < 0) {
    // File doesn't exist, create it
    fd = open(filename, O_CREATE | O_RDWR);
    if(fd < 0) {
      //printf( "append_to_file: cannot open or create %s\n", filename);
      return -1;
    }
  }

  // Find the end of the file by reading until EOF
  while((n = read(fd, buf, sizeof(buf))) > 0) {
    // Just keep reading to move the file pointer
  }

  if(n < 0) {
    //printf( "append_to_file: read error\n");
    close(fd);
    return -1;
  }

  // Now fd is positioned at the end of the file
  // Write the new content
  if(write(fd, text, strlen(text)) != strlen(text)) {
    //printf( "append_to_file: write error\n");
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}


int IsValidExtention(char* text)
{

if(strlen(text)<4)//the minimum size is .pkg which is 4
{
printf("\033[31;1mInvalid format for file name \"%s\"\033[0m\n",text);
return -1;
}

if(strcmp(&text[strlen(text)-4],".pkg") !=0)//if the extention is not .pkg temporary until we add new features
{
printf("\033[31;1mInvalid format for file name \"%s\"\033[0m\n",text);
return -1;
}

return 0;
}

int find_in_pkgsaurus_list(char* package)
{
int fd =open("pkgsaurusList.txt",O_RDWR);
if(fd<0)
{
printf("\033[31;1mERROR in opening \033[4m\"pkgsaurusList.txt\"\033[0m\n");
close(fd);
return -1;
}


char name[255];
//char version[30];
//char desc[512];


char buf[512];
int n = read(fd, buf, sizeof(buf));
if (n < 0) {
    printf("Failed to read the file.\n");
    close(fd);
    return -1;
}

// Manually parse line by line
int start = 0;
int found =0;
for (int i = 0; i < n; i++) {
    if (buf[i] == '\n' || i == n - 1) {
        // Extract line
        char line[100];
        int len = i - start + 1;
        if (len > sizeof(line) - 1) len = sizeof(line) - 1;
        memmove(line, &buf[start], len);
        line[len] = '\0';

        int z = 0;
        int j=0;
        while(line[z]!='|')//name extraction
        {
        name[j++]=line[z++];

        }
        j=0;//restart for the version
        // while(line[z]!='|')//version extraction
        //{
       // version[j++]=line[z++];

       // }
          //j=0;//restart for the description
          // while(line[z]!='|')//description extraction
       // {
       // desc[j++]=line[z++];
       // }

        if(strcmp(package,name)==0)
        {

        found =1;
        name[j++]='\0';
        //desc[j++]='\0';
        //version[j++]='\0';
        //found =1;
        //printf("\033[35;1mPackage \033[3m\"%s\"\033[0;35;1m was found with version %s and its description was %s\033[0m\n\n",name,version,desc);
        break;
        }
        // Now you can compare line with a package name or do anything
        //printf("Line: %s", line);  // Already includes newline

        start = i + 1;
    }

}
close(fd);
return found;
}



void install(char* argv[],int argc)// expected format:  pkgsaurus install pkg file.pkg
{

    if (argc<4)
    {
      printf("\033[31mplease enter the command in the format of \033[1m\"pkgsaurus install pkg file.pkg\" \033[0m \n");
      return;
    }
    if(IsValidExtention(argv[3]) != 0){exit(1);return;}//if the last argument is invalid name

    if(strcmp(argv[2],"pkg")==0)//if the 3rd argument is pkg extention pkgsaurus install pkg file.pkg
    {
    //before checking the directory of the registry file, we have to check the list to make sure its not present.
    int foundStat= find_in_pkgsaurus_list(argv[3]);
    if(foundStat==1)
    {
      printf("\033[35;1mpackage with name \033[36;3m\"%s\"\033[0;35;1m already exists!!!!\033[0m\n",argv[3]);
      return;
    }
    else if(foundStat==0)
    {
        printf("\033[35;1mpackage with name \033[36;3m\"%s\"\033[0;35;1m is not found in pkgsaurus list!!!\033[0m\n",argv[3]);
    }
    else  if(foundStat==-1)
    {
        printf("\033[31;1mERROR occurred while searching in packsaurus list !!!!\033[0m\n\n");
        return;
    }
          printf("\033[34;1msearching for package in the registry file of the server....\033[0m\n");
          //open file to check if package is in registery file or not
          int state = open("registryFile.txt",O_RDONLY);
    if(state == -1)
    {
    printf("\033[31;1mERROR: Registry File not found !!!!\033[0m\n");
    return;
    }
    char name[255];
    char version[30];
    char desc[512];


    char buf[512];
    int n = read(state, buf, sizeof(buf));
    if (n < 0)
    {
        printf("Failed to read the file.\n");
        close(state);
        return;
    }

    // Manually parse line by line
    int start = 0;
    int found =0;
    for (int i = 0; i < n; i++)
    {
        if (buf[i] == '\n' || i == n - 1)
        {
            // Extract line
            char line[100];
            int len = i - start + 1;
            if (len > sizeof(line) - 1) len = sizeof(line) - 1;
            memmove(line, &buf[start], len);
            line[len] = '\0';

            int z = 0;
            int j=0;
             // clear buffers
            memset(name, 0, sizeof(name));
            memset(version, 0, sizeof(version));
            memset(desc, 0, sizeof(desc));
            while(line[z]!='|'&& z < len)//name extraction
            {
            name[j++]=line[z++];

            }
            name[j]='\0';
            z++;
            j=0;//restart for the version
            while(line[z]!='|')//version extraction
            {
            version[j++]=line[z++];

            }
            version[j]='\0';
              z++;
              j=0;//restart for the description
            while(z < len && line[z] != '\n')//description extraction
            {
            desc[j++]=line[z++];
            }
              desc[j]='\0';
            if(strcmp(argv[3],name)==0)
            {

            found =1;



            printf("\033[36;1mpackage found: \033[32;1m%s\033[36;1m, version: \033[32;1m%s\033[36;1m, description: \033[32;1m%s\033[0m\n", name, version, desc);
            break;
            }
            // Now you can compare line with a package name or do anything
            //printf("Line: %s", line);  // Already includes newline

            start = i + 1;
        }

    }
    //printf("here\n");
    if(found)
        {
        printf("\033[92;1mThe package \033[92;3;4m%s \033[0;92;1m is found in the registry file\033[0m\n",argv[3]);
        }
    else
    {
    printf("\033[34;1m PACKAGE NOT FOUND IN THE REGISTRY FILE!!!!\033[0m\n");
    return;
    }
    printf("installing package...\n");
    //we want 2 things in this section: copy file name to pkgsaurus list then copy the package itself to the local inventory

    //A)copy file name to pkgsaurus list
    printf("\033[35;1mAdding files to the pkgsaurus list......\033[0m\n");
    //we have to gather the string together using memset to write it down

    append_to_file("pkgsaurusList.txt",name);
    append_to_file("pkgsaurusList.txt","|");
    append_to_file("pkgsaurusList.txt",version);
    append_to_file("pkgsaurusList.txt","|");
    append_to_file("pkgsaurusList.txt",desc);
    append_to_file("pkgsaurusList.txt","\n");
//cp file1
    printf("\033[32;1m package has been added successfully to the pkgsaurus list!!\033[0m\n");
    //b) adding packages to the local inventory
    //unfortunately no cp file so, I have to create it manually
    //retrieve the original file text
    char holder[64];
    char holder_for_copy[64];
    memcpy(holder,"server/",7);
    memcpy(holder+7,name,strlen(name)+1);

    memcpy(holder_for_copy,"pkgsaurus_packages/",19);
    memcpy(holder_for_copy+19,name,strlen(name)+1);

    int fd_original= open(holder,O_RDONLY);
    int fd_new= open(holder_for_copy,O_CREATE|O_RDWR);
    //printf("%s\n",holder);
    //printf("%s\n",holder_for_copy);

    if(fd_original<0)
    {
    printf("\033[31;1mERROR: cannot open the file %s\033[0m",holder);
    }


    if(fd_new<0)
    {
    printf("\033[31;1mERROR: cannot create the file %s\033[0m",holder_for_copy);
    }

   char buffer_to_copy[512];
   int nc;

   while((nc= read(fd_original,buffer_to_copy,sizeof(buffer_to_copy)))>0)
   {
       if(write(fd_new,buffer_to_copy,nc)!=nc)
       {
         printf("\033[31;1mERROR in copying data!!!\033[0m\n");
         close(fd_original);
          close(fd_new);
          exit(1);
       }



   }
    if(n<0)
       {
        printf("\033[31;1mERROR in reading data!!!\033[0m\n");
         close(fd_original);
          close(fd_new);
        exit(1);
       }
        close(fd_original);
          close(fd_new);
       printf("\033[32;1mpackage has been installed successfully to the \033[36;4;3mpkgsaurus_packages directory!!!\033[0m\n");
    return;

    }



}
void credits()
{



printf("\033[35;1mThank you for using pkgsaurus!!!\033[0m\n\n");
printf("\033[36;1mpkgsaurus\033[0;32m is a customized package manager for the xv6.\033[0m\n");
printf("\033[31;1minstructions:\033[0m\n");
printf("\033[32;1mpkgsaurus init \033[0;36;3m: Used to initiate the required directories and files that the package manager will work with like the main server that works as the internet and the local server which is the package server\033[0m\n");
printf("\033[32;1mpkgsaurus install pkg package_name.pkg\033[0;36;3m: Used to install a certain package from the server to the local repository of \033[4mpkgsaurus_packages directory\033[0m\n\n");
printf("\033[32;1mpkgsaurus search package_name.pkg\033[0;36;3m: Used to search for a certain package in the \033[4mpkgsaurus_list file\033[0m\n\n");


printf("\n\n\n\n\033[32;1m");
printf("                                                  ____\n");
printf("       ___                                      .-~. /\"-._\n");
printf("      `-._~-.                                  / /_ \"~o\\  :Y\n");
printf("          \\  \\                                / : \\~x.  ` ')\n");
printf("           ]  Y                              /  |  Y< ~-.__j\n");
printf("          /   !                        _.--~T : l  l<  /.-~\n");
printf("         /   /                 .-~~\"        /| .    ',-~\\ \\L|\n");
printf("        /   /             .-~~\"        /| .    ',-~\\ \\L|\n");
printf("       /   /             /     .^   \\ Y~Y \\.^>/l_   \"--'\n");
printf("      /   Y           .-\"(  .  l__  j_j l_/ /~_.-~    .\n");
printf("     Y    l          /    \\  )    ~~~.\" / `/'~ / \\.__/l_\n");
printf("     |     \\     _.-\"      ~-{__     l  :  l._Z~-.___.--~\n");
printf("     |      ~---~           /   ~~\"---\\_  ' __[>\n");
printf("     l  .                _.^   ___     _>-y~\n");
printf("      \\  \\     .      .-~   .-~   ~>--\"  /\n");
printf("       \\  ~---\"            /     ./  _.-'\n");
printf("        \"-._____.,_  _.--~\\     _.-~\n");
printf("                    ~~     (   _}       -Row\n");
printf("                           `. ~( \n");
printf("                             )  \\\n");
printf("                            /,`--'~\\--'~\\\n");
printf("\033[0m\n\n\n");

printf("\n\033[34;1mCreated by:\n\n\033[3;32mSamer Elhossany 221001697\033[0m\n\n\033[3;32;1mZahraa Gwyly 221000968\n\n\033[3;32;1mMustafa Adel 221002165\n\n\033[3;32;1mMohamed Ahmed 2210021705\n\n\033[3;32;1mYoussef Abbas 221000189\n\n\n\033[1;34mspecial thanks for: \033[0m\n\n\033[3;36;1m1)Dr Eman\033[0m\n\n\033[3;36;1m2)Eng Gannat\033[3;36;1m\033[0m\n\n\033[3;36;1m3)Eng mourad\033[0m\n");
}

int list_is_valid(int argc , char *argv[]){
  if (argc>2) {
    printf("\033[31;1mInvalid command: max 2 arguments\033[0m\n\n");
    return 0;
  }  
  return 1;
}
void list()
{
  int fd = open("pkgsaurusList.txt", O_RDONLY); 
  if (fd < 0)
    {
      printf("\033[31;1mERROR in opening \033[4m\"pkgsaurusList.txt\"\033[0m\n");
      return ;
    }
  char buf[512];  
  int n;
  while ((n = read(fd, buf, sizeof(buf)))>0)
  {
    buf[n] = '\0';            
    printf("%s", buf); 
  }
  close(fd);
  return;
}







//argc is the number of arguments
//argv are the arguments itself, argv[0] is always the program name
//example touch file.c, argv[0]= touch and argv[1] = file.c
int main(int argc,char* argv[])
{

if (argc == 1)//the least command needs at least two arguments
{
credits();
//printf("\033[31;1mInvalid command!!\033[0m\n");
//exit(1);//exit with error, remember guys that 1 always mean error
}
else if (strcmp("init",argv[1])==0)
{
    if (argc>2)
    {
        printf("\033[31mInvalid command: do you mean \033[1;3m\"pkgsaurus init\" \033[0;31m?\033[0m");
    return -1;
    }
    printf("\033[32minitializing the required resources for \033[36;1;3mpkgsaurus\033[0;32m....... \033[0m\n");
char *new_argv[] = {"setupEnv", 0};
exec("setupEnv", new_argv);
}
else if(strcmp("install",argv[1])==0)
{
install(argv,argc);
}

else if(strcmp("search",argv[1])==0)
  {
  if (strcmp(argv[2] + strlen(argv[2]) - 3, "pkg") == 0)//if the 3rd argument is pkg extention pkgsaurus install pkg file.pkg
  {
          int state= find_in_pkgsaurus_list(argv[2]);
          if(state ==0)
          {
            printf("\033[34;1mPackage not found!!!!\033[0m\n");

          }else if(state ==1)
            {

            printf("\033[32;1mFOUND!!!!!!\033[0m\n");
            }
  }
  else
  {
      printf("\033[31;1mERROR: wrong file format, please enter pkg files (e.g: package.pkg))\033[0m\n\n");

  }

}
else if(strcmp("remove",argv[1])==0)
{
printf("removing package\n");

}
else if(strcmp("update",argv[1])==0){
printf("updating package\n");
}
else if(strcmp("upgrade",argv[1])==0){
printf("upgrading package\n");
}
else if(strcmp("list",argv[1])==0)
{
if (list_is_valid(argc, argv) == 1){
printf("Listing packages\n");
list();}
}
else
{
printf("%s\n",argv[1]);
printf("\033[33;1mInvalid command!!!\033[0m\n");
}
return 0;
}



