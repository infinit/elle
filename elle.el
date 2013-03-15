(set-font-size 80)

(c++-project
 '(
   "elle/src"
   "cryptography/sources"
   )
 "_build/linux64"
 "./drake -j 4 //build"
 ""
 '()
)
