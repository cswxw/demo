/*
 *  passwd_demo
 *  a simple passwd utility for showing PAM usage
 *  derived from shadow
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

#define SHADOW_PAM_CONVERSATION misc_conv

static struct pam_conv conv = {
	SHADOW_PAM_CONVERSATION,
	NULL
};

void do_pam_passwd (const char *user, bool silent, bool change_expired)
{
	pam_handle_t *pamh = NULL;
	int flags = 0, ret;

	if (silent)
		flags |= PAM_SILENT;
	if (change_expired)
		flags |= PAM_CHANGE_EXPIRED_AUTHTOK;

	ret = pam_start ("passwd", user, &conv, &pamh);
	if (ret != PAM_SUCCESS) {
		fprintf (stderr,
			 "passwd: pam_start() failed, error %d\n", ret);
		exit (10);	/* XXX */
	}

	ret = pam_chauthtok (pamh, flags);
	if (ret != PAM_SUCCESS) {
		fprintf (stderr, "passwd: %s\n", pam_strerror (pamh, ret));
		fputs ("passwd: password unchanged\n", stderr);
		pam_end (pamh, ret);
		exit (10);	/* XXX */
	}

	fputs ("passwd: password updated successfully\n", stderr);
	(void) pam_end (pamh, PAM_SUCCESS);
}

int main(int argc, char *argv[])
{
	if (argc !=2 ) {
		fprintf(stderr, "usage: %s user\n", argv[0]);
		return -1;
	}
	char *user = argv[1];
	do_pam_passwd(user, false, false);
	return 0;
}




