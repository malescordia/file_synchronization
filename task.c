#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 256
#define LOG_FILE "logs.txt"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define PURPLE "\033[35m"
#define BLUE "\033[36m"
#define R "\033[0m" /* reset colors */

void	log_to_file(const char *log_message)
{
	FILE	*file;

	file = fopen(LOG_FILE, "a");
	if (file == NULL)
	{
		printf(RED "Error opening log file.\n" R);
		return ;
	}
	fprintf(file, "%s\n", log_message);
	fclose(file);
}

void	sync_folders(const char *src, const char *dest)
{
	DIR				*dir;
	struct dirent	*entry;
	struct stat		stat_buf;
	char			src_path[MAX_PATH_LENGTH];
	char			dest_path[MAX_PATH_LENGTH];

	if ((dir = opendir(src)) == NULL)
	{
		printf(RED "Error opening source folder.\n" R);
		return ;
	}
	/* Iterate over files and directories in the source folder */
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue ; /* Skip current and parent directories */
		snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
		snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);
		if (stat(src_path, &stat_buf) == -1)
		{
			printf(RED "Error getting file stats for %s.\n" R, src_path);
			continue ;
		}
		/* Check if entry is a file or directory */
		if (S_ISDIR(stat_buf.st_mode))
			sync_folders(src_path, dest_path);
		else
		{
			/* File found, check if it exists in destination folder */
			if (access(dest_path, F_OK) != -1)
			{
				/* File exists in destination, remove it before copying */
				remove(dest_path);
				printf(YELLOW "File %s removed from destination.\n" R, entry->d_name);
				log_to_file(entry->d_name);
			}
			/* Copy file from source to destination */
			if (link(src_path, dest_path) == -1)
				printf(RED "Error copying file %s to destination.\n" R, entry->d_name);
			else
			{
				printf(GREEN "File %s copied to destination.\n" R, entry->d_name);
				log_to_file(entry->d_name);
			}
		}
	}
	closedir(dir);
}

int	main(int ac, char **av)
{
	const char	*src = av[1];
	const char	*dest = av[2];
	int			sync_interval;

	if (ac != 4)
	{
		printf(PURPLE "Usage: %s <source_folder> <destination_folder> <sync_interval>\n" R, av[0]);
		return (1);
	}
	sync_interval = atoi(av[3]);
	if (access(src, F_OK) == -1 || access(dest, F_OK) == -1)
	{
		printf(RED "Source or destination folder does not exist.\n" R);
		return (1);
	}
	while (42)
	{
		sync_folders(src, dest);
		sleep(sync_interval);
	}
	return (0);
}
