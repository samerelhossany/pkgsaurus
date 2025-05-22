// provides the basic types definitions used across xv6 kernel like int, unsigned int, long and datatypes.
#include "kernel/types.h"
//define the metadata like size, type, device, inode number...etc
#include "kernel/stat.h"
//contain the system calls
#include "user/user.h"

#include "kernel/fcntl.h"




char* Scanf() {
      static char buffer[512];//static to make one copy of buffer, DO NOT REMOVE THIS KEYWORD AT ANY COST!!!!!!!
    int i = 0;
    char ch;


    while (i < 511 && read(0, &ch, 1) == 1) {//while the iterator is less that buffer_size-1 and I can read a characters please add it and if its new line(end of string) break and add null terminator
        if (ch == '\n') break;
        buffer[i++] = ch;
    }

    buffer[i] = '\0';

    return buffer;
}
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

/*
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
*/
int find_in_pkgsaurus_list(char* package)
{
int fd =open("pkgsaurusList.txt",O_RDWR);
if(fd<0)
{
printf("\033[31;1mERROR in opening \033[4m\"pkgsaurusList.txt\"\033[0m\n");
close(fd);
return -1;
}


char name[255]= {0};
//char version[30];
//char desc[512];


char buf[512]= {0};
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

    if (argc != 3)
    {
      printf("\033[31mplease enter the command in the format of \033[1m\"pkgsaurus install fileName\" \033[0m \n");
      return;
    }
    //if(IsValidExtention(argv[3]) != 0){exit(1);return;}//if the last argument is invalid name

    //if(strcmp(argv[2],"pkg")==0)//if the 3rd argument is pkg extention pkgsaurus install pkg file.pkg
    //{
    //before checking the directory of the registry file, we have to check the list to make sure its not present.
    int foundStat= find_in_pkgsaurus_list(argv[2]);
    if(foundStat==1)
    {
      printf("\033[35;1mpackage with name \033[36;3m\"%s\"\033[0;35;1m already exists!!!!\033[0m\n",argv[2]);
      return;
    }
    else if(foundStat==0)
    {
        printf("\033[35;1mpackage with name \033[36;3m\"%s\"\033[0;35;1m is not found in pkgsaurus list!!!\033[0m\n",argv[2]);
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
    char name[255]= {0};
    char version[30]= {0};
    char desc[512]= {0};


    char buf[512]= {0};
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


            if(strcmp(argv[2],name)==0)
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
        printf("\033[92;1mThe package \033[92;3;4m%s \033[0;92;1m is found in the registry file\033[0m\n",argv[2]);
        printf("\033[36;3mDo you want to install the package \"%s\"?[\033[32;1my\033[0m/\033[31;1mn\033[0;36;3m]\033[0m\n",argv[2]);
        char* buf = Scanf();
        if(0 ==buf)
        {
        memset(buf, 0, sizeof(buf));
        printf("\033[31mERROR in taking input, please try again!!\n\033[0m");
        exit(1);
        }
        else if(strcmp("n",buf)==0)
        {
        memset(buf, 0, sizeof(buf));
        printf("\033[36mCancel the installation of the package!!\n\033[0m");
        return;
        }else if(strcmp("y",buf)!=0)
        {
         memset(buf, 0, sizeof(buf));
        printf("\033[34mPlease enter \033[31;1my\033[0;34m for yes or\033[31;1m n\033[0;34m for no!!!\n\033[0m");
        return;

        }


        }
    else
    {
    printf("\033[34;1m PACKAGE NOT FOUND IN THE REGISTRY FILE!!!!\033[0m\n");
    return;
    }

    printf("\033[1minstalling package...\033[0m\n");
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

    //}



}
void credits()
{



printf("\033[35;1mThank you for using pkgsaurus!!!\033[0m\n\n");
printf("\033[36;1mpkgsaurus\033[0;32m is a customized package manager for the xv6.\033[0m\n\n");
printf("\033[31;1minstructions:\033[0m\n\n");
printf("\033[32;1mpkgsaurus init \033[0;36;3m: Used to initiate the required directories and files that the package manager will work with like the main server that works as the internet and the local server which is the package server\033[0m\n\n");
printf("\033[32;1mpkgsaurus install package_name.pkg\033[0;36;3m: Used to install a certain package from the server to the local repository of \033[4mpkgsaurus_packages directory\033[0m\n\n");
printf("\033[32;1mpkgsaurus search package_name.pkg\033[0;36;3m: Used to search for a certain package in the \033[4mpkgsaurus_list file\033[0m\n\n");
printf("\033[32;1mpkgsaurus list\033[0;36;3m: Used to list all the packages that are present in the\033[4mpkgsaurus_list file\033[0m\n\n");
printf("\033[32;1mpkgsaurus remove <packge_name>\033[0;36;3m: Used to delete package from \033[4mpkgsaurus list and pkgsaurus packages\033[0m\n\n");
printf("\033[32;1mpkgsaurus draw <drawing>\033[0;36;3m: Used to draw one of the following drawings {camel, panda, hitler, dance_dino, london_bridge, aladdin,toy_story,star_wars}\033[0m\n\n");

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


//youssef section: list function
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
  printf("\n\033[32;7;1m");
  while ((n = read(fd, buf, sizeof(buf)))>0)
  {
    buf[n] = '\0';
    printf("%s", buf);
  }
  close(fd);
    printf("\033[0m\n");
  return;
}

