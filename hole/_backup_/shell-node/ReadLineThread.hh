#ifndef READLINETHREAD_HH
# define READLINETHREAD_HH

# include <QThread>

class ReadLineThread : public QThread
{
  Q_OBJECT;

public:
  ReadLineThread(QObject * parent = 0);

public slots:
  void ReadLine();

signals:
  void LineRead(QString line);

protected:
  virtual void run();
};

#endif /* !READLINETHREAD_HH */
