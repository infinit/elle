#ifndef TEST_HH
# define TEST_HH

# include <QObject>

class Test: public QObject
{
public:
  Test();
  void trigger();

signals:
  void signal();

private slots:
  void slot();

private:
  Q_OBJECT;
};

#endif
