#include <elle/Elle.hh>
#include <elle/system/Frame.hh>

Status foo()
{
  printf("foo\n");

  frame(Frame::Event);

  Frame::Show();

  frame(&Frame::Application);

  printf("/foo\n");
}

int main(int argc, char** argv)
{
  Function<> f(&foo);

  Application::Setup(argc, argv);
  Elle::Initialize();
  Frame::Initialize();

  Timer timer;

  timer.Create(Timer::ModeRepetition,
	       f);
  timer.Start(2000);

  Application::Process();

  printf("END\n");
}
