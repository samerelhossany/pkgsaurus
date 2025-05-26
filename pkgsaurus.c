// provides the basic types definitions used across xv6 kernel like int, unsigned int, long and datatypes.
#include "kernel/types.h"
//define the metadata like size, type, device, inode number...etc
#include "kernel/stat.h"
//contain the system calls
#include "user/user.h"

#include "kernel/fcntl.h"

//I implemented the stdarg library to create our beloved _snprintf: By Samer
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)



int tokenize_string(char*str,char text[][64],int* ti,int* specifier_count)// the tokenizer that split strings into tokens based on % specifier
{



    char specifier_holder[3]={0};
    char strholder[64]={0};
    //int ti =0;//text index

    for(int i =0, j =0;i< strlen(str);i++)
  {

        if(i+1 ==strlen(str))
            {
            strholder[j++]=str[i];
            memcpy(text[(*ti)++],strholder,sizeof(strholder));
             //strcpy(text[(*ti)++], strholder);
             //printf("%s\n",strholder);
            }
       else if(str[i] == '%' && str[i+1] != ' ' && str[i+1] != '%'){

            if(strcmp(strholder,"") !=0)
            {
                //memcpy(text[(*ti)++],strholder,sizeof(strholder));
                strcpy(text[(*ti)++], strholder);
            }
            if(!(str[i+1] == 'd' || str[i+1] == 's' || str[i+1] == 'c'))
            {
                printf("\033[31;1mERROR INVALID SPECIFIER!!!\n\033[0m");
                return -1;
            }

            specifier_holder[0]=str[i++];
            specifier_holder[1]=str[i];
            specifier_holder[2]='\0';
            (*specifier_count)++;

             //memcpy(text[(*ti)++],specifier_holder,3);
             strcpy(text[(*ti)++], specifier_holder);
             memset(specifier_holder,0,3);

             int size = sizeof(strholder);
                 memset(strholder,0,size);
                 j=0;

        }else if(str[i] == '%' && str[i+1] == '%')
        {
        strholder[j++]='%';
        i++;
        }
        else
        {
            strholder[j++]=str[i];

        }



    }

//memcpy(text[(*ti)++],"\0",1);
strcpy(text[(*ti)++], "\0");

return 0;

}

void __itoa(int n, char* container) {
    int i = 0;
    int neg_flag = 0;
    if (n == 0) {
        container[0] = '0';
        container[1] = '\0';
        return;
    }
    if (n < 0) {
        neg_flag = 1;
        n = -n;
    }

    char reverse_container[64];
    while (n != 0) {
        reverse_container[i++] = '0' + (n % 10);
        n /= 10;
    }

    int j = 0;
    if (neg_flag) {
        container[j++] = '-';
    }
    while (i > 0) {
        container[j++] = reverse_container[--i];
    }
    container[j] = '\0';
}


// how this works?
//1)pass the array or container or whatever you want to add string to
//2)add buffer size as I added a condition to check the size and the offset to waarn you in case there is a bufferflow
//3) buffer is considered as the string that contains the specifier
//4)args is the number of the aguments, for example, I have %s only then I will pass 1
//5) ... is related to the variadict function that was supposed to be present in <stdarg> but as you know, xv6 don't have anything

