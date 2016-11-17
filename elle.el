(require 'cc-mode)

(defun c-lineup-C-comments-boxes (arg)
  (let ((box (save-excursion
               (goto-char (cdr arg))
               (looking-at "/\\*-*\\.$"))))
    (if box
        0
      (c-lineup-C-comments arg))))

(add-to-list 'c-style-alist
             '("infinit"
               (c-basic-offset . 2)
               (c-comment-only-line-offset . 0)
               (c-hanging-braces-alist     . ((substatement-open before after)))
               (c-offsets-alist . ((topmost-intro        . 0)
                                   (substatement         . +)
                                   (substatement-open    . 0)
                                   (case-label           . +)
                                   (access-label         . -)
                                   (inclass              . +)
                                   (inline-open          . 0)
                                   (c                    . c-lineup-C-comments-boxes)))))
(setq c-default-style "infinit")

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
