#include <elle/Elle.hh>

class ZZZ
{
public:
  ZZZ():
    v(21)
  {
  }

Status foo()
{
  enter();

  this->v *= 2;

  printf("foo(%u) this[0x%x] v[0x%x]\n", this->v, this, &this->v);

  if (this->v == 84)
    {
      printf("[save]\n");
      this->save0();
      printf("[/save]\n");
    }
  else if (this->v == 168)
    {
      printf("[restore]\n");
      this->restore0();
      printf("[/restore]\n");
    }

  printf("/foo\n");

  leave();
}

  void save0()
  {
    char buffer[42];

    strcpy(buffer, "suce mon cul");

    save1();

    if (strcmp(buffer, "suce mon cul") != 0)
      printf("ERROR SAVE0\n");
  }

  void save1()
  {
    int i = 1;
    char c = '*';
    double d = 2.49;

    save2();

    if ((i != 1) ||
	(c != '*') ||
	(d != 2.49))
      printf("ERROR SAVE1");
  }

  void restore0()
  {
    char useless[149];

    useless[45] = '*';

    restore1();

    if (useless[45] != '*')
      printf("ERROR RESTORE0");
  }

  void save2()
  {
    Event event;

    printf("SAVE2 is blocking\n");

    event.identifier = 12345;

    if (Fiber::Wait(event) == StatusError)
      show();

    printf("SAVE2 has been released\n");
  }

  void restore1()
  {
    Event event;

    event.identifier = 12345;

    printf("RESTORE1 awakening\n");

    if (Fiber::Awaken(event) == StatusError)
      show();

    printf("RESTORE1 completing\n");
  }

  int v;
};

int main(int argc, char** argv)
{
  ZZZ zzz;
  Entrance	f(Method<>(&zzz, &ZZZ::foo));

  Application::Setup(argc, argv);
  Elle::Initialize();

  Timer timer;

  timer.Create(Timer::ModeRepetition,
	       f);
  timer.Start(2000);

  Application::Process();
}
