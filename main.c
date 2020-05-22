#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(void)
{
    struct stat buf;
    DIR *d = opendir(".");
    struct dirent *dir;
    struct group *grp;
    struct passwd *pwd;
    struct tm *tmp;
    char outstr[200];
    int cnt = 0;
    int total = 0;
    if (!d)
    {
        perror("diropen");
        exit(1);
    }
    while (readdir(d) != NULL)
        cnt++;
    closedir(d);
    d = opendir(".");
    if (!d)
    {
        perror("diropen");
        exit(1);
    }
    int len = cnt;
    char **list_files = (char **)malloc(sizeof(char *) * (cnt + 1));
    list_files[cnt] = NULL;
    cnt = 0;
    while ((dir = readdir(d)) != NULL)
    {
        list_files[cnt] = (char *)malloc(sizeof(char) * (strlen(dir->d_name) + 1));
        strcpy(list_files[cnt], dir->d_name);
        cnt++;
    }
    char *swp;
    while (len)
    {
        cnt = 0;
        while (list_files[cnt])
        {
            if (list_files[cnt + 1] && strcasecmp(list_files[cnt], list_files[cnt + 1]) > 0)
            {
                swp = list_files[cnt];
                list_files[cnt] = list_files[cnt + 1];
                list_files[cnt + 1] = swp;
            }
            cnt++;
        }
        len--;
    }
    cnt = 0;
    while (list_files[cnt])
    {
        if (lstat(list_files[cnt], &buf) == -1)
            printf("lstat_error");
        total = total + buf.st_blocks;
        cnt++;
    }
    printf("total %i\n", total/2);
    cnt = 0;
    while (list_files[cnt])
    {
        if (lstat(list_files[cnt], &buf) == -1)
            printf("lstat_error");
        tmp = localtime(&buf.st_mtime);
        if (tmp == NULL)
        {
            perror("localtime");
            exit(EXIT_FAILURE);
        }
        if (strftime(outstr, sizeof(outstr), "%b %d %R", tmp) == 0)
        {
            fprintf(stderr, "strftime returned 0");
            exit(EXIT_FAILURE);
        }
        printf( (S_ISDIR(buf.st_mode)) ? "d" : "-");
        printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
        printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
        printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
        printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
        printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
        printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
        printf( (buf.st_mode & S_IROTH) ? "r" : "-");
        printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
        printf( (buf.st_mode & S_IXOTH) ? "x" : "-");
        printf(" ");
        printf("%3ld ", buf.st_nlink);
        pwd = getpwuid(buf.st_uid);
        printf("%s ", pwd->pw_name);
        grp = getgrgid(buf.st_gid);
        printf("%s ", grp->gr_name);
        printf("%6ld ", buf.st_size);
        printf("%s ", outstr);
        printf("%s", list_files[cnt]);
        printf("\n");
        cnt++;
    }
    cnt = 0;
    while (list_files[cnt])
    {
        free(list_files[cnt]);
        cnt++;
    }
    free(list_files);
    return 0;
}