//-------------------------------

//mustafa's part: remove function
//rename function: added by samer
/*
EACCES       13   // Permission denied
EEXIST       17   // File exists
EISDIR       21   // Is a directory
EMLINK       31   // Too many links
ENAMETOOLONG 36   // File name too long
ENOENT        2   // No such file or directory
ENOTDIR      20   // Not a directory
EPERM         1   // Operation not permitted
EROFS        30   // Read-only file system
EXDEV        18   // Invalid cross-device link
ENOSPC       28   // No space left on device
EIO           5   // I/O error
EBUSY        16   // Device or resource busy
EINVAL       22   // Invalid argument
*/
int rename(char* old_name,char* new_name)
{

int fd = open(old_name,O_RDONLY);
int fdNew = open(new_name,O_CREATE|O_WRONLY|O_TRUNC);
if(fd <0)
{
  printf("\033[31;1mERROR: cannot open the %s file\033[0m\n",old_name);
  return -1;
}
if(fdNew <0)
{
  printf("\033[31;1mERROR: cannot create the %s file\033[0m\n",new_name);
   return -1;
}
int n=0;
char buf[512];
while((n= read(fd,buf,sizeof(buf)))>0)
{
 if(write(fdNew,buf,n) != n)
 {
 printf("\033[31;1mERROR in writinf data\n\n\033[0m ");
 break;
 }

}
if(n<0){printf("\033[31;1mERROR in reading data\n\n\033[0m ");}

unlink(old_name);//remove the old_name
return 0;
}

//------------------------
void remove_pkg(char *argv[], int argc)
{
    if (argc < 3) {
        printf("\033[31;1mPlease enter the command in the format of \033[1m\"pkgsaurus remove package_name\"\033[0m\n");
        return;
    }



    // Check if package exists in pkgsaurusList.txt
    int found = find_in_pkgsaurus_list(argv[2]);
    if (found == 0) {
        printf("\033[34;1mPackage \033[36;3m\"%s\"\033[0;34;1m is not installed\033[0m\n", argv[2]);
        return;
    } else if (found == -1) {
        printf("\033[31;1mERROR occurred while searching in pkgsaurus list\033[0m\n");
        return;
    }

    // Remove package file from pkgsaurus_packages directory
    char package_path[64];
    memcpy(package_path, "pkgsaurus_packages/", 19);
    memcpy(package_path + 19, argv[2], strlen(argv[2]) + 1);

    if (unlink(package_path) < 0) {
        printf("\033[31;1mERROR: Cannot remove package file %s\033[0m\n", package_path);
        return;
    }

    // Create a temporary file to rewrite pkgsaurusList.txt without the removed package
    char temp_file[] = "temp_list.txt";
    int fd_temp = open(temp_file, O_CREATE | O_RDWR);
    if (fd_temp < 0) {
        printf("\033[31;1mERROR: Cannot create temporary file\033[0m\n");
        return;
    }

    int fd_list = open("pkgsaurusList.txt", O_RDONLY);
    if (fd_list < 0) {
        printf("\033[31;1mERROR in opening \033[4m\"pkgsaurusList.txt\"\033[0m\n");
        close(fd_temp);
        unlink(temp_file);
        return;
    }

    char buf[512];
    int n;
    char line[512];
    int start = 0;
    int line_pos = 0;
    //do not ever remove those lines or yoou will get the error of start is not used although its used and do not ask me why but remember the golden rule "if it works leave it"!!! by samer
     start++;
     start--;
     //they are not useless I'm warning you



    // Read pkgsaurusList.txt and write to temp file, excluding the package to remove
    while ((n = read(fd_list, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n' || i == n - 1) {
                if (buf[i] != '\n') {
                    line[line_pos++] = buf[i];
                }
                line[line_pos] = '\0';

                // Extract package name from the line
                char name[255];
                int j = 0;
                int z = 0;
                while (line[z] != '|' && z < line_pos) {
                    name[j++] = line[z++];
                }
                name[j] = '\0';

                // Only write the line if it doesn't match the package to remove

                if (strcmp(name, argv[2]) != 0) {
                    write(fd_temp, line, line_pos);
                    write(fd_temp, "\n", 1);
                }

                line_pos = 0;
                start = i + 1;
            } else {
                line[line_pos++] = buf[i];
            }
        }
    }

    if (n < 0) {
        printf("\033[31;1mERROR in reading pkgsaurusList.txt\033[0m\n");
        close(fd_list);
        close(fd_temp);
        unlink(temp_file);
        return;
    }

    close(fd_list);
    close(fd_temp);

    // Replace pkgsaurusList.txt with the temporary file
    if (unlink("pkgsaurusList.txt") < 0) {
        printf("\033[31;1mERROR: Cannot remove old pkgsaurusList.txt\033[0m\n");
        unlink(temp_file);
        return;
    }

    if (rename(temp_file, "pkgsaurusList.txt") < 0) {
        printf("\033[31;1mERROR: Cannot rename temporary file to pkgsaurusList.txt\033[0m\n");
        return;
    }

    printf("\033[32;1mPackage \033[36;3m\"%s\"\033[0;32;1m has been successfully removed\033[0m\n", argv[2]);
}

