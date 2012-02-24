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
  std::cout << "IdentityUpdater::Start()\n";
  this->_loginDialog.show();
  this->connect(
    &this->_loginDialog, SIGNAL(accepted()),
    this, SLOT(_DoLogin())
  );
}
void IdentityUpdater::_DoLogin()
{
  std::cout << "DO LOGIN\n";
  std::string login, password;
  this->_loginDialog.GetLoginPassword(login, password);
  if (!login.size() || !password.size())
    {
      this->_loginDialog.SetErrorMessage("Wrong login/password");
      this->_loginDialog.show();
      return;
    }
  this->_api.Login(
    login, password,
    std::bind(&IdentityUpdater::_OnLogin, this, std::placeholders::_1),
    std::bind(&IdentityUpdater::_OnError, this, std::placeholders::_1)
  );
  this->_loginDialog.setDisabled(true);
  this->_loginDialog.show();
}

void IdentityUpdater::_OnLogin(plasma::metaclient::LoginResponse const& response)
{
  this->_loginDialog.setDisabled(false);
  if (response.success)
    {
      std::cout << "Login success: "
                << response.fullname << ' '
                << response.email << ' '
                << response.token
                << '\n';
      this->_loginDialog.hide();
      emit identityUpdated(response.token, response.identity);
    }
  else
    {
      std::cerr << "Login error: " << response.error << '\n';
      this->_loginDialog.SetErrorMessage(response.error);
    }
}

void IdentityUpdater::_OnError(plasma::metaclient::MetaClient::Error error)
{
  std::cout << "ERROR: " << (int)error << "\n";
  this->_loginDialog.SetErrorMessage(
    "An error occured, please check your internet connection"
  );
  this->_loginDialog.setDisabled(false);
}

