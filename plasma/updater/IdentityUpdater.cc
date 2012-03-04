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

#include <QDir>

#include "plasma/common/resources.hh"

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
  std::string login, password;
  this->_loginDialog.GetLoginPassword(login, password);
  if (!login.size() || !password.size())
    {
      this->_loginDialog.SetErrorMessage("Wrong login/password");
      this->_loginDialog.show();
      return;
    }

    {
      std::cout << "DO LOGIN CALL\n";
      using namespace std::placeholders;
      this->_api.Login(
          login, password,
          std::bind(&IdentityUpdater::_OnLogin, this, _1),
          std::bind(&IdentityUpdater::_OnError, this, _1, _2)
      );
    }
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
      this->_identity = response.identity;
      this->_token = response.token;
      this->_loginDialog.hide();

      QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));

        {
          QFile f(homeDirectory.filePath("infinit.idy"));
          if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
              f.write(response.identity.c_str());
              f.close();
            }
          else
            std::cerr << "Could not create 'infinit.idy'.\n";
        }
        {
          QFile f(homeDirectory.filePath("infinit.dic"));
          if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
              f.write("0b000b00"); // XXX This is an empty dic,
              f.close();
            }
          else
            std::cerr << "Could not create 'infinit.idy'.\n";
        }



      if (!homeDirectory.exists("infinit.ppt"))
        {
          std::cout << "Checking out a new passport.\n";
          this->_UpdatePassport();
        }
      else
        {
          std::cout << "Found a passport file.\n";
          emit identityUpdated();
        }
    }
  else
    {
      std::cerr << "Login error: " << response.error << '\n';
      this->_loginDialog.SetErrorMessage(response.error);
    }
}

void IdentityUpdater::_OnError(plasma::metaclient::MetaClient::Error error,
                               std::string const& error_string)
{
  std::cout << "ERROR: " << (int)error << ": " << error_string << "\n";
  this->_loginDialog.SetErrorMessage(
    "An error occured, please check your internet connection"
  );
  this->_loginDialog.setDisabled(false);
}

void IdentityUpdater::_UpdatePassport()
{
  using namespace std::placeholders;
  this->_api.CreateDevice("default device name", "127.0.0.1",
      std::bind(&IdentityUpdater::_OnDeviceCreated, this, _1)
  );
}

void IdentityUpdater::_OnDeviceCreated(meta::CreateDeviceResponse const& res)
{
  std::cout << "Created device " << res.created_device_id
            << " with passport: " << res.passport << "\n";

  QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));

  QFile f(homeDirectory.filePath("infinit.ppt"));
  if (f.open(QIODevice::WriteOnly | QIODevice::WriteOnly))
    {
      f.write(res.passport.c_str());
      f.flush();
      f.close();
      emit identityUpdated();
    }
  else
    std::cerr << "Could not write into '"
              << homeDirectory.filePath("infinit.ppt").toStdString()
              << "'.\n";
}