int __snprintf(char* container,int bufsz,char* buffer,int args,...)
{

va_list ap;
va_start(ap,args);

//tokenizing the string
int ti =0;
int specifier_count =0;
char tokenized_string[16][64];
tokenize_string(buffer,tokenized_string,&ti,&specifier_count);
//for(int i=0;i<ti;i++)
//{
//printf("%s\n",tokenized_string[i]);//use it for debugging: Note is by samer
//}
int offset=0;
//in case you entered for example %d %s but you pass 2 only as a specifier which is not valid
if(args != specifier_count)
    {
    printf("\033[31;1mERROR: mismatch in number of specifiers!!\n\033[0m");
    return -1;
    }

        for(int i =0; i<ti;i++)
            {
            //I will handle string first then I will add other data types later
            if(tokenized_string[i][0] =='%') //if it starts with %
               {
                 if(tokenized_string[i][1] == 's') //if its an string
                  {
                      char* miniBuffer=va_arg(ap,char*);

                       //just a check to avoid the weird buffer overflow: by samer
                       int len = 0;
                        len = strlen(miniBuffer);


                        if (offset + len >= bufsz) {
                            printf("\033[31;1mERROR: Output buffer overflow!\n\033[0m");
                            va_end(ap);
                            return -1;
                        }
                        //----------------------------------------------
                       memcpy(container+offset,miniBuffer,strlen(miniBuffer));
                       offset+=strlen(miniBuffer);

                  }else if(tokenized_string[i][1] == 'c')
                  {
                   //char* chtostr= charToString(va_arg(ap,char));

                   container[offset++]= (char)va_arg(ap,int);

                  }else if(tokenized_string[i][1] == 'd')
                  {
                   char integers[64];
                   __itoa(va_arg(ap,int),integers);

                     //just a check to avoid the weird buffer overflow: by samer
                         int len = 0;

                        len = strlen(integers);

                        if (offset + len >= bufsz) {
                            printf("\033[31;1mERROR: Output buffer overflow!\n\033[0m");
                            va_end(ap);
                            return -1;
                        }
                        //----------------------------------------------

                   memcpy(container+offset,integers,strlen(integers));
                   offset+=strlen(integers);
                   //printf("%s\n",container);
                  }
                  else//for now just write down the specifier if its invalid for now
                    {

                      //just a check to avoid the weird buffer overflow: by samer

                               int len = 2; // for invalid specifier (copying %x)

                        if (offset + len >= bufsz) {
                            printf("\033[31;1mERROR: Output buffer overflow!\n\033[0m");
                            va_end(ap);
                            return -1;
                        }
                        //----------------------------------------------

                     memcpy(container+offset,tokenized_string[i],2);
                       offset+=2;
                    }

                   }else
                   {
                     //just a check to avoid the weird buffer overflow: by samer

                    memcpy(container+offset,tokenized_string[i],strlen(tokenized_string[i]));
                    offset += strlen(tokenized_string[i]);
                   }




                // tokenized_tring[i][1]=='d'||  //if its a int
                  //tokenized_tring[i][1]=='c')  //if its a character
               }




/*
for(int i = 0;i<args;i++)

    printf("%s\n",va_arg(ap,char*));
    */
va_end(ap);
if (offset >= bufsz) {
    printf("\033[31;1mERROR: Output buffer overflow!\n\033[0m");
    return -1;
}
container[offset] = '\0';
return 0;
}





/*
    int ti =0;
    int n =0;
    char tokenized_tring[512][512];

    //tokenize_string("hello %s how are your %d and how %c feels",&tokenized_tring,&ti,&n);
      char container[512];
   __snprintf(container,512,"hello %s how are your %c and how %d feels. I\'m 100% sure ",3,"samer",'s',321);

*/

//------------------------------------------------

/*
int _snprintf(char* buffer,unsigned int bufsz,int n,...)
{

va_list ap;
va_start(ap,n);
//1st I need to detect every possible % sign
//I have %d %c %s for now + I know that there are many
char** statements = (char**)calloc(512,sizeof(char)); //512 characters for now
char**values = (char**)calloc(512,sizeof(char));
char holder[2];
for(int i =0,j=0; i<bufsz;i++)
{
if(buffer[i] == '%')
{
    if(i+1 <bufsz)
    {
       values[j++]='%';
       values[j]=buffer[++i];

    }

}

}
va_end(ap);

return 0;
}

*/
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
        while(line[z]!='|' && line[z] != '\0')//name extraction
        {
        if (j >= sizeof(name) - 1) break;
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

//security layer: zahraa's part
//note by samer-> I implemented snprintf just for your part!!!//note by samer-> I implemented snprintf just for your part!!!
unsigned int crc32_update(unsigned int crc, const unsigned char *data, int len) {
    crc = ~crc;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }
    return ~crc;
}

void write_checksum(char *pkg_path) {
    int fd = open(pkg_path, O_RDONLY);
    if (fd < 0) {
        printf("\033[31;1mError: cannot open package file %s\n\033[0m", pkg_path);
        return;
    }

    char buf[128];
    int n;
    unsigned int crc = 0xFFFFFFFF;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        crc = crc32_update(crc, (unsigned char*)buf, n);
    }
    close(fd);

    char cksum_path[128];
    __snprintf(cksum_path, sizeof(cksum_path), "%s.cksum", 1, pkg_path);

    int cfd = open(cksum_path, O_CREATE | O_WRONLY);
    if (cfd < 0) {
        printf("\033[31;1mError: cannot create checksum file\n\033[0m");
        return;
    }
    write(cfd, &crc, sizeof(crc));
    close(cfd);
    printf("\033[33;1mChecksum written to %s\n\033[0m", cksum_path);
}

