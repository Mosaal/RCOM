```
// READ FILE
FILE *fileRead;
fileRead = fopen(argv[1], "rb");
long int size = getFileSize(fileRead);

unsigned char buffer[size];
size_t bytesRead = fread(buffer, 1, size, fileRead);

printf("BytesRead = %lu\n", bytesRead);

int j;
for (j = 0; j < size; j++)
	printf("0x%02x; ", buffer[j]);
printf("\n");

fclose(fileRead);

// WRITE FILE
FILE *fileWrite;
fileWrite = fopen(argv[2], "wb");

size_t bytesWritten = fwrite(buffer, 1, size, fileWrite);

printf("BytesWritten = %lu\n", bytesWritten);

fclose(fileWrite);
```
