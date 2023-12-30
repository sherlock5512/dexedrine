#include <bsd/libutil.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>
#include <unistd.h>
#include <linux/limits.h>

void handleSigs(int sig);

#define _cleanup_(f) __attribute__((cleanup(f)))

int main(int argc, char *argv[]) {
	_cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
	_cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
	_cleanup_(sd_bus_message_unrefp) sd_bus_message *msg = NULL;
	int fd = -1;
	int r;

	struct pidfh *pfh;
	pid_t otherpid;

	uid_t uid = getuid();
	char pidfilePath[PATH_MAX];

	/* Get the path to our pidfile in the users runtime directory */
	r = snprintf(
		pidfilePath,
		PATH_MAX - 1 ,
		"/run/user/%i/Dexedrine.pid", uid
	);

	if (r > PATH_MAX) {
		fprintf(
			stderr,
			"Path generation failed, pidfilePath too long\n"
		);
		exit(EXIT_FAILURE);
	}

	/* Attempt to open/create out pidfile */
	pfh = pidfile_open(pidfilePath, 0600, &otherpid);
	if (pfh == NULL) {
		switch (errno) {
			case EEXIST:
				fprintf(
					stderr,
					"Found an existing process at %jd, terminating\n",
					(intmax_t)otherpid
				);
				kill(otherpid,SIGINT);
				exit(EXIT_SUCCESS);
			case EINVAL:
				fprintf(
					stderr,
					"Pidfile at %s is locked, but the pid is invalid.\n",
					pidfilePath
				);
				exit(EXIT_FAILURE);
			default:
				fprintf(
					stderr,
					"Failed to create/write pidfile %s\n",
					strerror(errno)
				);
				exit(EXIT_FAILURE);
		}
	}

	/* since pfh is not null we shall write to it. */
	pidfile_write(pfh);

	/* Catch sigint */
	(void)signal(SIGINT,handleSigs);


	/*
	 * WARNING:
	 * after this line you must always exit by `goto finish;`
	 */

	/* connect to system bus */
	r = sd_bus_open_system(&bus);
	if (r < 0) {
		fprintf(
			stderr,
			"Failed to connect to system bus: %s\n",
			strerror(-r)
		);
		goto finish;
	}

	/* call the Inhibit method and store fd */
	r = sd_bus_call_method(
		bus,
		"org.freedesktop.login1",
		"/org/freedesktop/login1",
		"org.freedesktop.login1.Manager",
		"Inhibit",
		&error,
		&msg,
		"ssss",
		/* What is being inhibited */
		"idle:sleep:handle-lid-switch",
		/* Who is inhibiting */
		"Dexedrine",
		/* Why are we inhibiting */
		"Requested by user",
		/* How are we inhibiting  */
		"block"
	);
	if (r < 0) {
		fprintf(
			stderr,
			"Failed to issuse method call: %s\n",
			error.message
		);
		goto finish;
	}

	r = sd_bus_message_read_basic(msg, 'h', &fd);
	if (r < 0) {
		fprintf(
			stderr,
			"Failed to read file descriptor: %s\n",
			strerror(-r)
		);
		goto finish;
	}
	fd = dup(fd);

	printf("Waiting for SIGINT...\n");
	(void)pause(); /* Wait for any signal */
	printf("Recieved signal. Exiting\n");

	/* Close the file descriptor nicely */
	r = close(fd);
	if (r == -1) {
		fprintf(
			stderr,
			"Failed to close file descriptor: %s\n",
			strerror(errno));
	}

	/* Cleanup any mess before exiting */
finish:

	pidfile_remove(pfh);
	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*
 * While I don't want to actually do anything based on the signals
 * I do need to make sure that they don't crash the program.
 */
void handleSigs(int sig)
{
	return;
}
