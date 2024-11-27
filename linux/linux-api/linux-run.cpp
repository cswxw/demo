bool UnixRun(char *filename, char *arg, bool hide, bool wait)
{
typedef int pid_t;
	TOKEN_LIST *t;
	UINT ret;
	// Validate arguments
	if (filename == NULL)
	{
		return false;
	}
	if (arg == NULL)
	{
		arg = "";
	}

	// Create a child process
	ret = fork();
	if (ret == -1)
	{
		// Child process creation failure
		return false;
	}

	if (ret == 0)
	{
		Print("", filename, arg);
		// Child process
		if (hide)
		{
			// Close the standard I/O
			UnixCloseIO();
		}

		t = ParseToken(arg, " ");
		if (t == NULL)
		{
			AbortExit();
		}
		else
		{
			char **args;
			UINT num_args;
			UINT i;
			num_args = t->NumTokens + 2;
			args = ZeroMalloc(sizeof(char *) * num_args);
			args[0] = filename;
			for (i = 1;i < num_args - 1;i++)
			{
				args[i] = t->Token[i - 1];
			}
			execvp(filename, args);
			AbortExit();
		}
	}
	else
	{
		// Parent process
		pid_t pid = (pid_t)ret;

		if (wait)
		{
			int status = 0;
			// Wait for the termination of the child process
			if (waitpid(pid, &status, 0) == -1)
			{
				return false;
			}

			if (WEXITSTATUS(status) == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return true;
	}
}

// Close the standard I/O
void UnixCloseIO()
{
	static bool close_io_first = false;

	// Execute only once
	if (close_io_first)
	{
		return;
	}
	else
	{
		close(0);
		close(1);
		close(2);
		open("/dev/null", O_RDWR);
		dup2(0, 1);
		dup2(0, 2);
		close_io_first = false;
	}
}

// Stop system (abnormal termination)
void AbortExit()
{
#ifdef	OS_WIN32
	_exit(1);
#else	// OS_WIN32

#ifdef	RLIMIT_CORE
	UnixSetResourceLimit(RLIMIT_CORE, 0);
#endif	// RLIMIT_CORE

	abort();
#endif	// OS_WIN32
}


// Adjust the upper limit of resources that may be occupied
//#define	UNIX_MAX_MEMORY					(2147483648UL)	// Maximum memory capacity //2GB
//UnixSetResourceLimit(RLIMIT_CORE, UNIX_MAX_MEMORY); //内核转存文件的最大长度
void UnixSetResourceLimit(UINT id, UINT value)
{
	struct rlimit t;
	UINT hard_limit;

	Zero(&t, sizeof(t));
	getrlimit(id, &t);

	hard_limit = t.rlim_max;

	Zero(&t, sizeof(t));
	t.rlim_cur = MIN(value, hard_limit);
	t.rlim_max = hard_limit;
	setrlimit(id, &t);

	Zero(&t, sizeof(t));
	t.rlim_cur = value;
	t.rlim_max = value;
	setrlimit(id, &t);
}

