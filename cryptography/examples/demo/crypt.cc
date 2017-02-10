#if !defined(INFINIT_WINDOWS)
# include <termios.h>
#endif

#include <elle/Exception.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/finally.hh>

#include <cryptography/SecretKey.hh>
#include <cryptography/Error.hh>

static
void
echo_mode(bool enable)
{
#if defined(INFINIT_WINDOWS)
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hStdin, &mode);
  if (!enable)
    mode &= ~ENABLE_ECHO_INPUT;
  else
    mode |= ENABLE_ECHO_INPUT;
  SetConsoleMode(hStdin, mode );
#else
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if(!enable)
    tty.c_lflag &= ~ECHO;
  else
    tty.c_lflag |= ECHO;
  (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

static
elle::cryptography::SecretKey
key(bool verify)
{
  using namespace elle::cryptography;
  std::array<std::string, 2> passphrases;
  {
    elle::SafeFinally restore_echo([] { echo_mode(true); });
    echo_mode(false);
    std::cout << "Please use a combination of upper and lower case letters and numbers." << std::endl;
    auto get_key = [&] (std::string& p) {
      std::cout << "Enter passphrase: ";
      std::cout.flush();
      std::getline(std::cin, p);
      std::cout << std::endl;
    };
    if (verify)
    {
      for (std::string& p: passphrases)
        get_key(p);
      if (passphrases[0] != passphrases[1])
        elle::err("Passphrases do not match");
    }
    else
    {
      get_key(passphrases[0]);
    }
  }
  return SecretKey(passphrases[0]);
}

int
main(int argc, char* argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " [--encipher|--decipher] message\n";
    return 1;
  }
  try
  {
    if (argv[1] == std::string{"--encipher"})
    {
      auto secret = key(true);
      auto code = secret.encipher(argv[2]);
      std::cout << elle::format::hexadecimal::encode(elle::ConstWeakBuffer(code)) << std::endl;
    }
    else if (argv[1] == std::string{"--decipher"})
    {
      auto secret = key(false);
      auto code = elle::format::hexadecimal::decode(argv[2]);
      try
      {
        std::cout << secret.decipher(code) << std::endl;
      }
      catch (elle::cryptography::Error const&)
      {
        elle::err("Invalid password");
      }
    }
    else
      // Raise an elle::Error with text formatted.
      elle::err("unknown mode %s", argv[1]);
    return 0;
  }
  catch (std::exception const& e)
  {
    std::cerr << "fatal error: " << e.what() << '\n';
    return 1;
  }
}
