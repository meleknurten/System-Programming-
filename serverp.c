#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "restart.h"
#include "uici.h"
#include <pthread.h>
#include <sys/signal.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h> 
#include <dirent.h>

#define BLKSIZE PIPE_BUF
#define max_size 255500
#define SHM_KEY_VALUE 12

typedef struct {
    char name[255];
    char filesNames[255][255];
    int numOfFiles;

} klasor; // her klasorun bılgısı

typedef struct {
    int pid[255];
    int count;
} client;
FILE *output2,*output;
client current[255];
sem_t semaphoree;
klasor directories[255];
int shmid,shmid2,shmid3;
char *file;
char *toMe; // clıent-clıent dosya gonderılırken kullanlır.
int isdirectory(char *path); /* directory mı dıye bakan fonsıyon*/
void walkInPath(char *arr);
void hello(int signum);
struct timespec clockres, currenttime,starttime,endtime;
int main(int argc, char *argv[]) {
    int bytescopied = 0;
    int stop = 0;
    char *gelen,alcam[255];
    pid_t child;
    char dosya[max_size],normal[255];
    char PtrFirstPath[max_size];
    char buf[BLKSIZE];
    int bytesread;
    char select[255],deat[255];
    char sonuc[255][255];
    int s=0,l=0,y=0,m=0,k=0;
    char filename[255];
    char clientID[255],str[255];
    char deaTH[255][255],live[255][255];
    char client[MAX_CANON];
    int communfd;
    FILE *outp,*inp1,*inp2,*inp;
    char line[25]="\n";
    int listenfd, i = 0;
    current[0].count = 0;
    u_port_t portnumber;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }
    if (sem_init(&semaphoree, 0, 1) == -1) {
        printf("Semaphore failed.\n");
        return -1;
    }
    signal(SIGINT, hello); //ctrl+c
    portnumber = (u_port_t) atoi(argv[1]);
    if ((listenfd = u_open(portnumber)) == -1) {
        perror("Failed to create listening endpoint");
        return 1;
    }
    if (clock_gettime(CLOCK_REALTIME, &starttime) == -1) {
          perror("Failed to get current time");
          return 1;
        }
        printf("SERVER STARTED: 000 ms\n"); //millisecond
    fprintf(stderr, "[%ld]: Waiting for connection on port %d\n",
            (long) getpid(), (int) portnumber);
            output=fopen("living.txt","a+"); //yasayan clıentlar
            output2=fopen("death.txt","a+"); //yasayan clıentlar
    shmctl(shmid, IPC_RMID, NULL);
    if ((shmid = shmget(SHM_KEY_VALUE, sizeof (char), IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            //exit(1);
        }
        if ((gelen = (char *) shmat(shmid, NULL, 1)) == (void *) - 1) {
            perror("shmat");
            //exit(1);
        }

    if ((shmid2 = shmget(SHM_KEY_VALUE, sizeof (char), IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            //exit(1);
        }
        if ((file = (char *) shmat(shmid2, NULL, 1)) == (void *) - 1) {
            perror("shmat");
            //exit(1);
        }
        if ((shmid3 = shmget(SHM_KEY_VALUE, sizeof (char), IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            //exit(1);
        }
        if ((toMe = (char *) shmat(shmid3, NULL, 1)) == (void *) - 1) {
            perror("shmat");
            //exit(1);
        }
    for (;;) {
        if ((communfd = u_accept(listenfd, client, MAX_CANON)) == -1) {
            perror("Failed to accept connection");
            continue;
        }
        fprintf(stderr, "[%ld]:connected to %s\n", (long) getpid(), client);
         if (clock_gettime(CLOCK_REALTIME, &endtime) == -1) {
          perror("Failed to get current time");
          return 1;
        }
        printf("CLIENT TIME: %lld ms \n", (long long)(endtime.tv_sec-starttime.tv_sec)*100); //millisecond
        
        if ((child = fork()) == -1)
            perror("Failed to fork a child");


        if (child == 0) { /* child code */
            if (r_close(listenfd) == -1) {
                fprintf(stderr, "[%ld]:failed to close listenfd: %s\n",
                        (long) getpid(), strerror(errno));
                return 1;
            }
            //burda clıent ıd lerı structa doldur ve shared memory ıle gonder
            if ((gelen = (char *) shmat(shmid, NULL, 1)) == (void *) - 1) {
                    perror("shmat");
                    exit(1);
                }
            if ((file = (char *) shmat(shmid2, NULL, 1)) == (void *) - 1) {
                perror("shmat");
                exit(1);
            }
            bytesread = readblock(communfd, buf, sizeof (buf));
            sprintf(deat," %s", buf);
            sprintf(normal," %s", buf);
            sprintf(gelen," %s", buf); // shared memory e verdım.
            printf("Client ID:  %s\n", gelen);
            if(atoi(gelen)!=0){
            current[0].pid[current[0].count]=atoi(gelen);
            current[0].count++;
            }
            fputs(gelen,output);
            fputs(line,output);
            fclose(output);
            fclose(output2);
            
            while (stop == 0) { // clıent hala baglı ıse bu for da döner.  
                ////
                    getcwd(PtrFirstPath, 255); //bulundugu path
                    walkInPath(PtrFirstPath);
                    r_write(communfd, directories, sizeof (directories));
                    directories[0].numOfFiles=0;
            ////
                bytesread = readblock(communfd, alcam, sizeof (alcam));
                if(strcmp(alcam,"alcam")==0){
                    sprintf(normal,"%d",current[0].pid[0]);
                    
                     inp=fopen(normal,"r");
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
                    printf("yolladım dosyayı: %s\n",normal);
                    fclose(inp);
                    remove(normal); // o dosyayı kaldırıyoruz ki bır daha yollanmıs sanmasın
                    getcwd(PtrFirstPath, 255); //bulundugu path
                    walkInPath(PtrFirstPath);
                    r_write(communfd, directories, sizeof (directories));
                    strcpy(alcam,"almamm");
                    directories[0].numOfFiles=0;
                }
                bytesread = readblock(communfd, buf, sizeof (buf));
                if (clock_gettime(CLOCK_REALTIME, &endtime) == -1) {
                      perror("Failed to get current time");
                      return 1;
                    }
               printf("Time: %lld ms  ", (long long)(endtime.tv_sec-starttime.tv_sec)*100); //millisecond
                printf("Client Request:  %s\n", buf);
                sscanf(buf,"%s %s %s",select,filename,clientID);
                //sprintf(select, "%s", buf);
                if (strcmp(select, "listServer") == 0) {
                    getcwd(PtrFirstPath, 255); //bulundugu path
                    walkInPath(PtrFirstPath);
                    r_write(communfd, directories, sizeof (directories));
                }
                else if (strcmp(select, "lsClients") == 0) {
                    k=0;
                    m=0;
                    s=0;
                    current[0].count=0;
                    inp1=fopen("living.txt","r+");
                    inp2=fopen("death.txt","r+");
                    while(fgets(deaTH[k],255,inp2)!=NULL){
                           ++k; 
                    }
                    while(fgets(live[m],255,inp1)!=NULL){
                           ++m; 
                    }
                    fclose(inp1);
                    fclose(inp2);
                    for(l=0;l<m;++l){ // toplam client sayısı kadar dönecek
                        if(k=!0){
                        for(y=0;y<k;++y){
                            if(strcmp(deaTH[y],live[l])!=0){
                                strcpy(sonuc[s],live[l]);
                                   ++s;
                            } 
                        }
                       }else{
                            s=0;
                            for(y=0;y<m;++y){
                                    strcpy(sonuc[s],live[y]);
                                       ++s;
                                        
                            }
                            l=m; // for a gırmesın dıye
                            }
                    }
                    for(y=0;y<=100;++y){
                    current[0].pid[y]=0;
                    }
                    for(y=0;y<=s;++y){
                    current[0].pid[y]=atoi(sonuc[y]);
                    current[0].count=y;
                    }
                    r_write(communfd, current, sizeof (current));
                    //client id ,servera o ana kadar baglı olan clientların idleri
                } else if (strcmp(select, "sendFile") == 0) {
                    bytesread = readblock(communfd, dosya, sizeof (dosya));
                    printf("%s\n", dosya);
                    sprintf(file, "%s", dosya);
                    printf("%s\n", filename);
                    outp=fopen(clientID,"w+");
                    fprintf(outp,"%s \n",filename);// ilk satırda gercek ısım var
                    fputs(dosya, outp);
                    fclose(outp);
                    //client-client file yollama komutu. id yoksa serverın local directorysıne gider dosya
                } else if (strcmp(select, "stop") == 0) {
                    printf("Client killed with ^C. \n");
                    //ayrıca bu clıentı guncel clıent arrayınden sılmek lazım.
                    stop = 1;
                    fputs(deat,output2); // ölen client yazılır
                    fputs(line,output2);
                } else /* default: */ {
                }

            } //clıent 1 den fazla komut gırebılır, sonlanana kadar burda döner
            return 0;
        }
        if (r_close(communfd) == -1) /* parent code */
            fprintf(stderr, "[%ld]:failed to close communfd: %s\n",
                (long) getpid(), strerror(errno));
        while (r_waitpid(-1, NULL, WNOHANG) > 0); /* clean up zombies */

    }//buyuk for un parantezi
}

void hello(int signum) {
    printf("\nInterrup with ctrl+c ,program is closed.\n");
    printf("Interrup with ctrl+c, program is closed. \n");
    remove("living.txt");
    remove("death.txt");
    while (current[0].count > 0) {
            printf("Killed client PID:%d \n",current[0].pid[current[0].count-1]);
            kill(current[0].pid[current[0].count-1], SIGINT);
            --current[0].count;
        }
    fclose(output);
    fclose(output2);
    exit(0);

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
                    strcpy(directories[0].filesNames[directories[0].numOfFiles], strFilePath);
                    // printf( "%s\n", directories[0].filesNames[directories[0].numOfFiles]);

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
//NURTEN
int isdirectory(char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1)
        return 0;
    else
        return S_ISDIR(statbuf.st_mode);
}
