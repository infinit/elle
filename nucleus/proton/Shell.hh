#ifndef NUCLEUS_PROTON_SHELL_HH
# define NUCLEUS_PROTON_SHELL_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>

namespace nucleus
{
  namespace proton
  {
    /// XXX
    class Shell:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// XXX ownership or shared_ptr
      template <typename T>
      Shell(T* node);
      /// Destructor.
      ~Shell();

      /*--------.
      | Methods |
      `--------*/
    public:
      // XXX mode, encrypt, decrypt

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /// XXX
      ELLE_ATTRIBUTE_R(Breed, breed);
      /// XXX
      ELLE_ATTRIBUTE(Node*, node);
      /// XXX
      ELLE_ATTRIBUTE(cryptography::Cipher*, cipher);
    };
  }
}

# include <nucleus/proton/Shell.hxx>

#endif
