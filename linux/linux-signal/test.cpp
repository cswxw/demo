static void
setup_signals(void)
{
  struct sigaction action;        /* posix signal structure */
         
  /*
   * Setup signal handlers
   */
  (void) memset((void *) &action, 0, sizeof(action));
  action.sa_handler = su_sighandler;
#ifdef SA_RESETHAND
  action.sa_flags = SA_RESETHAND;
#endif
  (void) sigaction(SIGILL, &action, NULL);
  (void) sigaction(SIGTRAP, &action, NULL);
  (void) sigaction(SIGBUS, &action, NULL);
  (void) sigaction(SIGSEGV, &action, NULL);
  action.sa_handler = SIG_IGN;
  action.sa_flags = 0;
  (void) sigaction(SIGTERM, &action, NULL);
  (void) sigaction(SIGHUP, &action, NULL);
  (void) sigaction(SIGINT, &action, NULL);
  (void) sigaction(SIGQUIT, &action, NULL);
}

static void
su_sighandler(int sig)
{
#ifndef SA_RESETHAND
        /* emulate the behaviour of the SA_RESETHAND flag */
        if ( sig == SIGILL || sig == SIGTRAP || sig == SIGBUS || sig = SIGSERV ) {
		struct sigaction sa;
		memset(&sa, '\0', sizeof(sa));
		sa.sa_handler = SIG_DFL;
                sigaction(sig, &sa, NULL);
	}
#endif
        if (sig > 0) {
                _exit(sig);
        }
}

