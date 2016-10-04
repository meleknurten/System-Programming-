#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "restart.h"
#include "uici.h"
#include<pthread.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define max_size 255500
#define BLKSIZE PIPE_BUF

typedef struct {
    char name[255];
    char filesNames[255][255];
    int numOfFiles;

} klasor; // her klasorun bılgısı

typedef struct {
    int pid[255];
    int count;
} client;



void hello(int signum);
int isdirectory(char *path); /* directory mı dıye bakan fonsıyon*/
void walkInPath(char *arr);

client arr[255];
klasor directories[255];
klasor directoriesServer[255];
klasor directoriesServer2[255];
int bytescopied;
int bytesread;
int communfd;
char buf[BLKSIZE];
int control=0;
int main(int argc, char *argv[]) {
    int i = 0;
    char s[3]="/";
    char *token;
    char select[255],pid3[255];
    char str[255];  
    char dosya[max_size];
    char alcam[255]="almam";
    u_port_t portnumber;
    char PtrFirstPath[max_size];
    char filename[255],pidd[255],filename2[255];
    char clientID[255];
    FILE *inp,*input,*output;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host port\n", argv[0]);
        return 1;
    }
    signal(SIGINT, hello); //ctrl+c
    portnumber = (u_port_t) atoi(argv[2]);
    if ((communfd = u_connect(portnumber, argv[1])) == -1) {
        perror("Failed to make connection");
        return 1;
    }
    fprintf(stderr, "[%ld]:connected %s\n", (long) getpid(), argv[1]);
    sprintf(buf, "%ld", (long) getpid());
    sprintf(pidd, "%ld", (long) getpid());
    printf("gonderılen sey: %s \n", buf);
    r_write(communfd, buf, sizeof (buf));
    // komutları yapmaya basla.
    for (;;) {
        signal(SIGINT, hello); //ctrl+c
        ////
        bytesread = readblock(communfd, directoriesServer2, sizeof (directoriesServer2));
        fprintf(stderr, "[%ld]:received %d bytes\n",
                    (long) getpid(), bytesread);
//NURTEN
        for (i = 0; i < directoriesServer2[0].numOfFiles; ++i){
                //strtok ile / dan sonrasını alcam
               // filename2 e kayded
                token = strtok(directoriesServer2[0].filesNames[i], s);
               /* walk through other tokens */
               while( token != NULL ) 
               {
                 // printf( " %s\n", token );
                  strcpy(pid3, token);
                  token = strtok(NULL, s);
               }
            if(strcmp(pidd,pid3)==0){ // eger aynı ısımde dosya varsa al
                //dosya alıcam dıye bısı yollasın
                printf("A FILE IS SEND TO ME. RECEIVED. \n");
                strcpy(alcam,"alcam");
                r_write(communfd, alcam, sizeof (alcam));
                bytesread = readblock(communfd, dosya, sizeof (dosya)); //server dosyayı gonderır.
                   
                    strncpy(filename2,dosya,15); // dosya ıcerıgının ılk satırı dosya ısmı
                    printf("%s\n", filename2);
                    output=fopen(filename2,"w"); // dosyayı yazarım
                    fputs(dosya, output);
                    strcpy(alcam,"almamm");
                    fclose(output);
                    i=directoriesServer2[0].numOfFiles;
                    directoriesServer2[0].numOfFiles=0;
            }
        }
        ////
        r_write(communfd, alcam, sizeof (alcam));
        printf("Please enter comment:\n");
        fgets (select, 255, stdin);
        sprintf(buf, "%s", select); // sectıgım commentı gonderıyorum servera
        
        
        sscanf(buf,"%s %s %s",select,filename,clientID); 
        signal(SIGINT, hello); //ctrl+c
        r_write(communfd, buf, sizeof (buf));
        if (strcmp(select, "listLocal") == 0) {
            control++;
            getcwd(PtrFirstPath, 255); //bulundugu path
            walkInPath(PtrFirstPath);
        }
        else if (strcmp(select, "listServer") == 0) {
            control++;
            bytesread = readblock(communfd, directoriesServer, sizeof (directoriesServer));
            fprintf(stderr, "[%ld]:received %d bytes\n",
                    (long) getpid(), bytesread);

            for (i = 0; i < directoriesServer[0].numOfFiles; ++i)
                printf("%s\n", directoriesServer[0].filesNames[i]);
            //serverdan gelen dosyaların listesi
        } else if (strcmp(select, "lsClients") == 0) {
            control++;
            bytesread = readblock(communfd, arr, sizeof (arr));
            fprintf(stderr, "[%ld]:received %d bytes\n",
                    (long) getpid(), bytesread);

            for (i = 0; i < arr[0].count; ++i)
                printf("%d\n", arr[0].pid[i]);
            
            //client id ,servera baglı olan clientların idleri
        } else if (strcmp(select, "sendFile") == 0) {
                control++;
                inp=fopen(filename,"r");
                if(inp == NULL) 
                   {
                      perror("Error opening file");
                      return(-1);
                   }
                   while( fgets (str, 255, inp)!=NULL ) 
                   {
                      strcat(dosya,str);
                   }
                r_write(communfd, dosya, sizeof (dosya));
               fclose(inp); 
            //client-client file yollama komutu. id yoksa serverın local directorysıne gıder dosya
        } else if (strcmp(select, "Help") == 0) {
            printf("Available comments and their usage: \n");
            printf("\n\a ListLocal \n\a listServer \n\a lsClients \n\a sendFile <filename> <clientid>\n");
        } else /* default */ {
            printf("Wrong comment!! Please try 'Help' \n");
        }

    } // DÖNGÜNÜN PARANTEZİ
    return 0;
}

