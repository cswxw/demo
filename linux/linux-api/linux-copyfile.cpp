static int copy_file(const char *source_file, const char *dest_file,  int force_flag) {
	#define DESTINATION_FILE_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
	int buffer_size = 1024;
	 int source_file_descriptor, destination_file_descriptor, numberOfBitsRead, numberOfBitsWritten;

	 source_file_descriptor = open(source_file, O_RDONLY);

	 
	 if(source_file_descriptor == -1)
	 {
		perror("Unable to open source file for reading");
		return -1;
	 }

	
	 if(force_flag)
	 {
		destination_file_descriptor = open(dest_file, O_CREAT | O_TRUNC | O_WRONLY, DESTINATION_FILE_MODE);
		if(destination_file_descriptor == -1)
		{
			close(source_file_descriptor);
			perror("Unable to open destination file for writing");
			return -1;
		}
	 }
	 else
	 {
		destination_file_descriptor = open(dest_file, O_EXCL | O_CREAT | O_WRONLY, DESTINATION_FILE_MODE);
		if(destination_file_descriptor == -1)
		{
			close(source_file_descriptor);
			perror("Unable to open destination file for writing");
			return -1;
		}
	 }
	 char* buffer = malloc(sizeof(char) * buffer_size);
	 if(buffer == NULL)
	 {
		perror("Unable to allocate memory");
		close(source_file_descriptor);
		close(destination_file_descriptor);
		return -1;
	 }

	 while((numberOfBitsRead = read(source_file_descriptor, buffer, buffer_size)) != 0)
	 {
		if((numberOfBitsWritten = write(destination_file_descriptor, buffer, numberOfBitsRead)) == -1 || numberOfBitsRead != numberOfBitsWritten)
		{
			perror("unable to write to destination file");
			close(source_file_descriptor);
			close(destination_file_descriptor);
			free(buffer);
			return -1;
	 	}
	 }

	if(close(source_file_descriptor) == -1) {
		perror("Unable to close source file");
		free(buffer);
		close(destination_file_descriptor);
		return -1;
	}

	if(close(destination_file_descriptor) == -1) {
		perror("Unable to close destination file");
		free(buffer);
		return -1;
	}
	free(buffer);
	INFO("File %s was copied to %s \n", source_file, dest_file);
	return 0;
}



int copyone(char * comestr,char * endStr){
	FILE *pcomestr = fopen(comestr,"r");
	if(pcomestr == NULL){
		printf("open %s error !\n",pcomestr);  //注意 ： 用指针输出字符串的，
        return -1;
	}

	FILE *pend = fopen(endStr,"w");
	if(pend == NULL){
		printf("open %s error !\n",endStr);
		fclose(pcomestr);
        return -1;
	}

	int ch;
    while((ch=getc(pcomestr))!=EOF) //逐个字符去读取文件内容，
    {
        fputc(ch , pend);
    }
    fclose(pcomestr);
    fclose(pend);
	return 0;
}