void verify_checksum(char *pkg_path) {
    int fd = open(pkg_path, O_RDONLY);
    if (fd < 0) {
        printf("\033[31;1mError: cannot open package file %s\n\033[0;", pkg_path);
        return;
    }

    char buf[512];
    int n;
    unsigned int calc_crc = 0xFFFFFFFF;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        calc_crc = crc32_update(calc_crc, (unsigned char*)buf, n);
    }
    close(fd);

    char cksum_path[128];
    __snprintf(cksum_path, sizeof(cksum_path), "%s.cksum", 1, pkg_path);

    int cfd = open(cksum_path, O_RDONLY);
    if (cfd < 0) {
        printf("\033[31;1mError: checksum file not found: %s\n\033[0m", cksum_path);
        return;
    }

    unsigned int stored_crc;
    if (read(cfd, &stored_crc, sizeof(stored_crc)) != sizeof(stored_crc)) {
        printf("\033[31;1mError: failed to read checksum data\n\033[0m");
        close(cfd);
        return;
    }
    close(cfd);

    if (calc_crc == stored_crc) {
        printf("\033[32;1mOK: %s is verified\n\033[0m", pkg_path);
    } else {
        printf("\033[35;1mCORRUPTED: %s does not match checksum\n\033[0m", pkg_path);
    }
}

void test_crc(char *pkg_path) {
    write_checksum(pkg_path);
    verify_checksum(pkg_path);
}

//--------------------------------

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
    int pid = fork();
    if (pid < 0) {
    printf("fork failed\n");
    exit(1);
}
if (pid == 0) {
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
          //security check
          //char pathcrc[128];
          //strcpy(pathcrc,"pkgsaurus_packages/");
          //int offsetcrc=strlen("pkgsaurus_packages/");
          //strcpy(pathcrc+offsetcrc,argv[2]);
          //offsetcrc+=strlen(argv[2]);
          //strcpy(pathcrc+offsetcrc,"\0");
          //printf("%s\n",pathcrc);
          //write_checksum(pathcrc);
          //verify_checksum(pathcrc);
       printf("\033[32;1mpackage has been installed successfully to the \033[36;4;3mpkgsaurus_packages directory!!!\033[0m\n");
    return;

    //}
exit(0);
}else
{
int status;
    wait(&status);
    if (status == 0) {
        printf("\033[32;1mPackage found and processed successfully\033[0m\n");
        exit(0);
    } else {
        printf("\033[33;1mPackage not found or error occurred\033[0m\n");
        exit(1);
    }
}

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
printf("\033[32;1mpkgsaurus update\033[0;36;3m: Used to list all updates like packages with different versions for the upgrade function.\033[0m\n\n");
printf("\033[32;1mpkgsaurus upgrade\033[0;36;3m: Used to take the list of the updates then apply it by deleting old packages and installing new packages.\033[0m\n\n");
printf("\033[32;1mpkgsaurus writecrc <package path>\033[0;36;3m: Used to create cksum file in order to make comparison later and check the crc using \033[1m\"verifycrc\" \033[0;36;3mcommand.\033[0m\n\n");
printf("\033[32;1mpkgsaurus verifycrc <package path>\033[0;36;3m: Used to verify the crc that is present in the cksum file with the present file to make sure its not corrupted.\033[0m\n\n");
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

int pid = fork();
if (pid ==0){
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

}else
{
int status;
wait(&status);
if(status ==0)
  printf("Package found and processed successfully\n");
else
   printf("\033[31;1mERROR in creating install fork\n\033[0m");

}

}

//-------------------------------

//sto7y's part: update and upgrade


// 1. Check if local package has a newer version in registry
int find_latest_version(char *pkgname, char *local_version) {
    int fd = open("registryFile.txt", O_RDONLY);
    if (fd < 0) {
        printf("\033[31;1m[ERROR]\033[0m Cannot open registryFile.txt\n");
        return -1;
    }

    char buf[512];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        printf("\033[31;1m[ERROR]\033[0m Failed to read registryFile.txt\n");
        close(fd);
        return -1;
    }

    int start = 0;
    int line_num=0;
    for (int i = 0; i < n; i++) {
     line_num++;
            if(line_num==1)
            {
            continue;
            }
        if (buf[i] == '\n' || i == n - 1) {
            char line[128];
            int len = i - start;
            if (len >= sizeof(line)) len = sizeof(line) - 1;
            memmove(line, &buf[start], len);
            line[len] = '\0';

            char name[64] = {0}, version[32] = {0};
            int z = 0, j = 0;
            while (line[z] != '|' && line[z] != '\0') name[j++] = line[z++];
            name[j] = '\0';
            if (line[z] == '|') z++;
            j = 0;
            while (line[z] != '|' && line[z] != '\0') version[j++] = line[z++];
            version[j] = '\0';


            if (strcmp(pkgname, name) == 0) {
                close(fd);
                int cmp = strcmp(local_version, version);
                if (cmp < 0)
                    printf("\033[33;1m[INFO]\033[0m Package %s is outdated (local: %s, registry: %s)\n", name, local_version, version);
                else{
                    printf("\033[32;1m[INFO]\033[0m Package %s is up-to-date\n", name);

                      }


                return cmp < 0 ? 1 : 2;
            }
            start = i + 1;
        }
    }

    close(fd);
    printf("\033[34;1m[INFO]\033[0m Package %s not found in registry\n", pkgname);
    return 0; // not found in registry
}

