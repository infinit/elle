#ifndef WIDGET_HH
# define WIDGET_HH

# include <QObject>

class Widget:
  public QObject
{
public:
  Widget();

private:
  Q_OBJECT
};

#endif
