#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  int fd;
  int i = 0;

  if ((fd = open(argv[1], O_WRONLY | O_TRUNC)) == -1)
    return (1);

  while (1)
    {
      char buffer[1024];
      int n;

      lseek(fd, 0L, SEEK_SET);

      memset(buffer, 0x0, sizeof (buffer));

      sprintf(buffer, "test-%u", i);

      if ((n = write(fd, buffer, strlen(buffer))) == -1)
	return (1);

      printf("%s (%d)\n", buffer, n);

      sleep(1);

      i++;
    }

  return (0);
}
