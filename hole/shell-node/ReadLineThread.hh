#ifndef READLINETHREAD_HH
# define READLINETHREAD_HH

# include <QThread>

class ReadLineThread : public QThread
{
  Q_OBJECT;

public:
  ReadLineThread(QObject * parent = 0);

signals:
  void ReadLine(QString line);

protected:
  virtual void run();
};

#endif /* !READLINETHREAD_HH */
