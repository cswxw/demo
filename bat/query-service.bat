@echo off
for /f "tokens=1,2 delims=:"  %%i  in ('sc query')do (
	if "%%i" == "SERVICE_NAME" (
		for /f "tokens=1,2* delims=:" %%k in ('sc qc %%j') do (
			rem echo %%k
			if "%%k" == "        BINARY_PATH_NAME   " (
				echo [ServiceName]: %%j [ProgramName]: %%l%%m
			)
			
		)
	)
	if "%%i" == "DISPLAY_NAME" (
		echo [DISPLAY_NAME]: %%j
		echo. 
	)
	
)