void hello(int signum) {
    if(control!=0){
    printf("\nInterrup with ctrl+c ,program is closed.\n");
    printf("Interrup with ctrl+c, program is closed. \n");
    sprintf(buf, "%s", "stop"); // server a clıentın sonlandıgı bıldırılıyor
    r_write(communfd, buf, sizeof (buf));

    //remove("DosyaIsimleri.txt");
    remove("KlasorIsimleri.txt");
    exit(0);
    }else{
        printf("Your request number is %d and at least one execution should be created. \n", control);
        }
}

void walkInPath(char *arr) {
    struct dirent *direntPtr;
    DIR *dirPtr;
    char strNewPath[255];
    char strFilePath[255];
    char strRoot[255];
    strcpy(strNewPath, arr);
    if ((dirPtr = opendir(strNewPath)) == NULL) {
        fprintf(stderr, "Failed to open directory. \"%s\" : %s\n",
                direntPtr->d_name, strerror(errno));
        exit(0);
    }
    while ((direntPtr = readdir(dirPtr)) != NULL) {
        if (direntPtr->d_type & DT_DIR) {

            if (strcmp(direntPtr->d_name, ".") == 0 || strcmp(direntPtr->d_name, "..") == 0) {

            } else {
                directories[0].numOfFiles = 0; // dosya sayısı sıfırlanr
                strcpy(strRoot, strNewPath);
                strcat(strRoot, "/");
                strcat(strRoot, direntPtr->d_name);
                strcpy(directories[0].name, strRoot); // klasor bulduk ve kayıt altına aldık
                if (strlen(strRoot) <= 255) {
                } else {
                    printf("Boyut asildi.\n");
                    break;
                }
            }
        } else {
            if (direntPtr->d_name[(strlen(direntPtr->d_name) - 1)] != '~') {
                strcpy(strFilePath, strNewPath);
                strcat(strFilePath, "/");
                strcat(strFilePath, direntPtr->d_name);
                strcpy(directories[0].filesNames[directories[0].numOfFiles], strFilePath);
                if (strlen(strFilePath) <= 255) {
                    ++directories[0].numOfFiles;
                    printf("%s\n", strFilePath);
                } else {
                    printf("Dosya isim boyutu asti\nProgram kapatiliyor.\n");
                    break;
                }

            }
        }
    }
    if (closedir(dirPtr)) {
        fprintf(stderr, "'%s:  dosyasi kapatilamadi %s\n", arr, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int isdirectory(char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1)
        return 0;
    else
        return S_ISDIR(statbuf.st_mode);
}