// 2. Create "upgradeable" list by comparing registry with local list
void update() {
    printf("\033[36;1m[ACTION]\033[0m Checking for upgradeable packages...\n");
    int fd = open("pkgsaurusList.txt", O_RDONLY);
    if (fd < 0) {
        printf("\033[31;1m[ERROR]\033[0m Cannot open pkgsaurusList.txt\n");
        return;
    }

    char buf[512];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        printf("\033[31;1m[ERROR]\033[0m Failed to read pkgsaurusList.txt\n");
        close(fd);
        return;
    }

    int start = 0;

    for (int i = 0; i < n; i++) {

        if (buf[i] == '\n' || i == n - 1) {

            char line[128];
            int len = i - start;
            if (len >= sizeof(line)) len = sizeof(line) - 1;
            memmove(line, &buf[start], len);
            line[len] = '\0';

            char name[64] = {0}, version[32] = {0};
            int z = 0, j = 0;
            while (line[z] != '|' && line[z] != '\0') name[j++] = line[z++];
            name[j] = '\0';
            if (line[z] == '|') z++;
            j = 0;
            while (line[z] != '|' && line[z] != '\0') version[j++] = line[z++];
            version[j] = '\0';

             if((strcmp(name,"pkg Name")==0)||(strcmp(name,"")==0)){start = i + 1;continue;}
            int status = find_latest_version(name, version);
            if (status == 1) {
                printf("\033[33;1m[UPDATE]\033[0m Marking %s for upgrade\n", name);
                append_to_file("upgradeable", line);
            }
            start = i + 1;
        }

    }

    close(fd);
    printf("\033[36;1m[COMPLETE]\033[0m Update check finished.\n");
}

// 3. Perform the actual upgrade
void upgrade() {
    printf("\033[36;1m[ACTION]\033[0m Upgrading packages...\n");
    int fd = open("upgradeable", O_RDONLY);
    if (fd < 0) {
        printf("\033[31;1m[ERROR]\033[0m Cannot open upgradeable file\n");
        return;
    }

    char buf[512];
    int n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n <= 0) {
        printf("\033[32;1m[INFO]\033[0m No packages to upgrade.\n");
        return;
    }

    int start = 0;
    for (int i = 0; i < n; i++) {
        if (buf[i] == '\n' || i == n - 1) {
            char line[128];
            int len = i - start;
            if (len >= sizeof(line)) len = sizeof(line) - 1;
            memmove(line, &buf[start], len);
            line[len] = '\0';

            char name[64] = {0};
            int z = 0, j = 0;
            while (line[z] != '|' && line[z] != '\0') name[j++] = line[z++];
            name[j] = '\0';

            printf("\033[34;1m[UPGRADE]\033[0m Upgrading package: %s\n", name);
            printf("removing package %s ....\n",name);
            char *rm_argv[] = {"pkgsaurus", "remove", name};
             remove_pkg(rm_argv, 3);
                sleep(5);
             printf("installing the new package %s ....\n",name);
           char *inst_argv[] = {"pkgsaurus", "install", name};
              install(inst_argv, 3);

            start = i + 1;
        }
    }

    int clear_fd = open("upgradeable", O_CREATE | O_WRONLY | O_TRUNC);
    if (clear_fd >= 0) close(clear_fd);
    printf("\033[36;1m[COMPLETE]\033[0m All upgradeable packages have been processed.\n");
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
//test_crc("ls");
//printf("\033[31;1mInvalid command!!\033[0m\n");
//exit(1);//exit with error, remember guys that 1 always mean error:samer
//tokenize_string("Hello %s %c world",tokenized_string,&ti,&specifier_count);
     // char container[512];
   //__snprintf(container,64,"hello %s how are your %c and how %d feels. I\'m 100%% sure ",3,"samer",'s',321);

//printf("container: %s\n",container);
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
update();
}
else if(strcmp("upgrade",argv[1])==0){
printf("upgrading package\n");
upgrade();
}else if(strcmp("writecrc",argv[1])==0)
{
    if(argc != 3)
    {
    printf("\033[31;1mERROR invalid format for write crc\n\033[0m");

    }else
    {
    write_checksum(argv[2]);
    }
}else if(strcmp("verifycrc",argv[1])==0)
{
    if(argc != 3)
    {
    printf("\033[31;1mERROR invalid format for verify crc\n\033[0m");

    }else
    {
    verify_checksum(argv[2]);
    }
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



