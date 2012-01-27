//
// ---------- header ----------------------------------------------------------
//
// project      Infinit coding standard
//
// license      infinit
//
// author       Raphael Londeix   [Thu 26 Jan 2012 06:32:31 PM CET]
//

// ---------------------------------------------------------------------------
// Règles générales:
//
//      - Jamais de tabulation
//      - l'indentation fait deux espaces
//      - Les lignes ne dépassent pas 80 colonnes
//      - les fonctions ne dépassent pas 50 lignes
//      - JAMAIS DE UNDERSCORE
//

#ifndef MONNAMESPACE_CODINGSTANDARD_HH // NAMESPACE_NOMDUFICHIER_HH
#define MONNAMESPACE_CODINGSTANDARD_HH

//
// ---------- includes --------------------------------------------------------
//



#include <et enfin les notres> //deps infinit

#include <idiom/Close.hh>
# include <d'abord les include systeme>
# include <ensuite les lib>
#include <idiom/Open.hh>


namespace monnamespace // lowercased
{

//
// ---------- classes ---------------------------------------------------------
//


  ///
  /// La doc de la classe ici
  ///
  class CodingStandard : // pas de verbe, CamelCased
    public JHeriteDe,
    public PuisDe
  {
    //
    // D'abord les types
    //
  public:
    typedef int     MonTypePublic;
  protected:
    typedef int     MonTypeProtégé;
  private:
    typedef int     MonTypePrivé;


    //
    // Ensuite les variables et fonctions membres statiques
    //
  public:
    static int      FaireUnTruc();    // comporte sujet et un verbe, CamelCased

    //
    // Enfin les fonctions membres
    // Les constructeurs d'abord (mettre la forme canonique en avant)
  public:
    CodingStandard();
    void            Methode();
  private:
    void            JeSuisPrivée();

    //
    // Les variables
    //
  public:
    int             mavariablepublique;
  protected:
    int             mavariableprotégée;
  private:
    int             mavariableprivée;

  };

}



#endif /* ! CODINGSTANDARD_HH */


