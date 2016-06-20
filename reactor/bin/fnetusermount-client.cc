#include <unistd.h>
#include <boost/asio.hpp>

#define HAVE_MSGHDR_MSG_CONTROL 1
static
ssize_t
write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;
#else
    msg.msg_accrights = (caddr_t) &sendfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    return(sendmsg(fd, &msg, 0));
}
/* end write_fd */

#define err_quit(a) return -1
static
ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    int             newfd;
    msg.msg_accrights = (caddr_t) &newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
        if (cmptr->cmsg_level != SOL_SOCKET)
            err_quit("control level != SOL_SOCKET");
        if (cmptr->cmsg_type != SCM_RIGHTS)
            err_quit("control type != SCM_RIGHTS");
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
#else
/* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1;       /* descriptor was not passed */
/* *INDENT-ON* */
#endif

    return(n);
}
/* end read_fd */


/* Impersonate fusermount, forward requests to an unix socket
*/
using namespace boost::asio;
int main(int argc, char** argv)
{
  // We are impersonating fusermount, so we cant take arguments of our own
  const char* server_socket_path = "/tmp/fnetusermount.sock";
  const char* sspenv = getenv("FNETUSERMOUNT_SOCKET_PATH");
  bool debug = getenv("FNETUSERMOUNT_DEBUG");
  if (sspenv)
    server_socket_path = sspenv;
  std::string args;
  for (int i=1; i<argc; ++i)
  {
    args += argv[i];
    args += ' ';
  }
  args = args.substr(0, args.size()-1);
  args += '\n';
  io_service io;
  local::stream_protocol::socket s(io);
  s.connect(server_socket_path);
  write(s, boost::asio::buffer(args.data(), args.size()));
  int fd = s.native_handle();
  char data[1024];
  int recvfd = -1;
  size_t res = read_fd(fd, data, 1024, &recvfd);
  if (res >= 0)
    data[res] = 0;
  if (recvfd == -1)
  {
    if (debug)
      fprintf(stderr, "failure: %s\n", data);
    return 1;
  }
  if (debug)
    fprintf(stderr, "got fd %d\n", recvfd);
  // transmit fd to fuse
  int client_fd = std::stoi(getenv("_FUSE_COMMFD"));
  if (debug)
    fprintf(stderr, "transmiting fd to %d\n", client_fd);
  char sendchar=0;
  write_fd(client_fd, &sendchar, 1, recvfd);
  return 0;
}