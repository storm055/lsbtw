#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void mode_string(mode_t *mod, char *str) {

    if (S_ISDIR(*mod))       str[0] = 'd';
    else if (S_ISLNK(*mod))  str[0] = 'l';
    else if (S_ISCHR(*mod))  str[0] = 'c';
    else if (S_ISBLK(*mod))  str[0] = 'b';
    else if (S_ISFIFO(*mod)) str[0] = 'p';
    else if (S_ISSOCK(*mod)) str[0] = 's';
    else                     str[0] = '-';

    str[1] = (*mod & S_IRUSR) ? 'r' : '-';
    str[2] = (*mod & S_IWUSR) ? 'w' : '-';
    str[3] = (*mod & S_IXUSR) ? 'x' : '-';

    str[4] = (*mod & S_IRGRP) ? 'r' : '-';
    str[5] = (*mod & S_IWGRP) ? 'w' : '-';
    str[6] = (*mod & S_IXGRP) ? 'x' : '-';

    str[7] = (*mod & S_IROTH) ? 'r' : '-';
    str[8] = (*mod & S_IWOTH) ? 'w' : '-';
    str[9] = (*mod & S_IXOTH) ? 'x' : '-';

    str[10] = '\0';
}

void print_long(const char *dir, const char *name) {

    char fullpath[4096];

    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);

    struct stat st;

    if (lstat(fullpath, &st) < 0) {
        perror(name);
        return;
    }

    char modes[11];

    mode_string(&st.st_mode, modes);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    const char *user = pw ? pw->pw_name : "?";
    const char *group = gr ? gr->gr_name : "?";

    printf("%s %s %s %s\n", modes, user, group, name);
}

int show_all = 0;

int main(int argc, char *argv[]) {

    int opt;

    while ((opt = getopt(argc, argv, "a")) != -1) {

        switch (opt) {

            case 'a':
                show_all = 1;
                break;

            default:
                fprintf(stderr, "usage: %s [-a] [path]\n", argv[0]);
                return 1;
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    DIR *dir = opendir(path);

    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (!show_all && entry->d_name[0] == '.')
            continue;

        print_long(path, entry->d_name);
    }

    closedir(dir);

    return 0;
}

