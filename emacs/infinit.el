;; Infinit emacs configuration

;;
;; defines the comment characters used by each language.
;;
(setq
 infinit-c-list                 '((o . "/*") (i . " *") (c . " */"))
 infinit-css-list               '((o . "/*") (i . " *") (c . " */"))
 infinit-cpp-list               '((o . "//") (i . "//") (c . "//"))
 infinit-ada-list               '((o . "--") (i . "--") (c . "--"))
 infinit-java-list              '((o . "//") (i . "//") (c . "//"))
 infinit-latex-list             '((o . "%") (i . "%") (c . "%"))
 infinit-python-list            '((o . "#") (i . "#") (c . "#"))
 infinit-lisp-list              '((o . ";;") (i . ";;") (c . ";;"))
 infinit-scheme-list            '((o . ";;") (i . ";;") (c . ";;"))
 infinit-xdefault-list          '((o . "!!") (i . "!!") (c . "!!"))
 infinit-makefile-list          '((o . "#") (i . "#") (c . "#"))
 infinit-shell-list             '((o . "#") (i . "#") (c . "#"))
 infinit-perl-list              '((o . "#") (i . "#") (c . "#"))
 infinit-text-list              '((o . "#") (i . "#") (c . "#"))
 infinit-fundamental-list       '((o . "#") (i . "#") (c . "#"))
 infinit-html-list              '((o . "<!--") (i . "  --") (c . "-->"))
 infinit-nroff-list             '((o . "\\\"") (i . "\\\"") (c . "\\\""))
 )

;;
;; links a language to a comment style.
;;
(setq infinit-modes-list
      '(("C"                  . infinit-c-list)
        ("CSS"                . infinit-c-list)
        ("C++"                . infinit-cpp-list)
        ("C++/l"              . infinit-cpp-list)
        ("C++/lw"             . infinit-cpp-list)
        ("Ada"                . infinit-ada-list)
        ("Java"               . infinit-java-list)
        ("LaTeX"              . infinit-latex-list)
        ("latex"              . infinit-latex-list)
        ("TeX"                . infinit-latex-list)
        ("BibTeX"             . infinit-latex-list)
        ("Python"             . infinit-python-list)
        ("Lisp"               . infinit-lisp-list)
        ("Lisp Interaction"   . infinit-lisp-list)
        ("Scheme"             . infinit-scheme-list)
        ("Emacs-Lisp"         . infinit-lisp-list)
        ("xdefault"           . infinit-xdefault-list)
        ("Makefile"           . infinit-makefile-list)
        ("GNUmakefile"        . infinit-makefile-list)
        ("Text"               . infinit-text-list)
        ("Fundamental"        . infinit-fundamental-list)
        ("Conf[Unix]"         . infinit-fundamental-list)
        ("HTML"               . infinit-html-list)
        ("Nroff"              . infinit-nroff-list)
        ("Shell-script"       . infinit-shell-list)
        ("Perl"               . infinit-perl-list)
        ("CPerl"              . infinit-perl-list)))

;;
;; gets a special characters sequence: o, i or c for open-comment, in-comment
;; and close-comment.
;;
(defun infinit-comment (a)
  (interactive)
  (cdr (assoc a (eval (cdr (assoc mode-name infinit-modes-list))))))

;;
;; generates the file header.
;;
(defun infinit-generate-header ()
  (interactive)
  (goto-char (point-min))

  (insert (infinit-comment 'o))
  (newline)
  (insert (concat
           (infinit-comment 'i)
           " ---------- header "
           (make-string
            (- 79 (+ (length (infinit-comment 'i))
                     (length " ---------- header ")))
            ?-)))
  (newline)
  (insert (infinit-comment 'i))
  (newline)
  (insert (concat
           (infinit-comment 'i) " project       "
           (replace-regexp-in-string ".*/infinit/\\([^/]\+\\)/?.*" "\\1" (buffer-file-name))))
  (newline)
  (insert (infinit-comment 'i))
  (newline)
  (insert (concat (infinit-comment 'i) " license       infinit"))
  (newline)
  (insert (infinit-comment 'i))
  (newline)
  (insert (concat
           (infinit-comment 'i)
           " author        " (getenv "NAME") "   [" (downcase (current-time-string)) "]"))
  (newline)
  (insert (infinit-comment 'c))
  (newline)
  )

(add-hook 'find-file-hooks
          (lambda ()
            (when (string-match ".*/infinit/.*" (buffer-file-name))
              (when (memq major-mode '(c-mode c++-mode))
                (c-set-style "gnu")
                (highlight-regexp "XXX" "hi-red-b"))
              (when (equal (point-min) (point-max))
                (infinit-generate-header)
                (newline)
                (goto-line 11)))))
