#define _POSIX_C_SOURCE 200809L
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define STACK_SIZE 1000
#define PI 3.141592653589793

/*** Globals ***/

int WIDTH;
int HEIGHT;
char **canvas;
struct termios orig;

/*** Types ***/

typedef struct
{
	float x, y, angle;
} TurtleState;

/*** Drawing ***/

void
init_canvas ()
{
	struct winsize w;
	ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
	WIDTH = w.ws_col;
	HEIGHT = w.ws_row;
	canvas = malloc (HEIGHT * sizeof (char *));
	for (int y = 0; y < HEIGHT; y++)
		canvas[y] = malloc (WIDTH * sizeof (char));
}

/**
 * @brief Draw a straight line onto the canvas using Bresenhams algorithm
 *
 * @param x0 X coord start point
 * @param y0 Y coord start point
 * @param x1 X coord end point
 * @param y1 Y coord end point
 **/
void
draw_line (int x0, int y0, int x1, int y1)
{
	int delta_x = abs (x1 - x0), step_x = x0 < x1 ? 1 : -1;
	int delta_y = -abs (y1 - y0), step_y = y0 < y1 ? 1 : -1;
	int error = delta_y + delta_x;
	while (1)
	{
		if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
			canvas[y0][x0] = '*';
		if (x0 == x1 && y0 == y1)
			break;
		int diagonal_error = error * 2;
		if (diagonal_error >= delta_y)
		{
			error += delta_y;
			x0 += step_x;
		}
		if (diagonal_error <= delta_x)
		{
			error += delta_x;
			y0 += step_y;
		}
	}
}

void
draw_line_centred (int x0, int y0, int x1, int y1)
{
	draw_line (x0 * 2 + WIDTH / 2, y0 + HEIGHT / 2, x1 * 2 + WIDTH / 2,
			   y1 + HEIGHT / 2);
}

void
clear_canvas ()
{
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			canvas[y][x] = ' ';
}

void
print_canvas ()
{
	printf ("\033[2J\033[H");
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
			putchar (canvas[y][x]);
		printf ("\r\n");
	}
}

/*** L-System ***/

char *
rewrite (char *axiom, char *rule_find, char *rule_replace, int iterations)
{
	char *current = strdup (axiom);
	for (int i = 0; i < iterations; i++)
	{
		char *next = malloc (strlen (current) * strlen (rule_replace) + 1);
		next[0] = '\0';
		for (size_t j = 0; j < strlen (current); j++)
		{
			if (current[j] == rule_find[0])
				strcat (next, rule_replace);
			else
				strncat (next, &current[j], 1);
		}
		free (current);
		current = next;
	}
	return current;
}

void
render_with_branches (char *lstring, float step_size, float turn_angle)
{
	TurtleState t1;

	t1.x = 0;
	t1.y = 0;
	float turn_angle_radians = turn_angle * (PI / 180);
	t1.angle = PI / 2; // Start pointing upward (90 degrees)

	for (size_t i = 0; i < strlen (lstring); i++)
	{
		char c = *(lstring + i);

		float end_x = t1.x + step_size * cos (t1.angle);
		float end_y = t1.y - step_size * sin (t1.angle);

		if (c == 'F')
		{
			draw_line_centred ((int)t1.x, (int)t1.y, (int)end_x, (int)end_y);
			t1.x = end_x;
			t1.y = end_y;
		}
		else if (c == '-')
		{
			t1.angle -= turn_angle_radians;
		}
		else if (c == '+')
		{
			t1.angle += turn_angle_radians;
		}
	};
}

/*** Terminal ***/

void
disable_raw_mode (void)
{
	if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &orig) == -1)
		perror ("error while trying to disable raw mode");
}

void
enable_raw_mode (void)
{
	struct termios raw;
	if (tcgetattr (STDIN_FILENO, &raw) == -1)
		perror ("tcgetattr");
	if (tcgetattr (STDIN_FILENO, &orig) == -1)
		perror ("tcgetattr");
	atexit (&disable_raw_mode);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		perror ("tcsetattr");
}

void
read_input ()
{
	char c;
	while (read (STDIN_FILENO, &c, 1) == 1)
	{
		if (c == 'q')
		{
			disable_raw_mode ();
			break;
		}
	}
}

void
init ()
{
	init_canvas ();
	enable_raw_mode ();
	clear_canvas ();
}

/*** main ***/

int
main ()
{
	init ();

	char *lstring = rewrite ("F", "F", "F-F++F-F-----++FF+-F", 5);
	render_with_branches (lstring, 5, 60);
	free (lstring);

	print_canvas ();
	read_input ();
	return 0;
}
