#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#define DEFAULT_BLOCK_SIZE 512
#define MAX_PATH 512

void print_help(bool extended);
void exit_with_error(char* message);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void cleanup();
void init_nc();

void draw_status_bar();

bool arg_found = false;

WINDOW *my_window;

bool recursive = false;
int block_size = DEFAULT_BLOCK_SIZE;
char inputpath[MAX_PATH] = {0};
char outputpath[MAX_PATH] = {0};

int main (int argc, char** argv) {

	int opt = 0;
	const char* short_options = "hrb:";

	const struct option long_options[] = {
		{ "help",		no_argument,		NULL,	'h' },
		{ "recursive",	no_argument,		NULL,	'r' },
		{ "block-size",	required_argument,	NULL,	'b' },
		{ NULL, 		no_argument, 		NULL,	 0 }
	};

	do {
		opt = getopt_long(argc, argv, short_options, long_options, NULL);

		//printf("optarg: %s\n", optarg);

		switch (opt) {
			case 'r':
				arg_found = true;
				recursive = true;
				break;
			case 'b':
				arg_found = true;
				block_size = optarg ? 
						(atoi(optarg) > 0 ? 
							atoi(optarg) : 
							DEFAULT_BLOCK_SIZE) : 
						DEFAULT_BLOCK_SIZE;
				break;
			case 'h':
				print_help(true);
				return 0;
				break;
			default:
				if (argc == 1) {
					print_help(false);
					return -1;
				}
		}
	} while (opt != -1);

	if (argc >= 3) {
		sprintf(inputpath, "%s", argv[argc-1]);
		sprintf(outputpath, "%s", argv[argc-2]);
	}

	if (!(strlen(inputpath) && strlen(outputpath)))
		exit_with_error("No paths.");

	struct stat filestat;
	if (stat(inputpath, &filestat) < 0)
		exit_with_error("Not a valid input path.");
	if (stat(outputpath, &filestat) < 0) 
		exit_with_error("Not a valid output path.");

	// Start ncurses here
	init_nc();

    for (int i = 1; i < 80; ++i)
    {
    	mvwprintw (my_window, 2, i, "=>");
    	wrefresh(my_window);
    	usleep(1000 * 10);
    }

    refresh();
	getchar();
	return 0;
}

void print_help(bool extended) {
	printf("Usage: cpext [options] <inputpath> <outputpath>");

	if (extended) {
		printf("\n\n");

		printf("Options:\n");

		printf("--help\t\t-h\tPrints this output.\n");
		printf("--recursive\t-r\tCopies files and directories recursive.\n");
	}

	printf("\n");
}

void exit_with_error(char* message) {
	printf("Error: ");
	printf(message);
	printf("\n");

	print_help(false);

	exit(-1);
}

void init_nc() {
	atexit(cleanup);
	// init curses
	initscr();

	refresh();
	my_window = create_newwin(10, COLS, 0, 0);
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');

	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');

	wrefresh(local_win);
	delwin(local_win);
}

void cleanup()
{
	destroy_win(my_window);
	endwin();
}