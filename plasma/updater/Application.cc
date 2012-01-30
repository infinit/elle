
#include <cassert>
#include <iostream>

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::updater;

Application::Application(int ac, char** av) :
  QApplication(ac, av, false),
  _to_download(),
  _network_access_manager(0),
  _has_list(false)
{}

Application::~Application()
{
  delete this->_network_access_manager;
  this->_network_access_manager = 0;
}

int Application::Exec()
{
  this->_network_access_manager = new QNetworkAccessManager(this);
  this->connect(this->_network_access_manager,
                SIGNAL(finished(QNetworkReply*)),
                SLOT(_OnDownloadFinished(QNetworkReply*)));
  std::cout << "Checking out " << INFINIT_RELEASE_URI << std::endl;
  auto reply = this->_network_access_manager->get(QNetworkRequest(QUrl(INFINIT_RELEASE_URI)));

  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
          this, SLOT(_OnDownloadError(QNetworkReply::NetworkError)));

  return this->exec();
}

void Application::_OnDownloadFinished(QNetworkReply* reply)
{
  assert(reply != 0);
  if (reply->error() == QNetworkReply::NoError)
    {
      bool result;

      if (this->_has_list)
        result = this->_ProcessResource(*reply);
      else
        result = this->_ProcessResourceList(*reply);
      if (!result)
        this->quit();
    }
  else
    {
      std::cerr << "An error occured, ignoring replies\n";
    }
  reply->deleteLater();
}


void Application::_OnDownloadError(QNetworkReply::NetworkError error)
{
  std::cout << "Got error code = " << error << std::endl;
  this->exit(1);
}

bool Application::_ProcessResource(QNetworkReply& reply)
{
  assert(this->_has_list);
  return true;
}

bool Application::_ProcessResourceList(QNetworkReply& reply)
{
  assert(!this->_has_list);
  this->_has_list = true;
  return true;
}
