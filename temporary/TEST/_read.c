#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  int fd;

  if ((fd = open(argv[1], O_RDONLY)) == -1)
    return (1);

  while (1)
    {
      char buffer[1024];
      int n;

      lseek(fd, 0L, SEEK_SET);

      memset(buffer, 0x0, sizeof (buffer));

      if ((n = read(fd, buffer, sizeof (buffer))) < 0)
	return (1);

      printf("%s (%d)\n", buffer, n);

      sleep(3);
    }

  return (0);
}