//-------------------------------




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
else if(strcmp("init",argv[1])==0)
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
  //if (strcmp(argv[2] + strlen(argv[2]) - 3, "pkg") == 0)//if the 3rd argument is pkg extention pkgsaurus install pkg file.pkg
  //{
          int state= find_in_pkgsaurus_list(argv[2]);
          if(state ==0)
          {
            printf("\033[34;1mPackage not found!!!!\033[0m\n");

          }else if(state ==1)
            {

            printf("\033[32;1mFOUND!!!!!!\033[0m\n");
            }
  //}
  else
  {
      printf("\033[31;1mERROR: wrong file format, please enter pkg files (e.g: package.pkg))\033[0m\n\n");

  }

}
else if(strcmp("remove",argv[1])==0)
{
printf("removing package %s....\n",argv[2]);
remove_pkg(argv,argc);

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
list();
}


}
else if(strcmp("draw",argv[1])==0)
{
if(argc != 3)
{
printf("\033[31;1mERROR: you must use the ecommand in the format of pkgsaurus draw dino (or any suitable art)\033[0m\n");
exit(1);

}
if(strcmp(argv[2],"dance_dino")==0)
{
char *new_argv[] = {"draw", 0};
exec("draw", new_argv);

}
else if(strcmp(argv[2],"hitler")==0)
{
char *new_argv[] = {"HitlerDraw", 0};
exec("HitlerDraw", new_argv);

}
else if(strcmp(argv[2],"london_bridge")==0)
{
char *new_argv[] = {"london_bridge", 0};
exec("london_bridge",new_argv);
}
else if(strcmp(argv[2],"camel")==0)
{
char *new_argv[] = {"camel", 0};
exec("camel",new_argv);
}
else if(strcmp(argv[2],"panda")==0)
{
char *new_argv[] = {"panda", 0};
exec("panda",new_argv);
}
else if(strcmp(argv[2],"aladdin")==0)
{
char *new_argv[] = {"aladdin", 0};
exec("aladdin",new_argv);
}
else if(strcmp(argv[2],"toy_story")==0)
{
char *new_argv[] = {"toy_story", 0};
exec("toy_story",new_argv);
}
else if(strcmp(argv[2],"star_wars")==0)
{
char *new_argv[] = {"star_wars", 0};
exec("star_wars",new_argv);
}
else
{
printf("\033[31;1mERROR: no draw option with the name \"%s\"\033[0m\n",argv[2]);
exit(1);

}

}
else
{
printf("%s\n",argv[1]);
printf("\033[33;1mInvalid command!!!\033[0m\n");
}
return 0;
}



