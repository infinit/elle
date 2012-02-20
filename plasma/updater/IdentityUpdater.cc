//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphael Londeix   [Fri 17 Feb 2012 05:49:28 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <iostream>

#include "IdentityUpdater.hh"

using namespace plasma::updater;

//
// ---------- contructors & descructors ---------------------------------------
//

IdentityUpdater::IdentityUpdater(QApplication& app) :
  _api(app),
  _loginDialog()
{}

//
// ---------- methods  --------------------------------------------------------
//

void IdentityUpdater::Start()
{
  this->_loginDialog.show();
  this->connect(
    &this->_loginDialog, SIGNAL(accepted()),
    this, SLOT(_DoLogin())
  );
}
void IdentityUpdater::_DoLogin()
{
  std::string login, password;
  this->_loginDialog.GetLoginPassword(login, password);
  this->_api.Login(
    login, password,
    std::bind(&IdentityUpdater::_OnLogin, this, std::placeholders::_1),
    std::bind(&IdentityUpdater::_OnError, this, std::placeholders::_1)
  );
}

void IdentityUpdater::_OnLogin(plasma::metaclient::LoginResponse const& response)
{
  if (response.success)
    {
      std::cout << "Login success: "
                << response.fullname << ' '
                << response.token
                << '\n';
    }
  else
    {
      std::cerr << "Login error: " << response.error << '\n';
    }
}

void IdentityUpdater::_OnError(plasma::metaclient::MetaClient::Error error)
{
  std::cout << "ERROR: " << (int)error << "\n";
}

