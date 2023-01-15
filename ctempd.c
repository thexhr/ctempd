/* Code to set color temperature was written by Ted Unangst as is licensed as
 * public domain, do as you wish
 * https://flak.tedunangst.com/post/sct-set-color-temperature
 *
 * The rest of the code was written by Matthias Schmidt <xhr@giessen.ccc.de>
 * and is also public domain.
 */
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include <sys/syslimits.h>
#include <sys/wait.h>

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ctempd.h"

static int fg = 0;
static int verbose = 0;
static Display *dpy;

void show_usage(void);
void set_color(int);
void daemonize(void);
int sandbox(void);

void
show_usage(void)
{
	fprintf(stderr, "ctempd [-fhv] [temp]\n");
	exit(1);
}

void
set_color(int temp)
{
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);

	XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

	double ratio = temp % 500 / 500.0;

	double gammar = AVG(r);
	double gammag = AVG(g);
	double gammab = AVG(b);

	for (int c = 0; c < res->ncrtc; c++) {
		int crtcxid = res->crtcs[c];

		int size = XRRGetCrtcGammaSize(dpy, crtcxid);

		XRRCrtcGamma *crtc_gamma = XRRAllocGamma(size);

		for (int i = 0; i < size; i++) {
			double g = 65535.0 * i / size;
			crtc_gamma->red[i] = g * gammar;
			crtc_gamma->green[i] = g * gammag;
			crtc_gamma->blue[i] = g * gammab;
		}
		XRRSetCrtcGamma(dpy, crtcxid, crtc_gamma);

		XFree(crtc_gamma);
	}
}

#define TEMP_MORNING	6200
#define TEMP_LUNCH		6500
#define TEMP_EVENING	4500
#define TEMP_NIGHT		3200
#define TEMP_MIGNIGHT	3000

void
daemonize(void)
{
	struct tm tm;
	time_t t;
	int hour = 0;
	int temp = TEMP_DEFAULT;

	while (1) {
		t = time(NULL);
		tm = *localtime(&t);
		hour = tm.tm_hour;

		if (hour >= 0 && hour < 7)
			temp = TEMP_MIGNIGHT;
		else if (hour >= 7 && hour < 12)
			temp = TEMP_MORNING;
		else if (hour >= 12 && hour < 17)
			temp = TEMP_LUNCH;
		else if (hour >= 17 && hour < 22)
			temp = TEMP_EVENING;
		else if (hour >= 22)
			temp = TEMP_NIGHT;

		if (verbose)
			printf("Current time: %d. Set color temperature to %d\n", hour, temp);

		set_color(temp);

		sleep(SLEEP);
	}
}

int
main(int argc, char **argv)
{
	char *ep;
	unsigned long ulval;
	pid_t pid;
	int temp = TEMP_DEFAULT;
	int ch;

	while ((ch = getopt(argc, argv, "fhv")) != -1) {
		switch (ch) {
		case 'f':
			fg = 1;
			break;
		case 'h':
			show_usage();
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			show_usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if ((dpy = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "Cannot connect to X server\n");
		return 1;
	}

	sandbox();

	if (argc >= 1) {
		errno = 0;
		ulval = strtol(argv[0], &ep, 10);
		if (argv[0][0] == '\0' || *ep != '\0') {
			fprintf(stderr, "Not a valid number\n");
			return 1;
		}
		if (errno == ERANGE && ulval == ULONG_MAX) {
			fprintf(stderr, "Out of range\n");
			return 1;
		}
		temp = ulval;
		if (temp < 1000 || temp > 10000) {
			temp = TEMP_DEFAULT;
		}

		if (verbose)
			printf("Set color temperature to %d\n", temp);

		set_color(temp);
		return 0;
	}

	pid = fork();
	switch (pid) {
		case -1:
			fprintf(stderr, "Cannot fork\n");
			return 1;
		case 0:
			if (!fg)
				daemon(0, 0);
			daemonize();
	}

	wait(NULL);

	return 0;
}

int
sandbox(void)
{
#ifdef __OpenBSD__
	char hf[PATH_MAX], *env;

	if (unveil("/usr/X11R6/", "r") == -1)
		return 1;
	if (unveil("/tmp/.X11-unix/", "rw") == -1)
		return 1;
	if (unveil("/etc/hosts", "r") == -1)
		return 1;
	if (unveil("/etc/resolv.conf", "r") == -1)
		return 1;
	if ((env = getenv("HOME"))) {
		if (snprintf(hf, sizeof hf, "%s/.Xauthority", env) <= (int)sizeof(hf)) {
			if (unveil(hf, "r") == -1)
				return 1;
		}
	}

	if (unveil(NULL, NULL) == -1)
		return 1;

	if (pledge("stdio proc unix", NULL) == -1)
		return 1;
#endif /* __OpenBSD__ */

	return 0;
}
