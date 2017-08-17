#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

#include <elle/attribute.hh>

namespace elle
{
  namespace system
  {
    class XDG
    {
    public:
      /// Create a XDG wrapper for the current product.
      ///
      /// The different XDG folders will be followed by infinit/${product_name}.
      /// The default values can be overwritten using {XDG,${prefix}}_{TYPE}
      /// environment variables.
      ///
      /// For rappel: https://wiki.debian.org/XDGBaseDirectorySpecification
      ///
      /// e.g.
      ///
      /// auto const xdg = XDG("memo");
      /// // xdg.share_dir() == $HOME/.local/share/infinit/memo.
      ///
      /// @param company The name of the company.
      /// @param product The name of the product.
      /// @param environment_variable_prefix Environment variable prefix (will
      ///                                    be upper cased).
      XDG(std::string const& company,
          std::string const& product,
          boost::optional<std::string> const& environment_variable_prefix = {});

      XDG(XDG const&) = default;

    public:
      boost::filesystem::path
      home_dir() const;

    public:
      boost::filesystem::path
      _xdg(std::string const& type,
           boost::filesystem::path const& def) const;

    protected:
      boost::filesystem::path
      _xdg_home(std::string const& type,
                boost::filesystem::path const& def) const;

    public:
      boost::filesystem::path
      cache_dir() const;

      boost::filesystem::path
      config_dir() const;

      boost::filesystem::path
      data_dir() const;

      boost::filesystem::path
      tmp_dir() const;

      boost::filesystem::path
      runtime_dir(boost::optional<std::string> fallback = {}) const;

      boost::filesystem::path
      state_dir() const;

    private:
      ELLE_ATTRIBUTE(std::string, company);
      ELLE_ATTRIBUTE(std::string, product);
      ELLE_ATTRIBUTE(std::string, environment_variable_prefix);
    };
  }
}
